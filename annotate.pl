#!/usr/bin/perl

#
################################################################################

use warnings;
use strict;
use version; our $VERSION = qv('1.0.0');

use Carp;
use English qw { -no_match_vars };
use Readonly;

# Main program starts here

my %commands =
(
    "^A"  => "type out text",
    "^C"  => "stop execution",
    "^D"  => "set input radix to decimal",
    "^R"  => "set input radix",
    "^T"  => "type out character",
    "^U"  => "copy text to Q-register %s",
    ":^U" => "append text to Q-register %s",
    "^X"  => "set search mode flag",
    "!"   => "tag/comment",
    "\"A" => "if alpha",
    "\"D" => "if digit",
    "\"E" => "if == 0",
    "\"F" => "if false",
    "\"G" => "if > 0",
    "\"L" => "if < 0",
    "\"N" => "if <> 0",
    "\"S" => "if success",
    "\"T" => "if true",
    "\"U" => "if failure",
    "'"   => "endif",
    ";"   => "branch out of loop",
    "<"   => "start of loop",
    ">"   => "end of loop",
    "D"   => "delete characters",
    "EA"  => "switch to secondary output stream",
    "EB"  => "open file for backup",
    "EC"  => "close input and output files",
    "ED"  => "set edit level flag",
    "EF"  => "close output file",
    "EG"  => "operating system function",
    "EH"  => "set help level flag",
    "EI"  => "open indirect command file",
    "EN"  => "set wildcard filespec",
    "EN`" => "get next matching file",
    "EP"  => "switch to secondary input stream", 
    "ER"  => "open file for input",
    "ER`" => "switch to primary input stream",
    "ES"  => "set search verification flag",
    "ET"  => "set type out control flag",
    "EU"  => "set upper/lower case flag",
    "EV"  => "set edit verify flag",
    "EW"  => "open file for output",
    "EW`" => "switch to primary output stream",
    "EX"  => "exit",
    "EZ"  => "set TECO C flag",
    "FN"  => "non-stop search and replace",
    "FR"  => "delete and replace",
    "FS"  => "search and replace",
    "F'"  => "go to end of conditional",
    "F<"  => "go to start of loop",
    "F>"  => "go to end of loop",
    "F_"  => "non-stop search and replace",
    "F|"  => "go to else clause",
    "G"   => "copy Q-register %s to buffer",
    "G*"  => "copy command string to buffer",
    "G_"  => "copy search string to buffer",
    ":G"  => "print Q-register %s",
    ":G*" => "print command string",
    ":G_" => "print search string",
    "I"   => "insert",
    "J"   => "jump",
    "K"   => "delete lines",
    "L"   => "move forward/backward lines",
    "M"   => "execute Q-register %s", 
    "O"   => "goto %s", 
    "P"   => "page",
    "C"   => "move forward",
    "R"   => "move backward",
    "S"   => "search",
    "T"   => "print text",
    "U"   => "store number in Q-register %s",
    "X"   => "copy buffer to Q-register %s",
    ":X"  => "append buffer to Q-register %s",
    "Y"   => "yank",
    "]"   => "pop Q-register %s",
    "\\"  => "insert digit string",
    "["   => "push Q-register %s",
    "|"   => "else",
);

my $sequence = 0;

while (<>)
{
    chomp;

    s/\r//;

    next unless /^(.+?)\s+! (\S+), (\S*) !$/;

    my $teco = $1;
    my $key = uc $2;
    my $arg = $3;
    my $text = $commands{$key};

    if (!defined $text)
    {
        if ($key =~ /^(.+)`$/)
        {
            $text = $commands{$1};
        }
    }

    if (!defined $text)
    {
        if ($key =~ /^:(.+)$/)
        {
            $text = $commands{$1};
        }
    }    

    # If goto command, get the tag

    if ($key eq 'O')
    {
        if ($teco =~ /^\s*$key(.+)`$/)
        {
            $arg = $1;
        }
    }

    my $line = sprintf "%s", $teco;

    print $line;
    printf "%*s! % 5u: ", 64 - length $line, " ", ++$sequence;

    if (!defined $text)
    {
        print "(n/a)";
    }
    elsif ($text =~ /%s/)
    {
        printf $text, $arg;
    }
    else
    {
        print $text;
    }

    print " !\n";
}
