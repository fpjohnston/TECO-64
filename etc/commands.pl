#!/usr/bin/perl

#
#  commands.pl - Build-process tool for TECO-64 text editor.
#
#  @copyright 2020 Franklin P. Johnston / Nowwith Treble Software
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

use File::Basename;
use File::Spec;
use File::Slurp;
use Getopt::Long;
use XML::LibXML;
use English qw( -no_match_vars );
use autodie qw( open close );
use POSIX qw( strftime );
use Readonly;
use Carp;

# Command-line arguments

my %args = (
            input    => undef,      # Input file (usually errors.xml)
            template => undef,      # Template file
            output   => undef,      # Output file
);


Readonly my $NAME => q{@} . 'name';

my $warning = "///  *** Automatically generated file. DO NOT MODIFY. ***";
my $cmds;
my $e_cmds;
my $f_cmds;
my %commands;
my %parse_funcs;
my %macros;
my %scan_funcs;
my %exec_funcs;

#
#  Parse our command-line options
#

GetOptions('input=s'    => \$args{input},
           'template=s' => \$args{template},
           'output=s'   => \$args{output});

check_file($args{input},    'r', '-e');
check_file($args{template}, 'r', '-t');
check_file($args{output},   'w', '-o');

read_xml();                             # Parse XML file

my $template = read_file($args{template});

if ($args{output} =~ /commands.h/)
{
    make_commands_h();
}
elsif ($args{output} =~ /exec.h/)
{
    make_exec_h();
}
else
{
    croak "Unknown output file $args{output}";
}

exit;

#
#  Validate that we have a file name for specified option, that the file
#  exists, that it is readable, and that it is a plain file.
#

sub check_file
{
    my ($file, $mode, $option) = @_;

    croak "No file specified for $option option\n" if !$file;

    return if $mode eq 'w';

    croak "File $file does not exist" if !-e $file;
    croak "File $file is not readable" if !-r $file;
    croak "File $file is not a plain file" if !-f $file;

    return;
}

 
#
#  Get child element of current node, and validate it.
#

