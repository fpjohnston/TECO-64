#!/usr/bin/perl

#
#  options.pl - Build-process tool for TECO-64 text editor.
#
#  This script reads data from options.xml to determine command-line options and
#  program information for the teco utility. This data is output to a C header
#  file, options.h, which in turn is subsequently #included by options.c. The
#  purpose for doing this is to ensure tight coupling between data used in mul-
#  tiple tables, macros, and strings in teco, thus reducing possible errors due
#  to subsequent modifications or enhancements.
#
#  The output file, options.h, specifically includes:
#
#  1. A string defining the entire help text.
#  2. An enumerated list of values defining cases for each command-line option.
#  3. A string defining the short command-line options (including whether they
#     have optional or required arguments), used by the getopt_long() function.
#  4. A table of long options, used by the getopt_long() function.
#
#  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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

use Carp;
use Getopt::Long;
use Readonly;
use XML::LibXML;

use lib 'etc/';
use Teco qw(teco_read teco_write);

# Command-line arguments

my %args = (
    input  => undef,    # Options file (usually options.xml)
    debug  => undef,    # Enable processing of debugging options
    output => undef,    # Output file (usually options.h)
);

# Configuration options read from XML file

my %options = ();

# Sections to output to header file

my %header = (
    help  => undef,     # Help text
    enums => undef,     # Cases for command-line options
    debug => undef,     # Defines for debug options
    short => undef,     # Short options for getopt_long()
    long  => undef,     # Long options for getopt_long()
);

Readonly my $MIN_CTRL => 1;
Readonly my $MAX_CTRL => 31;

my $max_length = 0;

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

    my $input    = teco_read( $args{input} );
    my $template = teco_read( $args{template} );

    print "Reading configuration file $args{input}...\n";

    parse_xml( $input, 1 );

    my $nopts = parse_xml( $input, 2 );

    if ( $nopts == 0 )
    {
        print "No options found in $args{input}\n";
    }

    printf "...%u option%s found\n", $nopts, $nopts == 1 ? q{} : 's';

    build_strings();            # Build data strings for header file
    make_options($template);    # Create new header file

    return;
}

#
#  Build data strings needed for header file.
#

sub build_strings
{
    my $debug_enums = q{};

    foreach my $short ( sort keys %options )
    {
        my %option = %{ $options{$short} };

        my $long    = $option{long};
        my $debug   = $option{debug};
        my $argtype = $option{argtype};
        my $help    = $option{help};

        if ( !length $long )
        {
            die "short option '$short' has no long option\n";
        }

        next if ( $debug && !$args{debug} );

        if ($debug)
        {
            $short = sprintf '\'\\%03u\'', $short;
            $header{debug} .= sprintf "#define DEBUG_$debug $short"
              . "          ///< Debug command-line option $long\n";
        }
        else
        {
            $header{short} .= $short;
            $header{enums} .= "    OPTION_$short = '$short',\n";
            $short = "'$short'   ";
        }

        if ( $argtype =~ /required_argument/ms )
        {
            $header{short} .= q{:};
        }
        elsif ( $argtype =~ /optional_argument/ms )
        {
            $header{short} .= q{::};
        }

        $header{long} .= sprintf "    { %-17s %-18s  NULL,  $short },\n",
          "\"$long\",", "$argtype,";
    }

    if ( !defined $header{debug} )
    {
        $header{debug} = "// No debug options in this build\n";
    }

    chomp $header{help};
    chomp $header{long};
    chomp $header{enums};
    chomp $header{debug};
    chop $header{enums};

    return;
}

#
#  Verify that option isn't a duplicate and has help text.
#

sub check_option
{
    my ( $line, $option, $short, $max_help ) = @_;

    if ( defined $options{$short} )
    {
        print "Duplicate $option option at line $line\n";

        exit 1;
    }

    if ( $max_help < 0 )
    {
        print "No help text found for option '$option' at line $line\n";

        exit 1;
    }

    return;
}

#
#  Get 'argument' child element of current node, and validate it.
#

