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
#  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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

my $warning = '///  *** Automatically generated from template file. DO NOT MODIFY. ***';

# Command-line arguments

my %args = (
    config => undef,    # XML configuration file (usually options.xml)
    debug  => undef,    # Enable processing of debugging options
    output => undef,    # Output header file (usually options.h)
);

# Configuration options read from XML file

my %options = ();

# Sections to output to header file

my %header = (
    help  => undef,     # Help text
    enums => undef,     # Cases for command-line options
    short => undef,     # Short options for getopt_long()
    long  => undef,     # Long options for getopt_long()
);

Readonly my $MIN_CTRL => 1;
Readonly my $MAX_CTRL => 31;
Readonly my $NAME     => q{@} . 'name';

my $max_length = 0;

#
#  Parse our command-line options
#

GetOptions(
    'config=s'   => \$args{config},
    'template=s' => \$args{template},
    'debug'      => \$args{debug},
    'output=s'   => \$args{output}
);

check_file( $args{config},   'r', '-c' );
check_file( $args{template}, 'r', '-t' );
check_file( $args{output},   'w', '-o' );

read_xml();          # Parse command-line options
build_strings();     # Build data strings for header file
make_options_h();    # Create new header file

exit;

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
            croak "short option '$short' has no long option\n";
        }

        next if ( $debug && !$args{debug} );

        if ($debug)
        {
            $short = sprintf '\'\\%03u\'', $short;
            $debug_enums .= sprintf "    DEBUG_$debug = $short,\n";
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

    $header{enums} .= $debug_enums;

    chomp $header{help};
    chomp $header{long};
    chomp $header{enums};
    chop $header{enums};

    return;
}

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
#  Verify that option isn't a duplicate and has help text.
#

sub check_option
{
    my ( $line, $option, $short, $max_help ) = @_;

    croak "Duplicate option '$option' at line $line\n"
      if defined $options{$short};

    croak "No help text found for option '$option' at line $line\n"
      if $max_help < 0;

    return;
}

#
#  Get 'argument' child element of current node, and validate it.
#

sub get_argument
{
    my ( $tag, $child, $lineno ) = @_;

    my @child = $tag->findnodes("./$child");

    croak "Only one <$child> tag allowed for option at line $lineno"
      if ( $#child > 0 );

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

    croak "Only one <$child> tag allowed for option at line $lineno"
      if ( $#child > 0 );

    return q{} if $#child < 0;

    return q{ } if length $child[0]->textContent() == 0;

    return $child[0]->textContent();
}

#
#  Get 'help' child element of current node, and validate it.
#

sub get_help
{
    my ( $tag, $child, $lineno ) = @_;

    my @child = $tag->findnodes("./$child");

    croak "Missing <$child> tag for option at line $lineno"
      if ( $#child < 0 );

    my @help = ();

    for my $child (@child)
    {
        push @help, $child->textContent();
    }

    return @help;
}

#
#  Create options.h (or equivalent).
#

sub make_options_h
{
    my $file = $args{output};
    my $fh;
    my $template = read_file( $args{template} );

    $template =~ s"/\* \(INSERT: WARNING NOTICE\) \*/"$warning";
    $template =~ s"/\* \(INSERT: HELP OPTIONS\) \*/"$header{help}";
    $template =~ s"/\* \(INSERT: ENUM OPTIONS\) \*/"$header{enums}";
    $template =~ s"/\* \(INSERT: SHORT OPTIONS\) \*/"$header{short}";
    $template =~ s"/\* \(INSERT: LONG OPTIONS\) \*/"$header{long}";

    print {*STDERR} "Creating header file $file\n" or croak;

    ## no critic (RequireBriefOpen)

    open $fh, '>', $file or croak "Can't open $file: $OS_ERROR";

    print {$fh} $template or croak "Can't open $file: $OS_ERROR";

    close $fh or croak "Can't close $file: $OS_ERROR";

    ## use critic

    return;
}

#
#  parse_options() - Find all options in XML data.
#
#  Note that this subroutine is called twice: once to calculate the spacing
#  for the help text for each option, and then to actually store the data.
#

sub parse_options
{
    my ( $xml, $pass ) = @_;

    my $dom = XML::LibXML->load_xml( string => $xml, line_numbers => 1 );

    my $name = $dom->findnodes("/teco/$NAME");

    die "Can't find program name\n" if !$name;

    foreach my $section ( $dom->findnodes('/teco/section') )
    {
        my $line  = $section->line_number();
        my $title = $section->getAttribute('title');

        croak "Section element missing title at line $line" if !defined $title;

        if ( $pass == 2 )
        {
            $header{help} .= "    \"\",\n    \"$title:\",\n    \"\",\n";
        }

        foreach my $option ( $section->findnodes('./option') )
        {
            my @help   = get_help( $option, 'help', $line );
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

    if ( $pass == 2 )
    {
        my $options = scalar keys %options;

        croak "No options found in $args{config}\n" if ( $options == 0 );

        printf "...%u option%s found\n", $options, $options == 1 ? q{} : 's';
    }

    return;
}

#
#  read_xml() - Read XML file and extract data for each name.
#

sub read_xml
{
    print {*STDERR} "Reading configuration file $args{config}...\n" or croak;

    # Read entire input file into string.

    my $xml = do { local ( @ARGV, $RS ) = $args{config}; <> };

    parse_options( $xml, 1 );
    parse_options( $xml, 2 );

    return;
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
        croak "Unexpected short name for debug option '--$long' at line"
          . " $line\n"
          if length $short;

        return if !$args{debug};

        croak "Debug option out of range at line $line\n"
          if $debug < $MIN_CTRL or $debug > $MAX_CTRL;

        $short = $debug;

        die "Invalid debug option '--$long' at line $line\n"
          if !length $short;

        check_option( $line, "--$long", $short, $#help );
    }
    else
    {
        croak "Missing short name for option at line $line\n"
          if !length $short;

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
