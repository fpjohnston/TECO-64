#!/usr/bin/perl
#
#  test_errors.pl - Check for unused error codes.
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

use warnings;
use strict;
use version; our $VERSION = qv('1.0.0');

use Carp;
use English qw { -no_match_vars };
use Getopt::Long;
use Readonly;

my $brief   = undef;
my $summary = undef;

# Initialize command-line arguments and options.

GetOptions(
    'brief'   => \$brief,
    'summary' => \$summary,
);

$summary = 1 if !$summary && !$brief;
$brief   = 1 if $summary;

exit if $#ARGV < 0;

my $header = shift @ARGV;
my %codes;

#  Find all defined error codes in include/errors.h

open my $fh, '<', $header or croak "Can't open $header: $OS_ERROR\n";

my @lines = <$fh>;

close $fh or croak "Can't close $header: $OS_ERROR\n";

foreach my $line (@lines)
{
    if ( $line =~ /(E_[[:upper:]]{3,4})/ms )
    {
        $codes{$1} = 0 unless $1 eq 'E_NUL';
    }
}

#  Find all instances of throw() in source files

my $src    = join q{ }, @ARGV;          # Get all of the source files to scan
my @errors = qx{grep 'throw[(]' $src};  # Look for all instances of 'throw('

#  Look for patterns in the form throw(E_xxx), throw(E_xxxx), and
#  throw(E_xxx, yyy), and count the instances found.

foreach my $error (@errors)
{
    if ( $error =~ / throw [(] (E_[[:upper:]]{3,4}) (,.+)? [)] /msx )
    {
        ++$codes{$1};
    }
}

#  Get the names in lexicographic order

my @names;
my @orphans;

foreach my $code ( sort keys %codes )
{
    push @names, $code;

    if ( $codes{$code} == 0 )
    {
        push @orphans, $code;
    }
}

#  Now print the results in multiple columns

Readonly my $NCOLUMNS => 8;

my $nrows = int( ( scalar @names + $NCOLUMNS - 1 ) / $NCOLUMNS );

if ($summary)
{
    print "Error code summary:\n\n";

    for my $row ( 0 .. $nrows - 1 )
    {
        for my $column ( 0 .. $NCOLUMNS - 1 )
        {
            my $i = $row + ( $column * $nrows );

            if ( defined $names[$i] )
            {
                printf '%6s: %2u  ', $names[$i], $codes{ $names[$i] };
            }
        }

        printf "\n";
    }

    printf "\n";
}

#  See if we had any orphan error codes

if ( scalar @orphans > 0 )
{
    my $orphans = join q{ }, @orphans;

    print "Orphan codes: $orphans\n";

    exit 1;
}
else
{
    print "No orphan errors found\n";

    exit 0;
}
