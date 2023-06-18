#!/usr/bin/perl
#
#  options_test.pl - Test command-line options for TECO text editor.
#
#  @copyright 2023 Franklin P. Johnston / Nowwith Treble Software
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
use English qw( -no_match_vars );
use Getopt::Long;
use Readonly;

my $brief   = undef;
my $summary = undef;
my $verbose = undef;
my $detail  = undef;

#<<< perltidy:: preserve format

Readonly my $TECO   => 'teco -p';             # Base command
Readonly my $OWL    => 'TECO_MEMORY=_owl';    # Memory file w/ existing file
Readonly my $KANGA  => 'TECO_MEMORY=_kanga';  # Memory file w/ non-existent file
Readonly my $EEYORE => 'TECO_INIT=_eeyore';   # Dummy initialization file
Readonly my $TIGGER => 'TECO_VTEDIT=_tigger'; # Dummy initialization file

Readonly my @OPTIONS => (
    {
        name  => q{opening files},
        tests => [
            { cmd => "$TECO -C _pooh",            result => 'Editing file' },
            { cmd => "$TECO -C _piglet",          result => 'Creating new file' },
            { cmd => "$TECO -R _pooh",            result => 'Reading file' },
            { cmd => "$TECO -c _pooh",            result => 'Editing file' },
            { cmd => "$TECO -r _pooh",            result => 'Editing file' },
            { cmd => "$TECO --create _pooh",      result => 'Editing file' },
            { cmd => "$TECO --create _piglet",    result => 'Creating new file' },
            { cmd => "$TECO --make _pooh",        result => 'Creating new file' },
            { cmd => "$TECO --make love",         result => 'Not war?' },
            { cmd => "$TECO --read-only _pooh",   result => 'Reading file' },
            { cmd => "$TECO --nocreate _pooh",    result => 'Editing file' },
            { cmd => "$TECO --noread-only _pooh", result => 'Editing file' },
            { cmd => "$TECO _pooh _pooh",         result => 'same file' },
            { cmd => "$OWL $TECO ",               result => 'Editing file' },
            { cmd => "$KANGA $TECO ",             result => 'Ignoring TECO\'s memory' },
        ],
    },
    {
        name  => q{missing files},
        tests => [
            { cmd => "$TECO -R",                 result => 'inspect nothing' },
            { cmd => "$TECO --make",             result => 'make nothing'    },
            { cmd => "$TECO --mung",             result => 'mung nothing'    },
            { cmd => "$TECO --read-only",        result => 'inspect nothing' },
            { cmd => "$TECO _piglet",            result => 'Can\'t find file' },
            { cmd => "$TECO --nocreate _piglet", result => 'Can\'t find file' },
        ],
    },
    {
        name  => q{too many files},
        tests => [
            { cmd => "$TECO _pooh _piglet _owl",   result => 'Too many files' },
            { cmd => "$TECO --make _pooh _piglet", result => 'Too many files' },
            { cmd => "$TECO --make _pooh _piglet", result => 'Too many files' },
        ],
    },
    {
        name  => q{command file options},
        tests => [
            { cmd => "$TECO -E_pooh -A2,3",                   result => ' EI_pooh' },
            { cmd => "$TECO -A2,3 --execute=_pooh",           result => '2,3EI_pooh' },
            { cmd => "$TECO -E_pooh",                         result => 'EI_pooh' },
            { cmd => "$TECO -I_pooh",                         result => 'EI_pooh' },
            { cmd => "$EEYORE $TECO -i",                      result => '!begin! !end!' },
            { cmd => "$TECO --arguments=2,3 -E_pooh",         result => '2,3EI_pooh' },
            { cmd => "$TECO --arguments=2,3 --execute=_pooh", result => '2,3EI_pooh' },
            { cmd => "$TECO --execute=_pooh",                 result => 'EI_pooh' },
            { cmd => "$TECO --initialize=_pooh",              result => 'EI_pooh' },
            { cmd => "$TECO --mung _pooh",                    result => 'EI_pooh' },
            { cmd => "$EEYORE $TECO --noinitialize",          result => '!begin! !end!' },
        ],
    },
    {
        name  => q{display mode options},
        tests => [
            { cmd => "$TECO -D",                 result => '!begin! -1W' },
            { cmd => "$TECO -D_pooh",            result => 'EI_pooh' },
            { cmd => "$TIGGER $TECO",            result => 'EI_tigger' },
            { cmd => "$TIGGER $TECO -d",         result => '!begin! !end!' },
            { cmd => "$TECO -D_piglet",          result => 'EI_piglet' },
            { cmd => "$TIGGER $TECO -S5",        result => 'EI_tigger^[ 5,7:W' },
            { cmd => "$TECO -S5 -d -D",          result => '-1W 5,7:W' },
            { cmd => "$TECO --nodisplay",        result => '!begin! !end!' },
            { cmd => "$TECO --display",          result => '-1W' },
            { cmd => "$TECO --display=_pooh",    result => 'EI_pooh' },
            { cmd => "$TIGGER $TECO --scroll=5", result => 'EI_tigger^[ 5,7:W' },
            { cmd => "$TECO --scroll=5 -d",      result => '!begin! 5,7:W' },
        ],
    },
    {
        name  => q{miscellaneous options},
        tests => [
            { cmd => "$TECO -F",                                result => '1,0E3' },
            { cmd => "$TECO -f",                                result => '0,1E3' },
            { cmd => "$EEYORE $TIGGER $OWL $TECO -m",           result => 'EI_eeyore^[ EI_tigger^[ ^[' },
            { cmd => "$TECO",                                   result => '!begin! !end!' },
            { cmd => "$TECO -H",                                result => 'Usage: teco' },
            { cmd => "$TECO -L_pooh",                           result => 'EL_pooh' },
            { cmd => "$TECO -T_pooh",                           result => 'I_pooh' },
            { cmd => "$TECO -X",                                result => '!begin! EX' },
            { cmd => "$TECO -n --exit",                         result => '!begin! EX' },
            { cmd => "$TECO --formfeed",                        result => '1,0E3' },
            { cmd => "$TECO --help",                            result => 'Usage: teco' },
            { cmd => "$TECO --log=_pooh",                       result => 'EL_pooh' },
            { cmd => "$EEYORE $TIGGER $OWL $TECO --nodefaults", result => '!begin! !end!' },
            { cmd => "$EEYORE $TIGGER $TECO --nodisplay",       result => 'EI_eeyore^[ ^[' },
            { cmd => "$TECO --noformfeed",                      result => '0,1E3' },
            { cmd => "$EEYORE $TIGGER $OWL $TECO --nomemory",   result => 'EI_eeyore^[ EI_tigger^[ ^[' },
            { cmd => "$TECO --text=_pooh",                      result => 'I_pooh' },
            { cmd => "$TECO --version",                         result => 'version 200.' },
        ],
    },
    {
        name  => q{unknown options},
        tests => [
            { cmd => "$TECO -z",    result => 'Unknown option' },
            { cmd => "$TECO --foo", result => 'Unknown option' },
        ],
    },
    {
        name  => q{missing arguments},
        tests => [
            { cmd => "$TECO -A",              result => 'Missing argument' },
            { cmd => "$TECO -A --zero",       result => 'Missing argument' },
            { cmd => "$TECO -E",              result => 'Missing argument' },
            { cmd => "$TECO -E --zero",       result => 'Missing argument' },
            { cmd => "$TECO -I",              result => 'Missing argument' },
            { cmd => "$TECO -I --zero",       result => 'Missing argument' },
            { cmd => "$TECO -L",              result => 'Missing argument' },
            { cmd => "$TECO -L --zero",       result => 'Missing argument' },
            { cmd => "$TECO -S",              result => 'Missing argument' },
            { cmd => "$TECO -S --zero",       result => 'Missing argument' },
            { cmd => "$TECO -T",              result => 'Missing argument' },
            { cmd => "$TECO -T --zero",       result => 'Missing argument' },
            { cmd => "$TECO --arguments",     result => 'Missing argument' },
            { cmd => "$TECO --arguments -Z",  result => 'Missing argument' },
            { cmd => "$TECO --execute",       result => 'Missing argument' },
            { cmd => "$TECO --execute -Z",    result => 'Missing argument' },
            { cmd => "$TECO --initialize",    result => 'Missing argument' },
            { cmd => "$TECO --initialize -Z", result => 'Missing argument' },
            { cmd => "$TECO --log",           result => 'Missing argument' },
            { cmd => "$TECO --log -Z",        result => 'Missing argument' },
            { cmd => "$TECO --scroll",        result => 'Missing argument' },
            { cmd => "$TECO --scroll -Z",     result => 'Missing argument' },
            { cmd => "$TECO --text",          result => 'Missing argument' },
            { cmd => "$TECO --text -Z",       result => 'Missing argument' },
        ],
    },
    {
        name  => q{extraneous arguments},
        tests => [
            { cmd => "$TECO --create=_pooh",       result => 'Extraneous argument' },
            { cmd => "$TECO --exit=_pooh",         result => 'Extraneous argument' },
            { cmd => "$TECO --formfeed=_pooh",     result => 'Extraneous argument' },
            { cmd => "$TECO --help=_pooh",         result => 'Extraneous argument' },
            { cmd => "$TECO --nocreate=_pooh",     result => 'Extraneous argument' },
            { cmd => "$TECO --nodefaults=_pooh",   result => 'Extraneous argument' },
            { cmd => "$TECO --nodisplay=_pooh",    result => 'Extraneous argument' },
            { cmd => "$TECO --noformfeed=_pooh",   result => 'Extraneous argument' },
            { cmd => "$TECO --noinitialize=_pooh", result => 'Extraneous argument' },
            { cmd => "$TECO --nomemory=_pooh",     result => 'Extraneous argument' },
            { cmd => "$TECO --noread-only=_pooh",  result => 'Extraneous argument' },
            { cmd => "$TECO --read-only=_pooh",    result => 'Extraneous argument' },
        ],
    },
    {
        name  => q{invalid arguments},
        tests => [
            { cmd => "$TECO -Ax",              result => 'Invalid value' },
            { cmd => "$TECO -A2,x",            result => 'Invalid value' },
            { cmd => "$TECO -A2,3x",           result => 'Invalid value' },
            { cmd => "$TECO -Sx",              result => 'Invalid value' },
            { cmd => "$TECO -S2x",             result => 'Invalid value' },
            { cmd => "$TECO --arguments=x",    result => 'Invalid value' },
            { cmd => "$TECO --arguments=2,x",  result => 'Invalid value' },
            { cmd => "$TECO --arguments=2,3x", result => 'Invalid value' },
            { cmd => "$TECO --scroll=x",       result => 'Invalid value' },
            { cmd => "$TECO --scroll=2x",      result => 'Invalid value' },
            { cmd => "$TECO --scroll=2,3x",    result => 'Invalid value' },
        ],
    },
);

