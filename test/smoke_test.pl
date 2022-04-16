#!/usr/bin/perl -w
#
#  smoke_tests.pl - Create and run smoke tests for TECO text editor.
#
#  @copyright 2021-2022 Franklin P. Johnston / Nowwith Treble Software
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
use English qw( -no_match_vars );
use autodie qw( open close );
use Getopt::Long;
use File::Basename;
use File::Copy;
use File::Find;
use File::Glob;
use File::Slurp;
use File::Which;
use Readonly;

# Command-line options

my $init;
my $execute = 1;
my $make = 1;
my $orphans;
my $prove = 1;
my $skip;
my $target  = 'TECO-64';
my $verbose = q{};

my $testdir;
my $nscripts = 0;
my $nskipped = 0;
my $nfiles   = 0;
my $ntests   = 0;
my %scripts;
my %benchmark_files;
my @teco_files = ();

# Define tokens we will use to translate scripts into test sets,
# depending on the TECO we are targeting.

my %tokens   = (
    common => {
        'cmd1'  =>  q{cmd1.tmp},
        'cmd2'  =>  q{cmd2.tmp},
        'in1'   =>  q{in1.tmp},
        'in2'   =>  q{in2.tmp},
        'log1'  =>  q{log1.tmp},
        'out1'  =>  q{out1.tmp},
        'out2'  =>  q{out2.tmp},
        '"E'    =>  q{"E [[FAIL]] '},
        '"G'    =>  q{"G [[FAIL]] '},
        '"L'    =>  q{"L [[FAIL]] '},
        '"N'    =>  q{"N [[FAIL]] '},
        '"S'    =>  q{"S [[FAIL]] '},
        '"U'    =>  q{"U [[FAIL]] '},
        'I'     =>  q{13@I// 10@I//},
        '^T'    =>  q{13^T 10^T},
        'PASS'  => qq{@^A/!PASS!/ [[^T]]},
        'FAIL'  =>  q{[[error]] ^C},
        'error' => qq{@^A/!FAIL!/ [[^T]]},
        'enter' =>  q{0,128ET HK},
        'exit'  =>  q{^D EK HK [[PASS]] EX},
        '8'     =>  q{},
    },
    teco32 => {
        'bad'   =>  q{[TECO]},
        'expr'  =>  q{64},
        'LOOP'  =>  q{32},
        'Q'     =>  q{64},
        'EG'    =>  q{type hello.tmp}
    },
    teco64 => {
        '8'     =>  q{4096,0 ET},
        'bad'   =>  q{/dev/teco},
        'enter' =>  q{0,128ET HK 0E1 1,0E3},
        'expr'  =>  q{64},
        'LOOP'  =>  q{32},
        'Q'     =>  q{64},
        '^T'    =>  q{10^T},
        'EG'    =>  qq{echo 'hello, world!\n!PASS!'},
    },
    tecoc => {
        'bad'   =>  q{/dev/teco},
        'expr'  =>  q{63},
        'LOOP'  =>  q{31},
        'Q'     =>  q{21},
        'EG'    =>  qq{echo 'hello, world!\n!PASS!'},
    },
);

my @jabberwocky = (
    q{@} . 'I/Jabberwocky',
    'by Lewis Carroll',
    q{},
    '\'Twas brillig, and the slithy toves',
    '     Did gyre and gimble in the wabe:',
    'All mimsy were the borogoves,',
    '     And the mome raths outgrabe.',
    q{},
    '"Beware the Jabberwock, my son!',
    '     The jaws that bite, the claws that catch!',
    'Beware the Jubjub bird, and shun',
    '     The frumious Bandersnatch!"',
    q{},
    'He took his vorpal sword in hand;',
    '     Long time the manxome foe he sought --',
    'So rested he by the Tumtum tree',
    '     And stood awhile in thought.',
    q{},
    'And, as in uffish thought he stood,',
    '     The Jabberwock, with eyes of flame,',
    'Came whiffling through the tulgey wood,',
    '     And burbled as it came!',
    q{},
    'One, two! One, two! And through and through',
    '     The vorpal blade went snicker-snack!',
    'He left it dead, and with its head',
    '     He went galumphing back.',
    q{},
    '"And hast thou slain the Jabberwock?',
    '     Come to my arms, my beamish boy!',
    'O frabjous day! Callooh! Callay!"',
    '     He chortled in his joy.',
    q{},
    '\'Twas brillig, and the slithy toves',
    '     Did gyre and gimble in the wabe:',
    'All mimsy were the borogoves,',
    '     And the mome raths outgrabe.',
    q{/},
    );

#
#  Main program start
#

initialize();

find( { wanted => \&wanted, follow => 1 }, "$testdir/scripts" );

# If we found any .tec files in the scripts directory, then they're helper
# files for the test scripts, so copy them to the cases directory.

while ( defined( my $file = pop @teco_files ) )
{
    copy( $file, "$testdir/cases" )
      or croak "Failed to copy '$file' to '$testdir/cases': $OS_ERROR";
}

make_tests();

if ($execute)
{
    exec_tests();
}

if ($make)
{
    $nscripts += $nskipped;

    printf "Found %u script%s in $testdir/scripts, skipped %u\n", $nscripts,
        $nscripts == 1 ? q{} : 's', $nskipped;

    printf "Created %u file%s in $testdir/cases, with $ntests test%s\n",
        $nfiles, $nfiles == 1 ? q{} : 's', $ntests == 1 ? q{} : 's';
}

exit 0;


# Check to see that the test script specifies a valid TECO.

sub check_teco
{
    my ( $infile, $teco, $redirect ) = @_;

    if (   ( $teco eq 'TECO C'  && $target eq 'TECO-32' )
        || ( $teco eq 'TECO-10' && $target ne 'TECO-64' )
        || ( $teco eq 'TECO-32' && $target eq 'TECO C'  )
        || ( $teco eq 'TECO-64' && $target ne 'TECO-64' ) )
    {
        ++$nskipped;

        print "Skipping $teco file: $infile\n" if $skip;

        return;
    }

    # The following is temporary until we figure out how to redirect
    # input on VMS.

    if ( $target eq 'TECO-32' && $redirect )
    {
        ++$nskipped;

        print "Skipping $teco file: $infile\n" if $skip;

        return;
    }

    if (   $teco ne 'TECO'
        && $teco ne 'TECO C'
        && $teco ne 'TECO-10'
        && $teco ne 'TECO-32'
        && $teco ne 'TECO-64' )
    {
        croak "Unknown requirement $teco in script: $infile";
    }

    return $teco;
}


# Execute tests

sub exec_tests
{
    # Clean up any temp. files we may have created.

    my @files = glob "$testdir/cases/_teco_*";

    foreach my $file (@files)
    {
        unlink $file or croak "Can't delete file: $file";
    }

    # Tell user which benchmark files were superfluous

    if ($orphans && keys %benchmark_files != 0)
    {
        print "Orphan benchmark files:\n";

        foreach my $file (sort keys %benchmark_files)
        {
            print "    $file\n";
        }
    }

    return;
}


# Get data from text file, containing expected or actual test results.

sub get_data
{
    my ($file) = @_;

    if ( !-e $file )
    {
        return;
    }

    my $text = read_file($file);

    if ($text)
    {
        if ($text =~ /!START! . (.+) /msx)
        {
            $text = $1;
        }
    }

    return $text;
}


# Initialize command-line arguments and options.

sub initialize
{
    #
    #  Parse our command-line options
    #

    my $tecoc;
    my $teco32;
    my $teco64;

    GetOptions(
        '32'       => \$teco32,
        '64'       => \$teco64,
        'C'        => \$tecoc,
        'execute!' => \$execute,
        'init!'    => \$init,
        'make!'    => \$make,
        'orphans!' => \$orphans,
        'prove!'   => \$prove,
        'skip!'    => \$skip,
        'tecoc'    => \$tecoc,
        'teco32'   => \$teco32,
        'teco64'   => \$teco64,
        'teco=s'   => \$target,
        'verbose'  => \$verbose,
    );

    $target = 'TECOC'   if $tecoc;
    $target = 'TECO-32' if $teco32;
    $target = 'TECO-64' if $teco64;

    if ($#ARGV + 1 == 0)
    {
        $testdir = q{.};
    }
    elsif ($#ARGV == 0)
    {
        $testdir = $ARGV[0];
        $testdir =~ s{(.+)/}{$1}ms;
    }
    else
    {
        croak 'Too many arguments';
    }

    croak 'Can\'t find scripts directory'    if !-d "$testdir/scripts";
    croak 'Can\'t find benchmarks directory' if !-d "$testdir/benchmarks";

    if (!-e "$testdir/cases")
    {
        mkdir "$testdir/cases" or croak;
    }

    if (!-e "$testdir/results")
    {
        mkdir "$testdir/results" or croak;
    }

    my %targets = (
        '32'      => 'TECO-32',
        '64'      => 'TECO-64',
        'C'       => 'TECO C',
        'TECOC'   => 'TECO C',
        'TECO32'  => 'TECO-32',
        'TECO-32' => 'TECO-32',
        'TECO64'  => 'TECO-64',
        'TECO-64' => 'TECO-64',
        'VMS'     => 'TECO-32',
    );

    my $orig = $target;

    $target = $targets{ uc $orig };

    croak "Invalid TECO version: $orig" unless $target;

    # Get the list of benchmark files

    my @files = glob "$testdir/benchmarks/*.lis";

    foreach my $file (@files)
    {
        $file = basename($file);
        $benchmark_files{$file} = 1;
    }

    # If requested, reinitialize cases/ and results/ directories

    if ($init)
    {
        @files = glob "$testdir/cases/*";

        foreach my $file (@files)
        {
            unlink $file or croak "Can't delete file: $file";
        }

        @files = glob "$testdir/results/*";

        foreach my $file (@files)
        {
            unlink $file or croak "Can't delete file: $file";
        }
    }

    return;
}


# Make test scripts.

sub make_tests
{
    # Sort all of the input files by file name, then create a test script for
    # each one, translating any tokens as needed, then run the script, capture
    # the output, and verify that we got the desired result.

    foreach my $file ( sort { uc $a cmp uc $b } keys %scripts )
    {
        Readonly my $ERROR => -1;

        my $infile = "$scripts{$file}/$file.test";
        my @input  = read_file($infile);

        next if $#input == $ERROR;

        my $outfile = "$file.tec";
        my ( $report, $text, $expects, $redirect ) =
            parse_script( $infile, $outfile, @input );

        next unless $report;

        if ($make)
        {
            # Found a real test script, so count it

            ++$nscripts;

            $text = translate_tokens( $infile, $text, $redirect );

            write_test( "$testdir/cases/$outfile", $text );

            if ($execute)
            {
                my $cwd = getcwd;

                chdir "$testdir/cases" or croak "Can't change directory to $testdir/cases";

                my $actual = setup_test( $outfile, $report, $expects, $redirect );

                chdir $cwd or croak "Can't change directory to $cwd";

                write_result( "$testdir/results/$file.lis", $actual );
            }
            else
            {
                $prove = 0;
            }
        }

        if ($prove)
        {
            prove_test( $file, $report, $expects );
        }
    }

    return;
}

# Read a test script header, and figure out how we need to proceed.

sub parse_script
{
    my ( $infile, $outfile, @lines ) = @_;
    my $header      = 1;
    my $local_tests = 0;
    my $function;
    my $command;
    my $teco;
    my $redirect;
    my $expects;
    my $text;

    foreach my $line (@lines)
    {
        if ( $line =~ /\[\[enter\]\]/ms )
        {
            $header = 0;
        }
        elsif ($header)
        {
            if ( $line =~ /Function:\s(.+)\s!/ms )
            {
                $function = $1;
            }
            elsif ( $line =~ /Command:\s(.+)\s!/ms )
            {
                $command = $1;
            }
            elsif ( $line =~ /!\s+(<)?(TECO.*):\s(.+)\s!/ms )
            {
                if ( !$teco || $target eq 'TECO C' )
                {
                    $redirect = $1;
                    $expects  = $3;

                    $teco = check_teco( $infile, $2, $redirect );
                }
                elsif ( $2 eq 'TECO-32' && $target eq 'TECO-32' )
                {
                    $redirect = $1;
                    $expects  = $3;
                }
                elsif ( $2 eq 'TECO-64' && $target eq 'TECO-64' )
                {
                    $redirect = $1;
                    $expects  = $3;
                }

                return unless $teco;
            }

            next;
        }

        $text .= $line;

        if ( $line =~ / Test: /ms )
        {
            ++$local_tests;
        }
    }

    croak "No function found in script: $infile"     unless $function;
    croak "No command found in script: $infile"      unless $command;
    croak "No TECO version found in script: $infile" unless $teco;
    croak "No expectations found in script: $infile" unless $expects;

    $ntests += $local_tests;

    $outfile =~ s/ (.+) [.]tec /$1/msx;

    my $report = sprintf '%-11s %-45s %-13s', "$outfile", $function, $command;

    if ($redirect)
    {
        $report .= sprintf '  <%3d>', $local_tests;
    }
    else
    {
        $report .= sprintf '  [%3d]', $local_tests;
    }

    $report .= sprintf '  %s', $expects;

    return ( $report, $text, $expects, $redirect );
}


# Verify that we got the test resulted in the expected TECO error.

sub prove_error
{
    my ( $report, $expects, $actual ) = @_;
    my $match = '! Expects: ' . quotemeta $expects . ' !';

    $actual =~ s/$match//ms;

    # Here if test was supposed to abort with TECO error

    if ( index( $actual, $expects ) < 0 )
    {
        if ( $actual =~ /( \N{QUESTION MARK} \w\w\w )/msx )
        {
            # Test encountered unexpected TECO error

            printf "%s %s\n", $report, $1;
        }
        elsif ( $actual =~ /!FAIL\d+!/ms )
        {
            # Test failed when it should have issued TECO error

            printf "%s FAIL\n", $report;
        }
        elsif ( $actual =~ /!PASS!/ms )
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
    else                                # Test encountered expected TECO error
    {
        printf "%s OK\n", $report if $verbose;
    }

    return;
}


# Verify that we got the test resulted in the expected failure.

sub prove_fail
{
    my ( $report, $expected, $actual ) = @_;

    # Here if test was supposed to fail

    if ( $actual !~ /!FAIL\d+!/ms )
    {
        if ( $actual =~ / ( \N{QUESTION MARK} \w\w\w )/msx )
        {
            # Test encountered TECO error

            printf "%s %s\n", $report, $1;
        }
        elsif ( $actual =~ /!PASS!/ms )
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
    else
    {
        printf "%s OK\n", $report if $verbose;
    }

    return;
}


# Verify that we got the test resulted in the expected success.

sub prove_pass
{
    my ( $report, $expected, $actual ) = @_;

    # Here if test was supposed to succeed

    if ( $actual !~ /!PASS!/ms )
    {
        if ( $actual =~ / ( \N{QUESTION MARK} \w\w\w )/msx )
        {
            # Test encountered TECO error

            printf "%s %s\n", $report, $1;
        }
        elsif ( $actual =~ /!FAIL\d+!/ms )
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
    elsif ( defined $expected && $expected ne $actual )
    {
        printf "%s DIFF\n", $report;
    }
    else
    {
        printf "%s OK\n", $report if $verbose;
    }

    return;
}


# Verify the test results.

sub prove_test
{
    my ( $file, $report, $expects ) = @_;

    my $actual = get_data("$testdir/results/$file.lis");

    if (!$actual)
    {
        system 'reset -I';

        printf "%s -> EMPTY\n", $report;

        return;
    }

    my $expected = get_data("$testdir/benchmarks/$file.lis");

    delete $benchmark_files{"$file.lis"}; # Delete this key

    # TECO C sometimes terminates lines with CR/LF, and sometimes with LF/CR.
    # Removing all instances of CR just makes comparisons much easier.

    if ($target eq 'TECO C' || $target eq 'TECO-32')
    {
        $actual   =~ s/\r//gms;
        $expected =~ s/\r//gms if $expected;
    }

    if ($expected)
    {
        $report .= ' =>';
    }
    else
    {
        $report .= ' ->';
    }

    if ($actual =~ /lost block/gms)
    {
        print "$report MEMORY\n";

        return;
    }

    $actual =~ s/add_mblock.+?\n//gms;
    $actual =~ s/exit_mem.+?\n//gms;
    $actual =~ s/expand_mem.+?\n//gms;
    $actual =~ s/shrink_mem.+?\n//gms;

    if ( $actual eq 'TIMEOUT' )
    {
        print "$report TIMEOUT\n";
    }
    elsif ( $actual =~ / core \s dumped /imsx )
    {
        # This is handled specially without regard to the desired result,
        # because it was found that some tests could complete successfully,
        # but abort on exit, due to such things as stack smashing. So we
        # first check to see if there was an abort in the output, and only
        # then verify desired success or failure.

        print "$report ABORTED\n";
    }
    elsif ( $expects eq 'PASS' )
    {
        prove_pass( $report, $expected, $actual );
    }
    elsif ( $expects eq 'FAIL' )
    {
        prove_fail( $report, $expected, $actual );
    }
    else    # Here if test failed or should fail
    {
        prove_error( $report, $expects, $actual );
    }

    return;
}


# Execute a given test set. The reason for the alarm is that some of the tests
# cause TECO C to hang, and we need a way to bail out. The execution of the
# reset command ensures that we restore terminal characteristics to what they
# were originally.

sub run_test
{
    my ($command) = @_;
    my $result = q{};
    my $status = eval {
        local $SIG{ALRM} = sub { croak 'TECO alarm' };

        alarm 1;                        # Should be long enough for any TECO test

        $result = qx($command);

        alarm 0;
    };

    alarm 0;                            # Race condition protection

    if (!defined $status || ($EVAL_ERROR && $EVAL_ERROR =~ 'TECO alarm'))
    {
        $result = 'TIMEOUT';

        system 'reset -I';
    }

    return $result;
}


# Set up to run a test.

sub setup_test
{
    my ( $file, $report, $expects, $redirect ) = @_;
    my $fh;

    unlink 'out1.tmp', 'out2.tmp';

    open $fh, '>', 'in1.tmp' or die "Can't create in1.tmp: $OS_ERROR\n";

    print {$fh} "hello, world!\n";

    close $fh;

    open $fh, '>', 'cmd1.tmp' or die "Can't create cmd1.tmp: $OS_ERROR\n";

    print {$fh} "@^A/hello, world!/ 10^T";

    close $fh;

    my $command;
    my $actual;

    if ( $target eq 'TECO C' )
    {
        $command = 'tecoc';

        if (qx{which $command} eq q{})
        {
            print "Can't find $command executable\n";

            exit 1;
        }

        if ($redirect)
        {
            $command .= ' < ';
        }
        else
        {
            $command .= ' mung ';
        }

        $command .= " $file 2>&1";

        # Set up dummy environment variables that we can use to match our
        # benchmark results, instead of having actual directory and file
        # names that will vary depending on the environment. Note that
        # TECO C treats TEC_INIT not as the name of an indirect command
        # file, but as a command string, so we provide a dummy tag that
        # can be executed without actually doing anything.

        local $ENV{TEC_INIT}    = '!TEC_INIT!';
        local $ENV{TEC_LIBRARY} = 'TEC_LIBRARY';
        local $ENV{TEC_MEMORY}  = 'TEC_MEMORY';
        local $ENV{TEC_VTEDIT}  = 'TEC_VTEDIT';

        $actual = run_test($command);

        if ($actual =~ /core dumped/ims)
        {
            system 'reset -I';
        }
    }
    elsif ( $target eq 'TECO-32' )
    {
        croak "$target target not yet implemented";
    }
    elsif ( $target eq 'TECO-64' )
    {
        $command = 'teco';

        if (qx{which $command} eq q{})
        {
            print "Can't find $command executable\n";

            exit 1;
        }

        $command .= ' -n';

        if ($redirect)
        {
            $command .= ' <';
        }
        else
        {
            $command .= ' -v -i --mung';
        }

        $command .= " $file 2>&1";

        if ( $report =~ /:\@EG/ms )
        {
            # Set up dummy environment variables that we can use to match
            # our benchmark results, instead of having actual directory and
            # file names that will vary depending on the environment. Note
            # that we do this for :@EG commands, because other commands might
            # actually care about the settings of these (esp. TECO_LIBRARY).

            local $ENV{TECO_INIT}    = 'TECO_INIT';
            local $ENV{TECO_LIBRARY} = 'TECO_LIBRARY';
            local $ENV{TECO_MEMORY}  = 'TECO_MEMORY';
            local $ENV{TECO_VTEDIT}  = 'TECO_VTEDIT';

            $actual = run_test($command);
        }
        else
        {
            $actual = run_test($command);
        }
    }
    else
    {
        croak "Unknown target: $target";
    }

    if ( $actual =~ /!START!.(.+)/msx )
    {
        $actual = $1;
    }

    return $actual;
}


# Translate tokens in test script, depending on target TECO.

sub translate_tokens
{
    my ( $file, $text, $redirect ) = @_;
    my $sequence = 0;
    my $teco;

    if ($target eq 'TECO-64')
    {
        $teco = 'teco64';
    }
    elsif ($target eq 'TECO-32')
    {
        $teco = 'teco32';
    }
    elsif ($target eq 'TECO C')
    {
        $teco = 'tecoc';
    }
    else
    {
        croak "Invalid target: $target";
    }

    while ( $text =~ / (.*) \[ \[ (.+) \] \] (.*) /msx )
    {
        my $before = $1;
        my $token  = $2;
        my $after  = $3;
        my $middle;

        if ($token eq 'JABBERWOCKY')
        {
            $middle = join "\n", @jabberwocky;
        }
        elsif (exists $tokens{$teco}{$2})
        {
            $middle = $tokens{$teco}{$2};
        }
        elsif (exists $tokens{common}{$2})
        {
            $middle = $tokens{common}{$2};
        }
        else
        {
            croak "Can't translate token $2 in script: $file";
        }

        if ( $middle =~ /(.+!FAIL)(!.+)/ms )
        {
            $middle = $1 . sprintf '%u', ++$sequence;
            $middle .= $2;
        }

        if ($redirect)
        {
            if ( $token eq 'enter' )
            {
                $middle .= "\e\e\n";
            }
            elsif ( $token eq 'exit' )
            {
                $middle = "\e\e\n$middle";
            }
        }

        $text = $before . $middle . $after;
    }

    $text .= "\e\e";

    return $text;
}


# Helper function for find().

sub wanted
{
    my $infile = $_;

    # Only look for TECO script files

    if ( $infile =~ / (.+) [.] test $ /msx )
    {
        $scripts{$1} = $File::Find::dir;
    }
    elsif ( $infile =~ / [.] tec $ /msx )
    {
        $infile = getcwd . q{/} . $infile;

        push @teco_files, $infile;
    }

    return;
}


# Write test result to new file.

sub write_result
{
    my ( $file, $text ) = @_;

    open my $fh, '>', $file or croak "Can't open output file: $file";

    print {$fh} $text;

    close $fh;

    return;
}


# Write translated test script to new file.

sub write_test
{
    my ( $file, $text ) = @_;

    $file =~ s{[.]test$}{.tec}ms;

    open my $fh, '>', $file or croak "Can't open output file: $file";

    print {$fh} $text;

    close $fh;

    ++$nfiles;

    return;
}
