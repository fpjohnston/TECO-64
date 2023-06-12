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

Readonly my @OPTIONS => (
    {
        name  => q{-A},
        tests => [
            { cmd => '-Ex.tmp -A2,3',         result => ' EIx.tmp' },
            { cmd => '-A2,3 --execute=x.tmp', result => '2,3EIx.tmp' },
            { cmd => '-A',                    result => 'Missing argument' },
            { cmd => '-Ax',                   result => 'Invalid value' },
            { cmd => '-A2,x',                 result => 'Invalid value' },
            { cmd => '-A2,3x',                result => 'Invalid value' },
        ],
    },
    {
        name  => q{--arguments},
        tests => [
            { cmd => '--arguments=2,3 -Ex.tmp', result => '2,3EIx.tmp' },
            {
                cmd    => '--arguments=2,3 --execute=x.tmp',
                result => '2,3EIx.tmp'
            },
            { cmd => '--arguments', result => 'Missing argument' },
        ],
    },
    {
        name  => q{-C},
        tests => [
            { cmd => '-C x.tmp', result => 'Editing file' },
            { cmd => '-C y.tmp', result => 'Creating new file' },
        ],
    },
    {
        name  => q{--create},
        tests => [
            { cmd => '--create x.tmp', result => 'Editing file' },
            { cmd => '--create y.tmp', result => 'Creating new file' },
        ],
    },
    {
        name  => q{-c},
        tests => [ { cmd => '-c x.tmp', result => 'Editing file' }, ],
    },
    {
        name  => q{--nocreate},
        tests => [
            { cmd => '--nocreate x.tmp', result => 'Editing file' },
            { cmd => '--nocreate=x.tmp', result => 'No arguments allowed' },
        ],
    },
    {
        name  => q{-D},
        tests => [ { cmd => '-D',      result => '-1W' }, ],
        tests => [ { cmd => '-Dx.tmp', result => 'EIx.tmp' }, ],
    },
    {
        name  => q{--display},
        tests => [ { cmd => '--display',       result => '-1W' }, ],
        tests => [ { cmd => '--display=x.tmp', result => 'EIx.tmp' }, ],
    },
    {
        name  => q{-d},
        tests => [ { cmd => '-d', result => '!begin! !end!' }, ],
    },
    {
        name  => q{--nodisplay},
        tests => [ { cmd => '--nodisplay -X', result => '!begin! !end!' }, ],
        tests =>
          [ { cmd => '--nodisplay=x.tmp', result => 'No arguments allowed' }, ],
    },
    {
        name  => q{-E},
        tests => [
            { cmd => '-Ex.tmp', result => 'EIx.tmp' },
            { cmd => '-E',      result => 'Missing argument' },
        ],
    },
    {
        name  => q{--execute},
        tests => [
            { cmd => '--execute=x.tmp', result => 'EIx.tmp' },
            { cmd => '--execute',       result => 'Missing argument' },
        ],
    },
    {
        name  => q{-F},
        tests => [ { cmd => '-F', result => '1,0E3' }, ],
    },
    {
        name  => q{--formfeed},
        tests => [ { cmd => '--formfeed', result => '1,0E3' }, ],
    },
    {
        name  => q{-f},
        tests => [ { cmd => '-f', result => '0,1E3' }, ],
    },
    {
        name  => q{--noformfeed},
        tests => [ { cmd => '--noformfeed', result => '0,1E3' }, ],
    },
    {
        name  => q{-H},
        tests => [ { cmd => '-H', result => 'Usage: teco' }, ],
    },
    {
        name  => q{--help},
        tests => [ { cmd => '--help', result => 'Usage: teco' }, ],
    },
    {
        name  => q{-I},
        tests => [
            { cmd => '-Ix.tmp', result => 'EIx.tmp' },
            { cmd => '-I',      result => 'Missing argument' },
        ],
    },
    {
        name  => q{--initialize},
        tests => [
            { cmd => '--initialize=x.tmp', result => 'EIx.tmp' },
            { cmd => '--initialize',       result => 'Missing argument' },
        ],
    },
    {
        name  => q{-i},
        tests => [ { cmd => '-i', result => '!begin! !end!' }, ],
    },
    {
        name  => q{--noinitialize},
        tests => [
            { cmd => '--noinitialize',   result => '!begin! !end!' },
            { cmd => '--noinitialize=1', result => 'No arguments allowed' },
        ],
    },
    {
        name  => q{-L},
        tests => [
            { cmd => '-Lx.tmp', result => 'ELx.tmp' },
            { cmd => '-L',      result => 'Missing argument' },
        ],
    },
    {
        name  => q{--log},
        tests => [
            { cmd => '--log=x.tmp', result => 'ELx.tmp' },
            { cmd => '--log',       result => 'Missing argument' },
        ],
    },
    {
        name  => q{-m},
        tests => [ { cmd => '-m', result => '!begin! !end!' }, ],
    },
    {
        name  => q{--make},
        tests => [
            { cmd => '--make x.tmp',   result => 'EWx.tmp' },
            { cmd => '--make foo baz', result => 'Too many files' },
            { cmd => '--make love',    result => 'Not war?' },
            { cmd => '--make',         result => 'make nothing' },
        ],
    },
    {
        name  => q{--mung},
        tests => [
            { cmd => '--mung x.tmp',   result => 'EIx.tmp' },
            { cmd => '--make foo baz', result => 'Too many files' },
            { cmd => '--mung',         result => 'mung nothing' },
        ],
    },
    {
        name  => q{--nomemory},
        tests => [
            { cmd => '--nomemory',       result => '!begin! !end!' },
            { cmd => '--nomemory=x.tmp', result => 'No arguments allowed' },
        ],
    },
    {
        name  => q{-n},
        tests => [ { cmd => '-n', result => '!begin! !end!' }, ],
    },
    {
        name  => q{--nodefaults},
        tests => [
            { cmd => '--nodefaults',       result => '!begin! !end!' },
            { cmd => '--nodefaults=x.tmp', result => 'No arguments allowed' },
        ],
    },
    {
        name  => q{-R},
        tests => [
            { cmd => '-R x.tmp', result => 'Reading file' },
            { cmd => '-R',       result => 'inspect nothing' },
        ],
    },
    {
        name  => q{--read-only},
        tests => [ { cmd => '--read-only x.tmp', result => 'Reading file' }, ],
    },
    {
        name  => q{-r},
        tests => [ { cmd => '-r x.tmp', result => 'Editing file' }, ],
    },
    {
        name  => q{--noread-only},
        tests =>
          [ { cmd => '--noread-only x.tmp', result => 'Editing file' }, ],
    },
    {
        name  => q{-S},
        tests => [
            { cmd => '-S5',       result => '5,7:W' },
            { cmd => '-S5 -d -D', result => '-1W 5,7:W' },
            { cmd => '-S',        result => 'Missing argument' },
            { cmd => '-Sx',       result => 'Invalid value' },
            { cmd => '-S2x',      result => 'Invalid value' },
        ],
    },
    {
        name  => q{--scroll},
        tests => [
            { cmd => '--scroll=5',    result => '5,7:W' },
            { cmd => '--scroll=5 -d', result => '5,7:W' },
            { cmd => '--scroll',      result => 'Missing argument' },
        ],
    },
    {
        name  => q{-T},
        tests => [
            { cmd => '-Tx.tmp', result => 'Ix.tmp' },
            { cmd => '-T',      result => 'Missing argument' },
        ],
    },
    {
        name  => q{--text},
        tests => [
            { cmd => '--text=x.tmp', result => 'Ix.tmp' },
            { cmd => '--text',       result => 'Missing argument' },
        ],
    },
    {
        name  => q{--version},
        tests => [ { cmd => '--version', result => 'version 200.' } ],
    },
    {
        name  => q{-X},
        tests => [ { cmd => '-X', result => '!begin! EX' }, ],
    },
    {
        name  => q{--exit},
        tests => [ { cmd => '--exit', result => '!begin! EX' }, ],
    },
    {
        name  => q{general},
        tests => [
            {
                cmd    => '--display=z.tmp --execute=y.tmp --init=x.tmp',
                result => 'EIx.tmp^[ EIy.tmp^[ EIz.tmp^[ ^[^['
            },
            { cmd => 'x.tmp y.tmp z.tmp', result => 'Too many files' },
            { cmd => 'x.tmp x.tmp',       result => 'same file' },
            { cmd => 'y.tmp',             result => 'Can\'t find file' },
            { cmd => '-z',                result => 'Unknown option' },
            { cmd => '--foo',             result => 'Unknown option' },
        ],
    },
);

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

    foreach my $test (@tests)
    {
        my $command  = $test->{cmd};
        my $expected = $test->{result};

        my $actual = exec_command( $command, $option );

        prove_test( $expected, $actual );

        ++$ntests;
    }
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

    $command = "teco --practice -n $command";

    print "    Executing \"$command\"..." if $verbose;

    $command = "touch x.tmp; rm -f y.tmp; $command; rm -f x.tmp";

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