#>>>

# Main program start

initialize();

my $okay   = 0;
my $failed = 0;
my $ntests = 0;

for my $href (@OPTIONS)
{
    my $option = $href->{name};
    my @tests  = @{ $href->{tests} };

    if ($brief)
    {
        my $count = scalar @tests;

        printf "Executing $count test%s for $option...\n",
          $count == 1 ? q{} : 's';
    }

    system 'touch _pooh';               # Create dummy file to edit
    system 'echo _pooh > _owl';         # Create memory file w/ real file
    system 'echo _roo > _kanga';        # Create memory file w/ non-existent file
    system 'rm -f _piglet';             # Ensure this file does not exist

    foreach my $test (@tests)
    {
        my $command  = $test->{cmd};
        my $expected = $test->{result};

        my $actual = exec_command( $command, $option );

        prove_test( $expected, $actual );

        ++$ntests;
    }

    system 'rm -f _pooh _owl _kanga';   # Discard temporary files
}

if ($summary)
{
    printf "%d test%s executed with %d failure%s\n",
      $ntests, $ntests == 1 ? q{} : 's',
      $failed, $failed == 1 ? q{} : 's';
}

exit( $okay == $ntests ? 0 : 1 );       # Return success (0) or failure (1)

# Execute command and return result

sub exec_command
{
    my ($command) = @_;
    my $error = 0;

    print "    Executing \"$command\"..." if $verbose;

    $command = "TECO_INIT= TECO_VTEDIT= TECO_MEMORY= $command";

    my $result = run_test($command);

    $result =~ s/add_mblock.+?\n//gms;
    $result =~ s/delete_mblock.+?\n//gms;
    $result =~ s/exit_mem.+?\n//gms;
    $result =~ s/expand_mem.+?\n//gms;
    $result =~ s/shrink_mem.+?\n//gms;

    $result = ( split /\n/ms, $result )[0];

    chomp $result;

    return $result;
}

