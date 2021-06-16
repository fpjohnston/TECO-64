#!/usr/bin/perl

#
#  commands.pl - Build-process tool for TECO-64 text editor.
#
#  @copyright 2020-2021 Franklin P. Johnston / Nowwith Treble Software
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

my $warning = '///  *** Automatically generated file. DO NOT MODIFY. ***';
my $cmds;
my $e_cmds;
my $f_cmds;
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

if ($args{output} =~ /commands.h/msx)
{
    make_commands_h();
}
elsif ($args{output} =~ /exec.h/msx)
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

    print {*STDERR} "Creating $args{output}\n" or croak;
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
        $scan_list .= "extern bool $key(struct cmd *cmd);\n\n";
    }

    chomp $scan_list;

    foreach my $key (sort keys %exec_funcs)
    {
        $exec_list .= "extern void $key(struct cmd *cmd);\n\n";
    }

    chomp $exec_list;

    print {*STDERR} "Creating $args{output}\n" or croak;
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

    my $teco = $dom->findnodes("/teco/$NAME");

    die "Can't find program name\n" if !$teco;

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

            if (defined $scan)
            {
                $scan = "scan_$scan";
            }
            else
            {
                $scan = 'NULL';
            }

            if (defined $exec)
            {
                $exec = "exec_$exec";
            }

            my $parse = 'NULL';

            if (defined $format)
            {
                $parse = make_parse($format);
            }

            if ($name =~ /^E(.)$/msx)
            {
                $e_cmds .= make_entry($1, $parse, $scan, $exec);
            }
            elsif ($name ne 'FF' && $name =~ /^F(.)$/msx)
            {
                $f_cmds .= make_entry($1, $parse, $scan, $exec);
            }
            else
            {
                if ($name =~ /^\[(.)\]/msx)
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

    return;
}

sub make_entry
{
    my ($name, $parse, $scan, $exec) = @_;

    if ($name eq q{'} || $name eq q{\\})
    {
        $name = "'\\$name'";
    }
    elsif (length $name == 1 || $name =~ /^\\x..$/msx)
    {
        $name = "'$name'";
    }

    $name .= q{,};
    $name = sprintf '%-7s', $name;

    if ($parse ne 'NULL')
    {
        $scan_funcs{$parse} = 1;
    }

    if ($scan ne 'NULL')
    {
        $scan_funcs{$scan} = 1;
    }
    else
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

    $parse .= q{,};
    $parse = sprintf '%-15s', $parse;
    $scan .= q{,};
    $scan = sprintf '%-15s', $scan;
    $exec = sprintf '%-15s', $exec;

    my $entry = sprintf '%s', "    ENTRY($name  $parse  $scan  $exec),\n";

    if ($name =~ s/^('[[:upper:]]',)/\L$1/msx)
    {
        $entry .= sprintf '%s', "    ENTRY($name  $parse  $scan  $exec),\n";
    }

    return $entry;
}


sub make_parse
{
    my ($format) = @_;

    if (!length $format)
    {
        $format = 'X';
    }

    my %parse_table =
        (
            '@X//'       => 'parse_1',
            '@X///'      => 'parse_2',
            '+m,nX'      => 'parse_M',
            '+m,n@X//'   => 'parse_M1',
            '+m,n:X'     => 'parse_Mc',
            '+m,n:@X//'  => 'parse_Mc1',
            '+m,n:@X///' => 'parse_Mc2',
            '+m,n:Xq'    => 'parse_Mcq',
            '+m,n:@Xq//' => 'parse_Mcq1',
            '+n@X//'     => 'parse_N1',
            'X',         => 'parse_X',
            ':X',        => 'parse_c',
            ':@X//'      => 'parse_c1',
            ':@Xq//'     => 'parse_cq1',
            '::@X//'     => 'parse_d1',
            'X$',        => 'parse_escape',
            'nX!'        => 'parse_flag1',
            'm,nX!',     => 'parse_flag2',
            'm,n@X///'   => 'parse_m2',
            'm,n:X'      => 'parse_mc',
            'm,n:@X//'   => 'parse_mc1',
            'm,n:@X///'  => 'parse_mc2',
            'm,n:Xq'     => 'parse_mcq',
            'm,n::@X//'  => 'parse_md1',
            'm,n::@X///' => 'parse_md2',
            'm,nXq'      => 'parse_mq',
            'nX'         => 'parse_n',
            'n:X'        => 'parse_nc',
            'n:@X//'     => 'parse_nc1',
            'n:Xq'       => 'parse_ncq',
            'nXq'        => 'parse_nq',
            'X='         => 'parse_oper',
            '@Xq//'      => 'parse_q1',
        );

    if (exists $parse_table{$format})
    {
        return $parse_table{$format};
    }

    croak "Invalid format $format\n";
}


#
#  read_xml() - Read XML file and extract data for each name.
#

sub read_xml
{
    print {*STDERR} "Reading configuration file $args{input}...\n" or croak;

    # Read entire input file into string.

    my $xml = do { local(@ARGV, $RS) = $args{input}; <> };

    parse_commands($xml);

    return;
}