sub get_child
{
    my ($tag, $child, $lineno) = @_;

    my @child = $tag->findnodes("./$child");

    croak "Only one <$child> tag allowed for error at line $lineno"
          if ($#child > 0);

    return q{} if $#child < 0;

    return q{ } if length $child[0]->textContent() == 0;

    return $child[0]->textContent;
}


#
#  Get 'detail' child elements of current node, and validate it.
#

sub get_details
{
    my ($tag, $child, $lineno) = @_;

    my @child = $tag->findnodes("./$child");

    croak "Missing <$child> tag for error at line $lineno"
          if ($#child < 0);

    my @details = ();

    for my $child (@child)
    {
        push @details, $child->textContent();
    }

    return @details;
}


#
#  Create commands.h (or equivalent)
#

sub make_commands_h
{
    my $fh;

    print {*STDERR} "Creating $args{output}\n";
    open $fh, '>', $args{output};

    printf {$fh} $template, $warning, $cmds, $e_cmds, $f_cmds;

    close $fh;

    return;
}


#
#  Create exec.h (or equivalent)
#

sub make_exec_h
{
    my $fh;
    my $scan_list;
    my $exec_list;

    foreach my $key (sort keys %scan_funcs)
    {
#        print "$key -> $macros{$key}\n" if ($key =~ /^parse_/);
    }

    foreach my $key (sort keys %scan_funcs)
    {
        $scan_list .= "extern bool $key(struct cmd *cmd);\n\n";
    }

    chomp $scan_list;

    foreach my $key (sort keys %exec_funcs)
    {
        $exec_list .= "extern void $key(struct cmd *cmd);\n\n";
    }

    chomp $exec_list;

    print {*STDERR} "Creating $args{output}\n";
    open $fh, '>', $args{output};

    printf {$fh} $template, $warning, $scan_list, $exec_list;

    close $fh;

    return;
}


#
#  parse_commands() - Find all TECO commands in XML data.
#

sub parse_commands
{
    my ($xml) = @_;

    my $dom = XML::LibXML->load_xml(string => $xml, line_numbers => 1);

    my $name = $dom->findnodes("/teco/$NAME");

    die "Can't find program name\n" unless $name;

    foreach my $section ($dom->findnodes('/teco/section'))
    {
        my $line = $section->line_number();
        my $title = $section->getAttribute('title');

        croak "Section element missing title at line $line" if !defined $title;

        foreach my $command ($section->findnodes('./command'))
        {
            my $name    = $command->getAttribute('name');
            my $format  = $command->getAttribute('parse');
            my $scan    = $command->getAttribute('scan');
            my $exec    = $command->getAttribute('exec');

            $scan = 'NULL' unless defined $scan;

            my $parse = 'NULL';

            if (defined $format)
            {
                $parse = make_parse($format);
            }

            if ($name =~ /^E(.)$/)
            {
                $e_cmds .= make_entry($1, $parse, $scan, $exec);
            }
            elsif ($name ne 'FF' && $name =~ /^F(.)$/)
            {
                $f_cmds .= make_entry($1, $parse, $scan, $exec);
            }
            else
            {
                if ($name =~ /^\[(.)\]/)
                {
                    $name = $1;
                }

                $cmds .= make_entry($name, $parse, $scan, $exec);
            }
        }
    }

    chomp $cmds;
    chomp $e_cmds;
    chomp $f_cmds;
}
            
sub make_entry
{
    my ($name, $parse, $scan, $exec) = @_;

    if ($name eq '\'' || $name eq '\\')
    {
        $name = "'\\$name'";
    }
    elsif (length $name == 1 || $name =~ /^\\x..$/)
    {
        $name = "'$name'";
    }

    $name .= ",";
    $name = sprintf("%-7s", $name);

    $scan_funcs{$parse} = 1 unless $parse eq 'NULL';
    $scan_funcs{$scan} = 1 unless $scan eq 'NULL';

    if ($scan eq 'NULL')
    {
        $scan = $parse;
        $parse = 'NULL';
    }

    if (!defined $parse)
    {
        $parse = 'NULL';
    }

    if (defined $exec)
    {
        $exec_funcs{$exec} = 1;
    }
    else
    {
        $exec = 'NULL';
    }

    $parse .= ',';
    $parse = sprintf("%-15s", $parse);
    $scan .= ',';
    $scan = sprintf("%-15s", $scan);
    $exec = sprintf("%-15s", $exec);

    my $entry = sprintf "%s", "    ENTRY($name  $parse  $scan  $exec),\n";

    if ($name =~ s/^('[A-Z]',)/\L$1/)
    {
        $entry .= sprintf "%s", "    ENTRY($name  $parse  $scan  $exec),\n";
    }

    return $entry;
}


sub make_parse
{
    my ($format) = @_;

    my $orig = $format;
    my $macro;
    my $name = 'parse_';
    my $atsign;

    if ($format eq 'X=')
    {
        $name .= 'oper';
        $macro .= '  reject_colon  reject_atsign';
        $macros{$name} = $macro;

        return $name;
    }

    if ($format eq 'X$')
    {
        $name .= 'escape';
        $macro .= '  reject_colon  reject_atsign';
        $macros{$name} = $macro;

        return $name;
    }

    if ($format eq '!!X')
    {
        $name .= 'flag2';
        $macro .= '  reject_colon  reject_atsign';
        $macros{$name} = $macro;

        return $name;
    }

    if ($format eq '!X')
    {
        $name .= 'flag1';
        $macro .= '  reject_colon  reject_atsign';
        $macros{$name} = $macro;

        return $name;
    }

    if ($format eq 'X' || !length $format)
    {
        $name .= 'X';
    }

    if ($format =~ s[^\+m,n][])
    {
        $name .= 'M';
        $macro .= '  reject_neg_m';
        $macro .= '  require_n';
    }
    elsif ($format =~ s[^m,n][])
    {
        $name .= 'm';
        $macro .= '  require_n';
    }
    elsif ($format =~ s[^\+n][])
    {
        $name .= 'N';
        $macro .= '  reject_neg_n';
        $macro .= '  require_n';
    }
    elsif ($format =~ s[^n][])
    {
        $name .= 'n';
        $macro .= '  reject_m';
    }
    else
    {
        $macro .= '  reject_m';
        $macro .= '  reject_n';
    }

    if ($format =~ s[^::][])
    {
        $name .= 'd';
    }
    elsif ($format =~ s[^:][])
    {
        $name .= 'c';
        $macro .= '  reject_dcolon';
    }
    else
    {
        $macro .= '  reject_colon';
    }

    if ($format =~ s[^@][])
    {
        $atsign = 1;
    }

    $format =~ s[^X][];

    if ($format =~ s[^q][])
    {
        $name .= 'q';
        $macro .= '  scan_qreg';
    }

    if ($format =~ s[^///][])
    {
        $name .= '2';
        $macro .= '  scan_text(2)';
    }
    elsif ($format =~ s[^//][])
    {
        $name .= '1';
        $macro .= '  scan_text(1)';
    }
    elsif (defined $atsign)
    {
        croak "Invalid format $orig\n";
    }
    else
    {
        $macro .= '  reject_atsign';
    }

    $format =~ s[^\$][];
    $macros{$name} = $macro;

    return $name;
}


#
#  read_xml() - Read XML file and extract data for each name.
#

sub read_xml
{
    print {*STDERR} "Reading configuration file $args{input}...\n";

    # Read entire input file into string.

    my $xml = do { local(@ARGV, $RS) = $args{input}; <> };

    parse_commands($xml);

    return;
}
