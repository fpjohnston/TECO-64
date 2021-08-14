#!/usr/bin/perl

#
#
################################################################################

use warnings;
use strict;
use version; our $VERSION = qv('1.0.0');

use Carp;
use English qw { -no_match_vars };
use Readonly;
use Fatal qw { print };

# Main program starts here

print_lines('A');
print_lines('B');
print "\f";
print_lines('C');
print_lines('D');
print "\f";
print_lines('E');
print_lines('F');
print "\f";
print_lines('G');
print_lines('H');
print "\f";
print_lines('I');
print_lines('J');
print "\f";
print_lines('K');
print_lines('L');
print "\f";
print_lines('M');
print_lines('N');
print "\f";
print_lines('O');
print_lines('P');

exit;

sub print_lines
{
    my ($chr) = @_;

    const my MAX = 20;
    const my $REPEAT = 25;

    for my $i ( 1 .. $MAX )
    {
        printf "line %02d: %s\n", $i, $chr x $REPEAT;
    }

    return;
}
