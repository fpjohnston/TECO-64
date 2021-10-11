#!/usr/bin/perl -w

#
#  run_tests.pl - Run scripts for testing TECO text editor.
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
use Getopt::Long;
use File::Basename;
use File::Find;
use File::Glob;
use File::Slurp;

#  Set up basic TECO command we will issue for each test script.

my $verbose  = q{};
my $nscripts = 0;
my $all_tests = 0;
my $target = 'teco64';

#
#  Parse our command-line options
#

GetOptions(
    'target=s' => \$target,
    'verbose'  => \$verbose,
);

$target = lc $target;

if ($target eq 'vms' || $target eq 'teco-32')
{
    $target = 'teco32';
}
elsif ($target eq 'teco-64')
{
    $target = 'teco64';
}
elsif ($target ne 'teco32' && $target ne 'teco64' && $target ne 'tecoc')
{
    print "Invalid target: $target\n";

    exit;
}

croak "Input directory required" if ( $#ARGV != 0 );

my $indir = $ARGV[0];

#
#  Main program start
#

chdir $indir or croak "Can't change directory to $indir\n";

opendir my $dir, '.' or croak "Cannot open directory: $!\n";

my @files = readdir($dir);

closedir $dir;

@files = sort @files;

my $command;

# Execute each test script we find.

foreach my $file (@files)
{
    next if $file !~ /\.tec$/;

    unlink("out_1.tmp", "out_2.tmp");

    open FH, ">in_1.tmp" or die "Can't create in_1.tmp: $!\n";
    print FH "hello, world!\n";
    close FH;

    open FH, ">cmd_1.tmp" or die "Can't create cmd_1.tmp: $!\n";
    print FH "@^A/hello, world!/ 10^T";
    close FH;

    my ($report, $expects) = read_header($file);

    next unless defined $report;

    my $actual;

    if ($target eq 'tecoc')
    {
        my $teco = 'tecoc < ' . $file . ' 2>&1';

        $actual = qx/$teco/;            # Execute command and capture output

        if ( $file =~ /^EG_\d\d\.tec$/ms )
        {
            local $ENV{TEC_INIT}    = 'TECO_INIT';
            local $ENV{TEC_LIBRARY} = 'TECO_LIBRARY';
            local $ENV{TEC_MEMORY}  = 'TECO_MEMORY';
            local $ENV{TEC_VTEDIT}  = 'TECO_VTEDIT';

            $actual = qx/$teco/;        # Execute command and capture output
        }
        else
        {
            $actual = qx/$teco/;        # Execute command and capture output
        }
    }
    elsif ($target eq 'teco32')
    {
        croak "$target target not implemented yet\n";
    }
    elsif ($target eq 'teco64')
    {
        my $teco = 'teco -n -I"\'0,128ET\'" < ' . $file . ' 2>&1';

        if ( $file =~ /^EG-\d\d\.tec$/ms )
        {
            local $ENV{TECO_INIT}    = 'TECO_INIT';
            local $ENV{TECO_LIBRARY} = 'TECO_LIBRARY';
            local $ENV{TECO_MEMORY}  = 'TECO_MEMORY';
            local $ENV{TECO_VTEDIT}  = 'TECO_VTEDIT';

            $actual = qx/$teco/;        # Execute command and capture output
        }
        else
        {
            $actual = qx/$teco/;        # Execute command and capture output
        }
    }
    else
    {
        croak "Unknonwn target: $target\n";
    }

    chomp $actual;

    check_test($file, $report, $expects, $actual);
}

# Clean up any temp. files we may have created.

opendir $dir, '.' or croak "Cannot open directory: $!\n";

@files = readdir($dir);

closedir $dir;

foreach my $file (@files)
{
    next if $file !~ /\.tmp/;

    unlink $file;
}

chdir '..' or croak "Can't change directory to '..'\n";

printf "Total of $all_tests test%s in $nscripts script%s\n",
    $all_tests == 1 ? q{} : 's', $nscripts == 1 ? q{} : 's';

exit;

sub check_test
{
    my ( $file, $report, $expects, $actual ) = @_;

    my $diff;
    my $expected;

    # We expect the test to either pass or fail; if pass, then there may be
    # a file name that we will use to look for differences in the output.

    if ( $expects=~ / ^PASS (\s \[ (.+) \])? /msx )
    {
        $expects= 'pass';
        $diff   = $2;
    }
    elsif ( $expects=~ / ^FAIL /msx )
    {
        $expects= 'fail';
    }
    elsif ( $expects=~ /^ ([?][[:alpha:]]{3}) (\s \[ (.+) \])? /msx )
    {
        $expects= $1;
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

    ++$nscripts;

    if ($actual =~ /Aborted/)
    {
        # This is handled specially without regard to the desired result,
        # because it was found that some tests could complete successfully,
        # but abort on exit, due to such things as stack smashing. So we
        # first check to see if there was an abort in the output, and only
        # then check for desired success or failure.

        printf "%s ABORTED\n", $report;
    }
    elsif ( $expects eq 'pass' )
    {
        # Here if test should succeed

        if ( $actual !~ /!PASS!/ms )
        {
            if ($actual =~ /(\?\w\w\w)\s/)
            {
                # Test encountered TECO error

                printf "%s %s\n", $report, $1;
            }
            elsif ( $actual =~ /!FAIL!/ )
            {
                # Test failed when it should have succeeded

                printf "%s FAIL\n", $report;
            }
            else
            {
                # Test encountered unknown error

                printf "%s UNKNOWN\n", $report;
            }
        }
        elsif ( defined $diff && $expected ne $actual )
        {
            printf "%s DIFF\n", $report;
        }
        elsif ($verbose)
        {
            printf "%s OK\n", $report;
        }
    }
    elsif ( $expects eq 'fail' )
    {
        # Here if test  succeed

        if ( $actual !~ /!FAIL!/ms )
        {
            if ($actual =~ /(\?\w\w\w)\s/)
            {
                # Test encountered TECO error

                printf "%s %s\n", $report, $1;
            }
            elsif ( $actual =~ /!PASS!/ )
            {
                # Test succeeded when it should have failed

                printf "%s PASS\n", $report;
            }
            else
            {
                # Test encountered unknown error

                printf "%s UNKNOWN\n", $report;
            }
        }
        elsif ($verbose)
        {
            printf "%s OK\n", $report;
        }
    }
    else                                # Here if test failed or should fail
    {
        my $match = '! Expects: ' . quotemeta $expects . ' !';

        $actual =~ s/$match//ms;

        if ( index( $actual, $expects) < 0 )
        {
            if ($actual =~ /(\?\w\w\w)\s/)
            {
                # Test encountered unexpected TECO error

                printf "%s %s\n", $report, $1;
            }
            elsif ( $actual =~ /!FAIL!/ )
            {
                # Test failed when it should have issued TECO error

                printf "%s FAIL\n", $report;
            }
            elsif ( $actual =~ /!PASS!/ )
            {
                # Test succeeded when it should have failed

                printf "%s PASS\n", $report;
            }
            else
            {
                # Test encountered unknown error

                printf "%s UNKNOWN\n", $report;
            }
        }
        elsif ($verbose)
        {
            # Test encountered expected TECO error

            printf "%s OK\n", $report;
        }
    }

    return;
}

sub read_header
{
    my ( $file ) = @_;

    open my $fh, '<', $file or croak "Can't open file: $file";

    chomp(my @lines = <$fh>);

    close $fh or croak "Can't close file $file";

    my $abstract;
    my $commands;
    my $expects;
    my $ntests = 0;

    foreach my $line (@lines)
    {
        if ( $line =~ m[! \s+ TECO \s test: \s (.+?) \s (w/ (.+) \s )? ! ]x )
        {
            $abstract = $1;
            $commands = $3;
        }
        elsif ( $line =~ /! \s Expects: \s (.+?) \s (\[.+\] \s)? ! /x )
        {
            $expects = $1;

            if ($target eq 'tecoc' && ($expects eq '?FNF' || $expects eq '?ERR'))
            {
                $expects = '?SYS';
            }
        }
        elsif ($line =~ /Test:/)
        {
            ++$ntests;
        }
    }

    if (!defined $abstract)
    {
        return (undef, undef, undef, undef);
    }

    $all_tests += $ntests;

    croak "Can't find test commands in file: $file\n" unless defined $commands;
    croak "Can't find test expectations in file: $file\n" unless defined $expects;

    my $report = sprintf '%17s %-45s %-15s  (%2d)  %s ->', "[$file]", $abstract,
        $commands, $ntests, $expects;

    return ( $report, $expects );
}
