#!/usr/bin/perl

#
#  errors.pl - Build-process tool for TECO-64 text editor.
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
my %externs;

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
    my $extern;

    foreach my $key (sort keys %externs)
    {
        if ($key ne 'exec_escape')
        {
            $extern .= "extern void $key(struct cmd *cmd);\n\n";
        }
    }

    chomp $extern;

    print {*STDERR} "Creating $args{output}\n";
    open $fh, '>', $args{output};

    printf {$fh} $template, $warning, $extern;

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
            my $name = $command->getAttribute('name');
            my $format = $command->getAttribute('format');
            my $exec = $command->getAttribute('exec');

            if ($exec)
            {
                if ($exec =~ /^exec_E/)
                {
                    $name =~ s/^E//;

                    $e_cmds .= make_entry($name, $format, $exec);

                    next;
                }
                elsif ($exec =~ /^exec_F/)
                {
                    $name =~ s/^F//;

                    $f_cmds .= make_entry($name, $format, $exec);

                    next;
                }
            }

            $cmds .= make_entry($name, $format, $exec);
        }
    }

    chomp $cmds;
    chomp $e_cmds;
    chomp $f_cmds;
}
            
sub make_entry
{
    my ($name, $format, $exec) = @_;
    my $options;

    if ($name eq '\'' || $name eq '\\')
    {
        $name = "'\\$name'";
    }
    elsif (length $name == 1 || $name =~ /^\\x..$/)
    {
        $name = "'$name'";
    }

    my $orig = $format;

    if ($format)
    {
        if ($format =~ s/^m,(.*)/$1/)
        {
            $options .= "OPT_M|";
        }

        if ($format =~ s/^n(.*)/$1/)
        {
            $options .= "OPT_N|";
        }

        if ($format =~ s/^::(.*)/$1/)
        {
            $options .= "OPT_C|OPT_D|";
        }
        elsif ($format =~ s/^:(.*)/$1/)
        {
            $options .= "OPT_C|";
        }

        if ($format =~ s/^@(.*)/$1/)
        {
            $options .= "OPT_A|";
        }

        if ($format =~ s{^X(.*)}{$1})
        {
        }

        if ($format =~ s/^q(.*)/$1/)
        {
            $options .= "OPT_Q|";
        }

        if ($format =~ s{^///(.*)}{$1})
        {
            $options .= "OPT_T1|OPT_T2|";
        }
        elsif ($format =~ s{^//(.*)}{$1})
        {
            $options .= "OPT_T1|";
        }

        if ($format =~ s{!(.*)}{$1})
        {
            $options .= "OPT_F|";
        }

        if ($format =~ s{\$(.*)}{$1})
        {
            $options .= "OPT_M|OPT_N|OPT_E|";
        }

        if ($format)
        {
            print "Invalid format $orig for $name command\n";

            exit;
        }

        if ($options)
        {
            chop $options;
#            $options = "($options)";
        }
        else
        {
            $options = "0";
        }
    }
    elsif ($exec)
    {
        $options = "OPT_O";
    }

    $name .= ",";
    $name = sprintf("%-7s", $name);

    $externs{$exec} = 1;

    $exec .= ",";
    $exec = sprintf("%-15s", $exec);

    my $entry = sprintf "%s", "    ENTRY($name  $exec  $options),\n";

    if ($name =~ s/^('[A-Z]',)/\L$1/)
    {
        $entry .= sprintf "%s", "    ENTRY($name  $exec  $options),\n";
    }

    return $entry;
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
