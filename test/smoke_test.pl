#!/usr/bin/perl -w

#
#  smoke_test.pl - Smoke-test script for testing TECO-64 text editor.
#
#  @copyright 2021 Franklin P. Johnston / Nowwith Treble Software
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIA-
#  BILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#  THE SOFTWARE.
#
################################################################################

use strict;
use warnings;
use version; our $VERSION = '1.0.0';

use Carp;
use Cwd qw(getcwd abs_path);
use Getopt::Long;
use File::Basename;
use File::Find;
use File::Glob;
use File::Slurp;

#  Set up basic TECO command we will issue for each test script.

my %dirs;
my $okay   = q{};
my $nscripts = 0;
my $all_tests = 0;
my $auxdir = dirname(abs_path($0)) . '/aux';

#
#  Parse our command-line options
#

GetOptions(
    'all'     => \$okay,
    'okay'    => \$okay,
    'verbose' => \$okay,
);

my @filespecs = @ARGV;

# Use current directory as default if no arguments

if ( $#filespecs + 1 == 0 )
{
    @filespecs = q{.};
}

#
#  Main program start
#

# Search specified file specifications

foreach my $filespec (@filespecs)
{
    if ( -d $filespec )    # If directory, recurse through tree
    {
        open_dir($filespec);
    }
    else                   #  else assume specific file(s)
    {
        open_file($filespec);
    }
}

printf "Executed $nscripts script%s with a total of $all_tests test%s\n",
    $nscripts == 1 ? q{} : 's', $all_tests == 1 ? q{} : 's';

exit;

sub check_tests
{
    my ( $file, $abstract, $commands, $expect, $output, $ntests ) = @_;

    chomp $commands;

    my $diff;
    my $expected;

    # We expect the test to either pass or fail; if pass, then there may be
    # a file name that we will use to look for differences in the output.

    if ( $expect =~ / ^PASS (\s \[ (.+) \])? /msx )
    {
        $expect = 'pass';
        $diff   = $2;
    }
    elsif ( $expect =~ /^ ([?][[:alpha:]]{3}) (\s \[ (.+) \])? /msx )
    {
        $expect = $1;
        $diff   = $3;
    }
    else
    {
        print "$file: Invalid expectations\n";

        return;
    }

    if ( defined $diff )
    {
        $expected = read_file($diff);

        chomp $expected;
    }

    my $report = sprintf '%17s %-45s %-15s  (%2d) ', "[$file]", $abstract,
        $commands, $ntests;

    ++$nscripts;

    if ( $expect eq 'pass' )
    {
        # Here if test should succeed

        if ( $output !~ /!PASS!/ms )
        {
            # Say error occurred, but only print first word in output

            my $len = index $output, q{ };

            if ( $len < 0 )
            {
                $len = length $output;
            }

            printf "%s %s -> %s\n", $report, $expect, substr $output, 0, $len;
        }
        elsif ( defined $diff && $expected ne $output )
        {
            printf "%s %s -> DIFF\n", $report, $expect;
        }
        elsif ($okay)
        {
            printf "%s %s -> OK\n", $report, $expect;
        }
    }
    else
    {
        # Here if test failed or should fail

        if ( index( $output, $expect ) < 0 )
        {
            # Say error occurred, but only print first word in output

            my $len = index $output, q{ };

            if ( $len < 0 )
            {
                $len = length $output;
            }

            printf "%s %s -> %s\n", $report, $expect, substr $output, 0, $len;
        }
        elsif ( defined $diff && $expected ne $output )
        {
            printf "%s %s -> DIFF\n", $report, $expect;
        }
        elsif ($okay)
        {
            printf "%s %s -> OK\n", $report, $expect;
        }
    }

    return;
}

sub make_test
{
    my ($file) = @_;
    my @lines = read_file($file);
    my $dir = dirname($0);

    $file = "/tmp/$file";

    open(FH, ">", $file) or croak "Can't open output file $file";

    foreach my $line (@lines)
    {
        if ( $line =~ /! \s Include: \s (.+) \s !/msx )
        {
            my $contents = read_file("$auxdir/$1");

            print FH $contents;
        }
        else
        {
            print FH $line;
        }
    }

    print FH "\e\e";                    # End macro with double ESCape

    close(FH);

    return $file;
}

sub open_dir
{
    my ($filespec) = @_;

    find( { wanted => \&wanted }, $filespec );

    # Sort the directories, then sort the files in each one

    foreach my $dir ( sort { uc $a cmp uc $b } keys %dirs )
    {
        my $cwd = getcwd;

        chdir $dir or croak "Can't change directory to $dir";

        my @files = @{ $dirs{$dir} };

        foreach my $file ( sort { uc $a cmp uc $b } @files )
        {
            my ( $abstract, $commands, $expect, $options, $ntests, $execution ) =
              read_header( $dir, $file );

            if ( defined $abstract && defined $expect )
            {
                my $output = run_test( $file, $options, $execution );

                if ( length $output )
                {
                    check_tests( $file, $abstract, $commands, $expect, $output,
                                 $ntests );
                }
            }
        }

        chdir $cwd or croak "Can't change directory to $cwd";
    }

    return;
}

sub open_file
{
    my ($filespec) = @_;

    my @files = glob $filespec;

    foreach my $file (@files)
    {
        my $dir = dirname($file);

        $file = basename($file);

        my $cwd = getcwd;

        chdir $dir or croak "Can't change directory to $dir";

        my ( $abstract, $commands, $expect, $options, $ntests, $execution ) =
          read_header( $dir, $file );

        if ( defined $abstract && defined $expect )
        {
            my $output = run_test( $file, $options, $execution );

            if ( length $output )
            {
                check_tests( $file, $abstract, $commands, $expect, $output,
                             $ntests );
            }
        }

        chdir $cwd or croak "Can't change directory to $cwd";
    }

    return;
}

sub read_header
{
    my ( $dir, $file ) = @_;

    open my $fh, '<', $file or croak "Can't open file $file";

    chomp(my @lines = <$fh>);

    my $abstract     = $lines[0];
    my $commands     = $lines[1];
    my $requirements = $lines[2];
    my $execution    = $lines[3];
    my $expect       = $lines[4];
    my $options      = $lines[5];

    close $fh or croak "Can't close file $file";

    if ( $abstract !~ s/! \s+ TECO \s test: \s (.+) \s ! /$1/msx )
    {
        return ( undef, undef, undef, undef, undef, undef );
    }

    if ( $commands !~ s/! \s+ Commands: \s+ (.+) \s ! /$1/msx )
    {
        print "[$file] No commands found in test script\n";

        exit;
    }

    # Find out whether we expect success or failure

    if ( $expect !~ s/! \s+ Expect: \s+ (.+) \s ! /$1/msx )
    {
        print "[$file] No expectations found in test script\n";

        exit;
    }

    if ( $requirements !~ s/! \s+ Requirements: \s+ (.+) \s ! /$1/msx )
    {
        print "[$file] No requirements found in test script\n";

        exit;
    }

    if ( $execution !~ s/! \s+ Execution: \s+ (.+) \s ! /$1/msx )
    {
        print "[$file] Don't know how to execute test script\n";

        exit;
    }

    my $ntests = 0;

    foreach my $line (@lines)
    {
        if ($line =~ /Test:/)
        {
            ++$ntests;
            ++$all_tests;
        }
    }

    # Look for possible additional options for TECO command line. This
    # line is not required in the test script, so don't use it if it's
    # not in the expected form.

    if ( $options !~ s/! \s+ Options: \s+ (.+) \s ! /$1/msx )
    {
        return ( $abstract, $commands, $expect, q{}, $ntests, $execution );
    }

    return ( $abstract, $commands, $expect, $options, $ntests, $execution );
}

sub run_test
{
    my ( $file, $options, $execution ) = @_;
    my $command;
    my $output;
    my $teco = 'teco -n -I"\'0,128ET\'"' . " $options";

    $file = make_test($file);

    if ( $execution =~ /Standard/ )
    {
        $command = "$teco -X -E $file";
    }
    elsif ( $execution =~ /Redirect/ )
    {
        $command = "$teco < $file";
    }
    else
    {
        print "Skipping $file\n";

        return q{};
    }

    # Use special environment variables for :EG commands so we don't have
    # to guess how the user might have set them up.

    if ( $file =~ /EG-\d\d\.tec/ms )
    {
        local $ENV{TECO_INIT}    = 'TECO_INIT';
        local $ENV{TECO_LIBRARY} = 'TECO_LIBRARY';
        local $ENV{TECO_MEMORY}  = 'TECO_MEMORY';
        local $ENV{TECO_VTEDIT}  = 'TECO_VTEDIT';

        $output = qx/$command/;    # Execute command and capture output
    }
    else
    {
        $output = qx/$command/;    # Execute command and capture output
    }

    chomp $output;

    return $output;
}

sub wanted
{
    my $file = $_;

    # Only look for TECO script files

    if ( $file !~ /.+[.]tec$/msx )
    {
        return;
    }

    # Create a hash of arrays, with directory names as keys, and the matching
    # files as elements in the array used as the hash's value.

    push @{ $dirs{$File::Find::dir} }, $file;

    return;
}
