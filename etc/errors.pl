#!/usr/bin/perl

#
#  errors.pl - Build-process tool for TECO-64 text editor.
#
#  @copyright 2020-2022 Franklin P. Johnston / Nowwith Treble Software
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

use Getopt::Long;
use XML::LibXML;

use lib 'etc/';
use Teco qw(teco_read teco_write);

# Command-line arguments

my %args = (
    input    => undef,    # Input XML file
    template => undef,    # Template file
    output   => undef,    # Output file
);

#
#  Main program entry.
#

main();

exit 0;

#
#  This subroutine is defined so that variables we use therein are not global
#  and therefore don't cause perltidy to complain if they happen to be the
#  same as variables used in the subroutines below.
#

sub main
{
    parse_options();

    my $xml      = teco_read( $args{input} );
    my $template = teco_read( $args{template} );

    print "Reading configuration file $args{input}...\n";

    my %errors;

    my $hash_ref = parse_xml( $xml, 2 );

    if ( $args{output} =~ /errcodes.h/msx )
    {
        make_errcodes( $template, $hash_ref );
    }
    elsif ( $args{output} =~ /errors.md/msx )
    {
        make_errors( $template, $hash_ref );
    }
    elsif ( $args{output} =~ /errtables.h/msx )
    {
        make_errtables( $template, $hash_ref );
    }
    else
    {
        die "Unknown output file: $args{output}\n";
    }

    return;
}

#
#  Get child element of current node, and validate it.
#

sub get_child
{
    my ( $tag, $child, $lineno ) = @_;

    my @child = $tag->findnodes("./$child");

    if ( $#child > 0 )
    {
        print "Duplicate <$child> tag at line $lineno\n";

        exit 1;
    }

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

    if ( $#child < 0 )
    {
        print "Missing <$child> tag at line $lineno\n";

        exit 1;
    }

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

sub make_errcodes
{
    my ( $template, $hash_ref ) = @_;
    my %errors = %{$hash_ref};
    my $errcodes;

    foreach my $code ( sort keys %errors )
    {
        my $message = $errors{$code}{message};

        if ( $code eq 'DET' || $code eq 'ERR' )
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

    my %changes = ( 'ERROR CODES' => $errcodes, );

    teco_write( $template, $args{output}, %changes );

    return;
}

#
#  Create errors.md
#

sub make_errors
{
    my ( $template, $hash_ref ) = @_;
    my %errors = %{$hash_ref};
    my $errors;

    foreach my $code ( sort keys %errors )
    {
        my $message = $errors{$code}{message};

        if ( $code eq 'DET' || $code eq 'ERR' )
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

    my %changes = ( 'ERROR CODES' => $errors, );

    teco_write( $template, $args{output}, %changes );

    return;
}

#
#  Create errtables.h.
#

sub make_errtables
{
    my ( $template, $hash_ref ) = @_;
    my %errors = %{$hash_ref};
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

    my %changes = (
        'ERROR MESSAGES' => $errlist,
        'ERROR DETAILS'  => $errhelp,
    );

    teco_write( $template, $args{output}, %changes );

    return;
}

#
#  Parse command-line options.
#

sub parse_options
{
    GetOptions(
        'input=s'    => \$args{input},
        'template=s' => \$args{template},
        'output=s'   => \$args{output}
    );

    die "Not enough input files\n" if $#ARGV < 1;
    die "Too many input files\n"   if $#ARGV > 1;

    $args{input}    = $ARGV[0];
    $args{template} = $ARGV[1];

    return;
}

#
#  parse_xml() - Parse XML file.
#

sub parse_xml
{
    my ( $xml, $type ) = @_;
    my $dom = XML::LibXML->load_xml( string => $xml, line_numbers => 1 );
    my %errors;

    ## no critic (ProhibitInterpolationOfLiterals)

    my $teco = $dom->findvalue(qq{/teco/\@name});

    ## use critic

    die "Missing name attribute\n"         if !defined $teco;
    die "Invalid name attribute '$teco'\n" if $teco ne 'teco';

    foreach my $section ( $dom->findnodes('/teco/section') )
    {
        my $line  = $section->line_number();
        my $title = $section->getAttribute('title');

        if ( !defined $title )
        {
            die "Missing title at line $line\n";
        }

        foreach my $error ( $section->findnodes('./error') )
        {
            my @details = get_details( $error, 'detail', $line );

            my $code    = get_child( $error, 'code',    $line );
            my $message = get_child( $error, 'message', $line );

            if ( length $code == 0 )
            {
                die "Missing error code at line $line\n";
            }

            if ( length $message == 0 )
            {
                die "Missing error message at line $line\n";
            }

            $errors{$code}{line}    = $line;
            $errors{$code}{message} = $message;
            $errors{$code}{details} = \@details;
        }
    }

    return \%errors;
}
