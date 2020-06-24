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

# Main program starts here

exit if $#ARGV < 0;

my $header = shift @ARGV;
my %codes;

read_codes();

while (<>)
{
    check_line($_);
}

print "Error codes used:\n\n";

foreach my $code (sort keys %codes)
{
    print "$code: $codes{$code}\n" if $codes{$code};
}

print "\nError codes not used:\n\n";

foreach my $code (sort keys %codes)
{
    print "$code: $codes{$code}\n" unless $codes{$code};
}

exit;

sub check_line
{
    my ($line) = @_;

    foreach my $code (keys %codes)
    {
        ++$codes{$code} if $line =~ /_err\($code/;
    }
}

sub read_codes
{
    open (my $fh, $header) or die $!;

    while (<$fh>)
    {
        chomp;

        next unless /(E_...)/;
        next if $1 eq 'E_NUL';

        $codes{$1} = 0;
    }

    close($fh);
}