sub get_argument
{
    my ( $tag, $child, $lineno ) = @_;

    my @child = $tag->findnodes("./$child");

    if ( $#child > 0 )
    {
        print "Duplicate <$child> tag for option at line $lineno\n";
    }

    if ( $#child == 0 )
    {
        return 'required_argument' if ( $child[0] =~ /required/ims );
        return 'optional_argument' if ( $child[0] =~ /optional/ims );
    }

    return 'no_argument';
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
#  Create options.h (or equivalent).
#

sub make_options
{
    my ($template) = @_;
    my %changes = (
        'HELP OPTIONS'  => $header{help},
        'ENUM OPTIONS'  => $header{enums},
        'DEBUG OPTIONS' => $header{debug},
        'SHORT OPTIONS' => $header{short},
        'LONG OPTIONS'  => $header{long},
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
        'debug'    => \$args{debug},
        'output=s' => \$args{output}
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
#  Note that this subroutine is called twice: once to calculate the spacing
#  for the help text for each option, and then to actually store the data.
#

sub parse_xml
{
    my ( $xml, $pass ) = @_;
    my $dom = XML::LibXML->load_xml( string => $xml, line_numbers => 1 );

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

        if ( $pass == 2 )
        {
            $header{help} .= "    \"\",\n    \"$title:\",\n    \"\",\n";
        }

        foreach my $option ( $section->findnodes('./option') )
        {
            my @help   = get_details( $option, 'help', $line );
            my %option = (
                pass    => $pass,
                line    => $option->line_number(),
                debug   => $option->getAttribute('debug'),
                short   => get_child( $option, 'short_name', $line ),
                long    => get_child( $option, 'long_name', $line ),
                argtype => get_argument( $option, 'argument', $line ),
                help    => \@help,
            );

            save_option(%option);
        }
    }

    return scalar keys %options;
}

#
#  save_option() - Validate option and save it for sorting.
#

sub save_option
{
    my %option = @_;

    my $pass    = $option{pass};
    my $line    = $option{line};
    my $debug   = $option{debug};
    my $short   = $option{short};
    my $long    = $option{long};
    my $argtype = $option{argtype};
    my @help    = @{ $option{help} };

    croak "Missing long option at line $line\n" if !length $long;

    if ($debug)
    {
        if ( length $short != 0 )
        {
            die "Unexpected short name for debug option '--$long' at "
              . "line $line\n";
        }

        return if !$args{debug};

        if ( $debug < $MIN_CTRL or $debug > $MAX_CTRL )
        {
            die "Debug option out of range at line $line\n";
        }

        $short = $debug;

        if ( length $short == 0 )
        {
            die "Invalid debug option '--$long' at line $line\n";
        }

        check_option( $line, "--$long", $short, $#help );
    }
    else
    {
        if ( length $short == 0 )
        {
            die "Missing short name for option at line $line\n";
        }

        check_option( $line, "-$short", $short, $#help );
    }

    my $options;
    my $help;    # 1st line of help text

    if (
        $help[0] =~ m{
                      (.*)              # Start of text
                      \'                # Value delimiter
                      (.+)              # Option value
                      \'                # Value delimiter
                      (.*)              # Remainder of text
                     }msx
      )
    {
        $help    = sprintf "$1$3";
        $options = sprintf "%s--$long=$2", $debug ? q{} : "-$short, ";
    }
    else
    {
        $help    = $help[0];
        $options = sprintf "%s--$long", $debug ? q{} : "-$short, ";
    }

    # First pass is just to figure out how much space we need between the
    # options (short and long) and the subsequent descriptive help text.

    if ( $pass == 1 )
    {
        if ( $max_length < length $options )
        {
            $max_length = length $options;
        }

        return;
    }

    my $format = "    \"  %-*s   %s\",\n";

    for my $i ( 0 .. $#help )
    {
        $header{help} .= sprintf $format, $max_length, $options, $help[$i];
        $options = q{    };
    }

    $options{$short} = {
        line    => $line,
        long    => $long,
        argtype => $argtype,
        help    => $help,
        debug   => $debug,
    };

    return;
}
