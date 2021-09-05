#!/usr/bin/perl

#
#  errors.pl - Build-process tool for TECO-64 text editor.
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

Readonly my $INSERT_WARNING  => '/\* \(INSERT: WARNING NOTICE\) \*/';
Readonly my $INSERT_ERRCODES => '/\* \(INSERT: ERROR CODES\) \*/';
Readonly my $INSERT_ERRLIST  => '/\* \(INSERT: ERROR MESSAGES\) \*/';
Readonly my $INSERT_ERRHELP  => '/\* \(INSERT: ERROR DETAILS\) \*/';

Readonly my $WARNING =>
  '///  *** Automatically generated from template file. DO NOT MODIFY. ***';

# Command-line arguments

my %args = (
    input    => undef,    # Input file (usually errors.xml)
    template => undef,    # Template file
    output   => undef,    # Output file
);

Readonly my $NAME => q{@} . 'name';

my %errors;

#
#  Parse our command-line options
#

GetOptions(
    'input=s'    => \$args{input},
    'template=s' => \$args{template},
    'output=s'   => \$args{output}
);

check_file( $args{input},    'r', '-e' );
check_file( $args{template}, 'r', '-t' );
check_file( $args{output},   'w', '-o' );

read_xml();    # Parse XML file

my $template = read_file( $args{template} );

if ( $args{output} =~ /errcodes.h/msx )
{
    make_errcodes_h();
}
elsif ( $args{output} =~ /errors.md/msx )
{
    make_errors_md();
}
elsif ( $args{output} =~ /errtables.h/msx )
{
    make_errtables_h();
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
    my ( $file, $mode, $option ) = @_;

    croak "No file specified for $option option\n" if !$file;

    return if $mode eq 'w';

    croak "File $file does not exist"      if !-e $file;
    croak "File $file is not readable"     if !-r $file;
    croak "File $file is not a plain file" if !-f $file;

    return;
}

#
#  Get child element of current node, and validate it.
#

sub get_child
{
    my ( $tag, $child, $lineno ) = @_;

    my @child = $tag->findnodes("./$child");

    croak "Only one <$child> tag allowed for error at line $lineno"
      if ( $#child > 0 );

    return q{} if $#child < 0;

    return q{ } if length $child[0]->textContent() == 0;

    return $child[0]->textContent;
}

#
#  Get 'detail' child elements of current node, and validate it.
#

sub get_details
{
    my ( $tag, $child, $lineno ) = @_;

    my @child = $tag->findnodes("./$child");

    croak "Missing <$child> tag for error at line $lineno"
      if ( $#child < 0 );

    my @details = ();

    for my $child (@child)
    {
        push @details, $child->textContent();
    }

    return @details;
}

#
#  Create errcodes.h (or equivalent)
#

sub make_errcodes_h
{
    my $fh;
    my $errcodes;

    foreach my $code ( sort keys %errors )
    {
        my $message = $errors{$code}{message};

        if ( $code eq 'ERR' )
        {
            $message =~ s/%s/(error message)/gmsx;
        }
        else
        {
            $message =~ s/%s/foo/gmsx;
        }

        $message =~ s/%c/x/gmsx;

        $errcodes .= sprintf "    E_%s,          ///< %s\n", $code, $message;
    }

    $template =~ s/$INSERT_WARNING/$WARNING/ms;
    $template =~ s/$INSERT_ERRCODES/$errcodes/ms;

    print {*STDERR} "Creating $args{output}\n" or croak;

    ## no critic (RequireBriefOpen)

    open $fh, '>', $args{output} or croak "Can't open $args{output}\n";

    print {$fh} $template or croak "Can't print to $args{output}\n";

    close $fh or croak "Can't close $args{output}\n";

    # use critic

    return;
}

#
#  Create errors.md (or equivalent)
#

sub make_errors_md
{
    my $fh;
    my $errors;

    foreach my $code ( sort keys %errors )
    {
        my $message = $errors{$code}{message};

        if ( $code eq 'ERR' )
        {
            $message =~ s/%s/(error message)/gmsx;
        }
        else
        {
            $message =~ s/%s/foo/gmsx;
        }

        $message =~ s/%c/*x*/gmsx;

        my @details = @{ $errors{$code}{details} };
        my $details = join q{ }, @details;

        $errors .= sprintf "| <nobr>?%s</nobr> | <nobr>%s</nobr> | %s |\n",
          $code, $message, $details;
    }

    $template =~ s/$INSERT_ERRCODES/$errors/ms;

    print {*STDERR} "Creating $args{output}\n" or croak;

    ## no critic (RequireBriefOpen)

    open $fh, '>', $args{output} or croak "Can't open $args{output}\n";

    print {$fh} $template or croak "Can't print to $args{output}\n";

    close $fh or croak "Can't close $args{output}\n";

    ## use critic

    return;
}

sub make_errtables_h
{
    my $fh;
    my $errlist;
    my $errhelp;

    foreach my $code ( sort keys %errors )
    {
        my $message = $errors{$code}{message};

        $message =~ s/%c/%s/msxg;

        my @details = @{ $errors{$code}{details} };

        for my $i ( 0 .. $#details )
        {
            $details[$i] =~ s/"/\\"/msxg;
        }

        my $details = join " \"\n              \"", @details;

        $errlist .= sprintf "    [E_%s] = { \"%s\",  \"%s\" },\n", $code,
          $code, $message;
        $errhelp .= sprintf "    [E_%s] = \"%s\",\n", $code, $details;
    }

    $template =~ s/$INSERT_WARNING/$WARNING/ms;
    $template =~ s/$INSERT_ERRLIST/$errlist/ms;
    $template =~ s/$INSERT_ERRHELP/$errhelp/ms;

    print {*STDERR} "Creating $args{output}\n" or croak;

    ## no critic (RequireBriefOpen)

    open $fh, '>', $args{output} or croak "Can't open $args{output}\n";

    print {$fh} $template or croak "Can't print to $args{output}\n";

    close $fh or croak "Can't close $args{output}\n";

    ## use critic

    return;
}

#
#  parse_errors() - Find all error messages in XML data.
#

sub parse_errors
{
    my ($xml) = @_;

    my $dom = XML::LibXML->load_xml( string => $xml, line_numbers => 1 );

    my $name = $dom->findnodes("/teco/$NAME");

    die "Can't find program name\n" if !$name;

    foreach my $section ( $dom->findnodes('/teco/section') )
    {
        my $line  = $section->line_number();
        my $title = $section->getAttribute('title');

        croak "Section element missing title at line $line" if !defined $title;

        foreach my $error ( $section->findnodes('./error') )
        {
            my @details = get_details( $error, 'detail', $line );

            my $code    = get_child( $error, 'code',    $line );
            my $message = get_child( $error, 'message', $line );

            croak "Missing error code at line $line\n"    if !length $code;
            croak "Missing error message at line $line\n" if !length $message;

            $errors{$code}{line}    = $line;
            $errors{$code}{message} = $message;
            $errors{$code}{details} = \@details;
        }
    }

    return;
}

#
#  read_xml() - Read XML file and extract data for each name.
#

sub read_xml
{
    print {*STDERR} "Reading configuration file $args{input}...\n" or croak;

    # Read entire input file into string.

    my $xml = do { local ( @ARGV, $RS ) = $args{input}; <> };

    parse_errors($xml);

    return;
}
