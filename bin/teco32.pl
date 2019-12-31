#!/usr/bin/perl

#
################################################################################

use warnings;
use strict;
use version; our $VERSION = qv('1.0.0');

use Carp;
use English qw { -no_match_vars };
use Readonly;

my $macro;

# Main program starts here

while (<>)
{
    chomp;

    next unless /^.ASCII\s+(.+)$/;
    
    my $text = $1;

    $text =~ s/\r//g;

    if ($text =~ /\\(.+?)\\(.*)$/)
    {
        $text = $1 . $2;
    }

    $text =~ s/\?//g;

    $text =~ s/<13>/^M/g;
    $text =~ s/<10>/^J/g;
    $text =~ s/<(\d+)>/chr($1)/eg;

    $macro .= $text;
}

$macro .= chr(27) . chr(27);

print $macro;
