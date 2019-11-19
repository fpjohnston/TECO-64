#!/usr/bin/perl

while (<>)
{
    chomp;

    my $line = $_;

    $line =~ s/},//;

    printf "%-52s},\n", $line;
}

