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
use Cwd qw(getcwd);
use Getopt::Long;
use File::Basename;
use File::Find;
use File::Glob;
use File::Slurp;

#  Set up basic TECO command we will issue for each test script.

my $teco = 'teco -n -I"\'0,128ET\'"';
my %dirs;
my $okay   = q{};
my $ntests = 0;

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

printf "$ntests test script%s executed\n", $ntests == 1 ? q{} : 's';

exit;

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
            my ( $abstract, $expect, $options ) = read_header( $dir, $file );

            if ( defined $abstract && defined $expect )
            {
                test_file( $file, $abstract, $expect, $options );
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

        my ( $abstract, $expect, $options ) = read_header( $dir, $file );

        if ( defined $abstract && defined $expect )
        {
            test_file( $file, $abstract, $expect, $options );
        }

        chdir $cwd or croak "Can't change directory to $cwd";
    }

    return;
}

sub read_header
{
    my ( $dir, $file ) = @_;

    open my $fh, '<', $file or croak "Can't open file $file";

    my $abstract = <$fh>;
    my $expect   = <$fh>;
    my $options  = <$fh>;

    close $fh or croak "Can't close file $file";

    if ( !defined $abstract )
    {
        return ( undef, undef, undef );
    }

    if ( $abstract !~ s/! \s+ TECO-64 \s test \s script: \s (.+) \s !\n/$1/msx )
    {
        return ( undef, undef, undef );
    }

    # Find out whether we expect success or failure

    if ( !defined $expect
        || $expect !~ s/! \s+ Expect: \s+ (.+) \s !\n/$1/msx )
    {
        print "[$file] Test script is missing expectations\n";

        return ( undef, undef, undef );
    }

    # Look for possible additional options for TECO command line. This
    # line is not required in the test script, so don't use it if it's
    # not in the expected form.

    if ( !defined $options
        || $options !~ s/! \s+ Options: \s+ (.+) \s !\n/$1/msx )
    {
        $options = q{};
    }

    return ( $abstract, $expect, $options );
}

sub test_file
{
    my ( $file, $abstract, $expect, $options ) = @_;
    my $command;

    if ( $file =~ /.+[.]test/msx )
    {
        $command = "$teco $options -X -E $file";
    }
    elsif ( $file =~ /.+[.]key/msx )
    {
        $command = "$teco $options < $file";
    }

    my $output = qx/$command/;    # Execute command and capture output

    chomp $output;

    ++$ntests;

    my $commands = q{};
    my $diff;

    if ( $abstract =~ /(.+) -- (.+)/ms )
    {
        $abstract = $1;
        $commands = $2;
    }

    # We expect the test to either pass or fail; if pass, then there may be
    # a file name that we will use to look for differences in the output.

    if ( $expect =~ /^PASS/msx )
    {
        if ( $expect =~ / (PASS) \s \[ (.+) \] /msx )
        {
            $expect = $1;
            $diff   = $2;
        }
    }
    elsif ( $expect eq 'FAIL' )
    {
        $expect = 'fail';
    }
    else
    {
        print "$file: Invalid header format\n";

        return;
    }

    my $report = sprintf '%17s %-45s %-15s %s ', "[$file]", $abstract,
      $commands, $expect;

    if ( $expect eq 'PASS' && $output =~ /PASS/ms )
    {
        if ( defined $diff )
        {
            my $expected = read_file($diff);

            chomp $expected;

            if ( $expected ne $output )
            {
                print "$report-> DIFF: $diff\n";

                return;
            }
        }
    }
    elsif ( $expect ne 'fail' || $output =~ /PASS/ms )
    {
        print "$report-> ERROR: $output\n";
        print "output = $output\n";

        return;
    }

    # Only output normal results if requested

    if ($okay)
    {
        printf "$report-> OK\n";
    }

    return;
}

sub wanted
{
    my $file = $_;

    # Only look for TECO script files

    if ( $file !~ /.+[.]test$/msx && $file !~ /.+[.]key$/msx )
    {
        return;
    }

    # Create a hash of arrays, with directory names as keys, and the matching
    # files as elements in the array used as the hash's value.

    push @{ $dirs{$File::Find::dir} }, $file;

    return;
}
