#!/usr/bin/perl

#
################################################################################

use warnings;
use strict;
use version; our $VERSION = qv('1.0.0');

use Carp;
use English qw { -no_match_vars };
use Readonly;

my %tests =
(
    "001_001" =>
    {
        abstract => "CTRL/A w/ colon",
        command  => "\":@^A/hello, world!/\"",
        expected => "hello, world!\r\n",
    },

    "001_002" =>
    {
        abstract => "CTRL/A w/o colon",
        command  => "\"@^A/hello, world!/\"",
        expected => "hello, world!",
    },

    "002_001" =>
    {
        abstract => "Addition",
        command  => "\"5+3=\"",
        expected => "8\r\n",
    },

    "002_002" =>
    {
        abstract => "Subtraction",
        command  => "\"5-12=\"",
        expected => "-7\r\n",
    },

    "002_003" =>
    {
        abstract => "Multiplication",
        command  => "\"5*3=\"",
        expected => "15\r\n",
    },

    "002_004" =>
    {
        abstract => "Division",
        command  => "\"5/3=\"",
        expected => "1\r\n",
    },
);

foreach my $id (sort keys %tests)
{
    my $abstract = $tests{$id}{abstract};
    my $command  = $tests{$id}{command};
    my $expected = $tests{$id}{expected};

    print "Test ", $id, ": $abstract: ";

    my $teco     = "teco --exec='$command' -X";
    my $output   = `$teco`;

    $output =~ s/\r\r/\r/;

    if ($output eq $expected)
    {
        print "success\n";
    }
    else
    {
        print "FAILURE\n";
    }
}

exit;
