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

my %details =
(
    "^A"  => "type out text",
    "^C"  => "stop execution",
    "^D"  => "set input radix to decimal",
    "^R"  => "set input radix",
    "^T"  => "type character",
    "^U"  => "copy text to Q-register %c",
    ":^U" => "append text to Q-register %c",
    "^X"  => "set search mode flag",
    "!"   => "tag/comment",
    "%"   => "increment Q-register %c and return value",
    "\"A" => "if alpha",
    "\"C" => "if symbol",
    "\"D" => "if digit",
    "\"E" => "if == 0",
    "\"F" => "if false",
    "\"G" => "if > 0",
    "\"L" => "if < 0",
    "\"N" => "if <> 0",
    "\"R" => "if alphanumeric",
    "\"S" => "if success",
    "\"T" => "if true",
    "\"U" => "if failure",
    "\"V" => "if lower case",
    "\"W" => "if upper case",
    "\"<" => "if < 0",
    "\"=" => "if == 0",
    "\">" => "if > 0",
    "'"   => "endif",
    ";"   => "branch out of loop",
    "<"   => "start of loop",
    "="   => "print value",
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
    "G"   => "copy Q-register %c to buffer",
    "G*"  => "copy command string to buffer",
    "G_"  => "copy search string to buffer",
    ":G"  => "print Q-register %c",
    ":G*" => "print command string",
    ":G_" => "print search string",
    "I"   => "insert",
    "J"   => "jump",
    "K"   => "delete lines",
    "L"   => "move forward/backward lines",
    "M"   => "execute Q-register %c", 
    "O"   => "goto %s", 
    "P"   => "page",
    "C"   => "move forward",
    "R"   => "move backward",
    "S"   => "search",
    "T"   => "print text",
    "U"   => "store number in Q-register %c",
    "X"   => "copy buffer to Q-register %c",
    ":X"  => "append buffer to Q-register %c",
    "Y"   => "yank",
    "]"   => "pop Q-register %c",
    "\\"  => "insert digit string",
    "["   => "push Q-register %c",
    "|"   => "else",
);

my $sequence = 0;
my $indent = 0;

while (<>)
{
    chomp;

    s/\r//;

    my $command = $_;
    my $expression = "";
    my $key;
    my $colon;
    my $atsign;
    my $qreg;
    my $text;

    if (/^\((.+)\) (.+)$/)
    {
        my $tail;

        $tail = $1;
        $key = $command = $2;

        while ($tail =~ /^(.+?)^^ (.+)$/)
        {
            $expression .= $1;
            $expression =~ s/\s+//g;
            $expression .= "^^ ";
            $tail = $2;
        }

        $tail =~ s/\s+//g;
        $expression .= $tail;
    }
    else
    {
        $key = $command;
    }

    $key =~ s/\s+//g;
    $key = $1 if ($key =~ /^::(.+)$/);

    if ($key =~ /^:(.+)$/)
    {
        $colon = 1;
        $key = $1;
    }

    if ($key =~ /^@(.+)$/)
    {
        $atsign = 1;
        $key = $1;
    }

    if ($key =~ /^([][%GMQUX])(.)/i || $key =~ /^(\^U)(.)/i)
    {
        $key = $1;
        $qreg = $2;
    }
    elsif ($key =~ /^([EF].)(.*)/i)
    {
        $key = $1;
        $text = $2;
    }
    elsif ($key =~ /^([INS_]).+/i)
    {
        $key = $1;
    }
    elsif ($atsign && $key =~ /^([!O]).(.+)./i)
    {
        $key = $1;
        $text = $2;
    }
    elsif ($key =~ /^(O)(.+)`/i)
    {
        $key = $1;
        $text = $2;
    }
    elsif ($key =~ /^(!)(.+)!/)
    {
        $key = $1;
        $text = $2;
    }
    elsif ($key =~ /^(".)/)
    {
        $key = $1;
    }
    elsif ($key =~ /(\^.)/)
    {
        $key = $1;
    }

    my $detail;

    $key = uc $key;

    if ($text && $text =~ /^`$/)
    {
        $detail = $details{$key};
    }
    elsif ($colon)
    {
        if ($qreg)
        {
            $detail = $details{":$key$qreg"};
        }
        else
        {
            $detail = $details{":$key"};
        }
    }
    elsif ($qreg)
    {
        $detail = $details{"$key$qreg"};
    }

    if (!defined $detail)
    {
        $detail = $details{$key};
    }

    my $c1 = substr($key, 0, 1);

    if ($c1 eq '\'' || $c1 eq '|')
    {
        if (length $expression)
        {
            $expression = sprintf "%*s%s", $indent, "", $expression;

            printf "%s%*s! % 5u: (TBD)!\n", $expression, 64 - length $expression, " ", ++$sequence;

            $expression = "";
        }
        $indent -= 4;
    }

    $command = sprintf "%*s%s%s%s", $indent, "", $expression,
        length $expression ? " " : "", $command;

    if ($c1 eq '"' || $c1 eq '|')
    {
        $indent += 4;
    }

    printf "%s%*s! % 5u: ", $command, 64 - length $command, " ", ++$sequence;

    if (!defined $detail)
    {
        print "(n/a)";
    }
    elsif ($detail =~ /%c/)
    {
        printf $detail, ord($qreg);
    }
    elsif ($detail =~ /%s/)
    {
        printf $detail, $text;
    }
    else
    {
        print $detail;
    }

    print " !\n";
}
