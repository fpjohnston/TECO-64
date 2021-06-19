#!/usr/bin/perl

#
################################################################################

use warnings;
use strict;
use version; our $VERSION = qv('1.0.0');

use Carp;
use English qw { -no_match_vars };
use Readonly;
use Fatal qw { print };

my %tests = (
    '001_001' => {
        detail   => 'CTRL/A w/ colon',
        command  => "\":@^A/hello, world!/\"",
        expected => "hello, world!\r\n",
    },

    '001_002' => {
        detail   => 'CTRL/A w/o colon',
        command  => "\"@^A/hello, world!/\"",
        expected => 'hello, world!',
    },

    '002_001' => {
        detail   => 'Addition',
        command  => '"5+3=',
        expected => "8\r\n",
    },

    '002_002' => {
        detail   => 'Subtraction',
        command  => '5-12=',
        expected => "-7\r\n",
    },

    '002_003' => {
        detail   => 'Multiplication',
        command  => '5*3=',
        expected => "15\r\n",
    },

    '002_004' => {
        detail   => 'Division',
        command  => '5/3=',
        expected => "1\r\n",
    },
);

foreach my $id ( sort keys %tests )
{
    my $detail   = $tests{$id}{detail};
    my $command  = $tests{$id}{command};
    my $expected = $tests{$id}{expected};

    print 'Test ', $id, ": $detail: ";

    my $teco   = "teco --exec='$command' -X";
    my $output = `$teco`;

    $output =~ s/\r\r/\r/msx;

    if ( $output eq $expected )
    {
        print "success\n";
    }
    else
    {
        print "FAILURE\n";
    }
}

exit;