# Initialize command-line arguments and options.

sub initialize
{
    #
    #  Parse our command-line options
    #

    GetOptions(
        'brief'   => \$brief,
        'detail'  => \$detail,
        'summary' => \$summary,
        'verbose' => \$verbose,
    );

    $brief = 1 if !$summary && !$brief && !$verbose && !$detail;

    $verbose = 1 if $detail;
    $brief   = 1 if $verbose;
    $summary = 1 if $brief;

    return;
}

# Check to see if test succeeded or failed

sub prove_test
{
    my ( $expected, $actual ) = @_;

    # Here to see whether the command returned the expected results

    if ( index( $actual, $expected ) + 1 != 0 )
    {
        ++$okay;

        print 'OK' if $verbose;
    }
    else
    {
        ++$failed;

        print 'FAILED' if $verbose;
    }

    if ($detail)
    {
        $actual =~ s/ Commands: \N{SPACE} //imsx;    # Trim off any header

        print ", result: $actual";
    }

    print "\n" if $verbose || $detail;

    return;
}

# Execute the TECO command and return any generated output

sub run_test
{
    my ($command) = @_;
    my $result    = q{};
    my $status    = eval {
        local $SIG{ALRM} = sub { croak 'TECO alarm' };

        alarm 1;                        # Should be long enough for any TECO test

        $result = qx($command);

        alarm 0;
    };

    alarm 0;                            # Race condition protection

    if ( !defined $status || ( $EVAL_ERROR && $EVAL_ERROR =~ 'TECO alarm' ) )
    {
        $result = 'TIMEOUT';

        system 'reset -I';
    }

    return $result;
}
