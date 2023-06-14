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

Readonly my @OPTIONS => (
    {
        name  => q{-A},
        tests => [
            { cmd => 'teco -pn -Ex.tmp -A2,3',         result => ' EIx.tmp' },
            { cmd => 'teco -pn -A2,3 --execute=x.tmp', result => '2,3EIx.tmp' },
            { cmd => 'teco -pn -A',                    result => 'Missing argument' },
            { cmd => 'teco -pn -Ax',                   result => 'Invalid value' },
            { cmd => 'teco -pn -A2,x',                 result => 'Invalid value' },
            { cmd => 'teco -pn -A2,3x',                result => 'Invalid value' },
        ],
    },
    {
        name  => q{--arguments},
        tests => [
            { cmd => 'teco -pn --arguments=2,3 -Ex.tmp', result => '2,3EIx.tmp' },
            {
                cmd    => 'teco -pn --arguments=2,3 --execute=x.tmp',
                result => '2,3EIx.tmp'
            },
            { cmd => 'teco -pn --arguments', result => 'Missing argument' },
        ],
    },
    {
        name  => q{-C},
        tests => [
            { cmd => 'teco -pn -C x.tmp', result => 'Editing file' },
            { cmd => 'teco -pn -C y.tmp', result => 'Creating new file' },
        ],
    },
    {
        name  => q{--create},
        tests => [
            { cmd => 'teco -pn --create x.tmp', result => 'Editing file' },
            { cmd => 'teco -pn --create y.tmp', result => 'Creating new file' },
        ],
    },
    {
        name  => q{-c},
        tests => [ { cmd => 'teco -pn -c x.tmp', result => 'Editing file' }, ],
    },
    {
        name  => q{--nocreate},
        tests => [
            { cmd => 'teco -pn --nocreate x.tmp', result => 'Editing file' },
            { cmd => 'teco -pn --nocreate=x.tmp', result => 'No arguments allowed' },
        ],
    },
    {
        name  => q{-D},
        tests => [
            { cmd => 'teco -pn -D',      result => '-1W' },
            { cmd => 'teco -pn -Dx.tmp', result => 'EIx.tmp' },
            {
                cmd    => 'TECO_VTEDIT=x.tmp teco -pim',
                result => 'EIx.tmp'
            },
            {
                cmd    => 'TECO_VTEDIT=x.tmp teco -pimd',
                result => '!begin! !end!'
            },
            {
                cmd    => 'TECO_VTEDIT=x.tmp teco -pim -Dy.tmp',
                result => 'EIy.tmp'
            },
        ],
    },
    {
        name  => q{--display},
        tests => [
            { cmd => 'teco -pn --display',       result => '-1W' },
            { cmd => 'teco -pn --display=x.tmp', result => 'EIx.tmp' },
        ],
    },
    {
        name  => q{-d},
        tests => [ { cmd => 'teco -pn -d', result => '!begin! !end!' }, ],
    },
    {
        name  => q{--nodisplay},
        tests => [
            { cmd => 'teco -pn --nodisplay',       result => '!begin! !end!' },
            { cmd => 'teco -pn --nodisplay=x.tmp', result => 'No arguments allowed' },
        ],
    },
    {
        name  => q{-E},
        tests => [
            { cmd => 'teco -pn -Ex.tmp', result => 'EIx.tmp' },
            { cmd => 'teco -pn -E',      result => 'Missing argument' },
        ],
    },
    {
        name  => q{--execute},
        tests => [
            { cmd => 'teco -pn --execute=x.tmp', result => 'EIx.tmp' },
            { cmd => 'teco -pn --execute',       result => 'Missing argument' },
        ],
    },
    {
        name  => q{-F},
        tests => [ { cmd => 'teco -pn -F', result => '1,0E3' }, ],
    },
    {
        name  => q{--formfeed},
        tests => [ { cmd => 'teco -pn --formfeed', result => '1,0E3' }, ],
    },
    {
        name  => q{-f},
        tests => [ { cmd => 'teco -pn -f', result => '0,1E3' }, ],
    },
    {
        name  => q{--noformfeed},
        tests => [ { cmd => 'teco -pn --noformfeed', result => '0,1E3' }, ],
    },
    {
        name  => q{-H},
        tests => [ { cmd => 'teco -pn -H', result => 'Usage: teco' }, ],
    },
    {
        name  => q{--help},
        tests => [ { cmd => 'teco -pn --help', result => 'Usage: teco' }, ],
    },
    {
        name  => q{-I},
        tests => [
            { cmd => 'teco -pn -Ix.tmp', result => 'EIx.tmp' },
            { cmd => 'teco -pn -I',      result => 'Missing argument' },
        ],
    },
    {
        name  => q{--initialize},
        tests => [
            { cmd => 'teco -pn --initialize=x.tmp', result => 'EIx.tmp' },
            { cmd => 'teco -pn --initialize',       result => 'Missing argument' },
        ],
    },
    {
        name  => q{-i},
        tests => [ { cmd => 'teco -pn -i', result => '!begin! !end!' }, ],
    },
    {
        name  => q{--noinitialize},
        tests => [
            { cmd => 'teco -pn --noinitialize',   result => '!begin! !end!' },
            { cmd => 'teco -pn --noinitialize=1', result => 'No arguments allowed' },
        ],
    },
    {
        name  => q{-L},
        tests => [
            { cmd => 'teco -pn -Lx.tmp', result => 'ELx.tmp' },
            { cmd => 'teco -pn -L',      result => 'Missing argument' },
        ],
    },
    {
        name  => q{--log},
        tests => [
            { cmd => 'teco -pn --log=x.tmp', result => 'ELx.tmp' },
            { cmd => 'teco -pn --log',       result => 'Missing argument' },
        ],
    },
    {
        name  => q{-m},
        tests => [ { cmd => 'teco -pn -m', result => '!begin! !end!' }, ],
    },
    {
        name  => q{--make},
        tests => [
            { cmd => 'teco -pn --make x.tmp',   result => 'EWx.tmp' },
            { cmd => 'teco -pn --make foo baz', result => 'Too many files' },
            { cmd => 'teco -pn --make love',    result => 'Not war?' },
            { cmd => 'teco -pn --make',         result => 'make nothing' },
        ],
    },
    {
        name  => q{--mung},
        tests => [
            { cmd => 'teco -pn --mung x.tmp',   result => 'EIx.tmp' },
            { cmd => 'teco -pn --make foo baz', result => 'Too many files' },
            { cmd => 'teco -pn --mung',         result => 'mung nothing' },
        ],
    },
    {
        name  => q{--nomemory},
        tests => [
            { cmd => 'teco -pn --nomemory',       result => '!begin! !end!' },
            { cmd => 'teco -pn --nomemory=x.tmp', result => 'No arguments allowed' },
        ],
    },
    {
        name  => q{-n},
        tests => [ { cmd => 'teco -pn -n', result => '!begin! !end!' }, ],
    },
    {
        name  => q{--nodefaults},
        tests => [
            { cmd => 'teco -pn --nodefaults',       result => '!begin! !end!' },
            { cmd => 'teco -pn --nodefaults=x.tmp', result => 'No arguments allowed' },
        ],
    },
    {
        name  => q{-R},
        tests => [
            { cmd => 'teco -pn -R x.tmp', result => 'Reading file' },
            { cmd => 'teco -pn -R',       result => 'inspect nothing' },
        ],
    },
    {
        name  => q{--read-only},
        tests => [ { cmd => 'teco -pn --read-only x.tmp', result => 'Reading file' }, ],
    },
    {
        name  => q{-r},
        tests => [ { cmd => 'teco -pn -r x.tmp', result => 'Editing file' }, ],
    },
    {
        name  => q{--noread-only},
        tests =>
          [ { cmd => 'teco -pn --noread-only x.tmp', result => 'Editing file' }, ],
    },
    {
        name  => q{-S},
        tests => [
            { cmd => 'teco -pn -S5',       result => '5,7:W' },
            { cmd => 'teco -pn -S5 -d -D', result => '-1W 5,7:W' },
            { cmd => 'teco -pn -S',        result => 'Missing argument' },
            { cmd => 'teco -pn -Sx',       result => 'Invalid value' },
            { cmd => 'teco -pn -S2x',      result => 'Invalid value' },
        ],
    },
    {
        name  => q{--scroll},
        tests => [
            { cmd => 'teco -pn --scroll=5',    result => '5,7:W' },
            { cmd => 'teco -pn --scroll=5 -d', result => '5,7:W' },
            { cmd => 'teco -pn --scroll',      result => 'Missing argument' },
        ],
    },
    {
        name  => q{-T},
        tests => [
            { cmd => 'teco -pn -Tx.tmp', result => 'Ix.tmp' },
            { cmd => 'teco -pn -T',      result => 'Missing argument' },
        ],
    },
    {
        name  => q{--text},
        tests => [
            { cmd => 'teco -pn --text=x.tmp', result => 'Ix.tmp' },
            { cmd => 'teco -pn --text',       result => 'Missing argument' },
        ],
    },
    {
        name  => q{--version},
        tests => [ { cmd => 'teco -pn --version', result => 'version 200.' } ],
    },
    {
        name  => q{-X},
        tests => [ { cmd => 'teco -pn -X', result => '!begin! EX' }, ],
    },
    {
        name  => q{--exit},
        tests => [ { cmd => 'teco -pn --exit', result => '!begin! EX' }, ],
    },
    {
        name  => q{general},
        tests => [
            {
                cmd    => 'teco -pn --display=z.tmp --execute=y.tmp --init=x.tmp',
                result => 'EIx.tmp^[ EIy.tmp^[ EIz.tmp^[ ^[^['
            },
            { cmd => 'teco -pn x.tmp y.tmp z.tmp', result => 'Too many files' },
            { cmd => 'teco -pn x.tmp x.tmp',       result => 'same file' },
            { cmd => 'teco -pn y.tmp',             result => 'Can\'t find file' },
            { cmd => 'teco -pn -z',                result => 'Unknown option' },
            { cmd => 'teco -pn --foo',             result => 'Unknown option' },
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
    my $option  = $href->{name};
    my $general = ( $option eq 'general' );
    my @tests   = @{ $href->{tests} };

    if ($brief)
    {
        printf "Testing $option option%s...\n", $general ? 's' : q{};
    }

    system 'touch x.tmp';               # Make sure this file exists
    system 'rm -f y.tmp';               # Make sure this file does not exist

    foreach my $test (@tests)
    {
        my $command  = $test->{cmd};
        my $expected = $test->{result};

        my $actual = exec_command( $command, $option );

        prove_test( $expected, $actual );

        ++$ntests;
    }

    system 'rm -f x.tmp';               # Discard temporary file
}

if ($summary)
{
    printf "Executed %d test%s\n", $ntests, $ntests == 1 ? q{} : 's';
    printf "$okay completed OK, $failed failed\n";
}

exit( $okay == $ntests ? 0 : 1 );       # Return success (0) or failure (1)

# Execute command and return result

sub exec_command
{
    my ($command) = @_;
    my $error = 0;

    print "    Executing \"$command\"..." if $verbose;

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

        print "OK\n" if $verbose;
    }
    else
    {
        ++$failed;

        print "FAILED\n" if $verbose;
    }

    if ($detail)
    {
        $actual =~ s/ Commands: \N{SPACE} //imsx;    # Trim off any header

        print "    Result: $actual\n\n";
    }

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
