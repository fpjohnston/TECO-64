#!/usr/bin/perl

#
#  make_commands.pl - Build-process tool for TECO-64 text editor.
#
#  @copyright 2020-2023 Franklin P. Johnston / Nowwith Treble Software
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
use English qw( -no_match_vars );
use XML::LibXML;

use lib 'etc/';
use Teco qw(teco_read teco_write);

# Command-line arguments

#<<< perltidy: preserve format

my %args = (
    input    => undef,                  # Input XML file
    output   => undef,                  # Output header file
    template => undef,                  # Template file
);

#>>>

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

    my $xmlfile  = teco_read( $args{input} );
    my $template = teco_read( $args{template} );

    print "Reading $args{input}...";

    my $array_ref = parse_xml( $xmlfile, 1 );

    if ( $args{output} =~ /commands.h/msx )
    {
        my ( $cmds, $e_cmds, $f_cmds ) = make_commands($array_ref);

        write_commands( $template, $cmds, $e_cmds, $f_cmds );
    }
    elsif ( $args{output} =~ /exec.h/msx )
    {
        my ( $scan_list, $exec_list ) = make_exec($array_ref);

        write_exec( $template, $scan_list, $exec_list );
    }
    else
    {
        print "Unknown output file $args{output}\n";

        exit 1;
    }

    return;
}

#
#  Create macro line for each command defined in commands.h
#

sub make_commands
{
    my ($ref) = @_;
    my @commands = @{$ref};
    my $cmds;
    my $e_cmds;
    my $f_cmds;

    foreach (@commands)
    {
        my $name    = $_->{name};
        my $scan    = $_->{scan};
        my $exec    = $_->{exec};
        my $mn_args = $_->{mn_args};

        if ( $name eq q{'} || $name eq q{\\} )
        {
            $name = "'\\$name'";
        }
        elsif ( $name eq q{F'} )
        {
            $name = q{'\\''};
        }
        elsif ( length $name == 1 || $name =~ /^\\x..$/msx )
        {
            $name = "'$name'";
        }

        if ( !defined $exec )
        {
            $exec = 'NULL';
        }

        $scan .= q{,};
        $scan = sprintf '%-15s', $scan;
        $exec .= q{,};
        $exec = sprintf '%-15s', $exec;

        if ( $name =~ /^E(.)$/ms )
        {
            $name = "'$1'";
        }
        elsif ( $name =~ /^F(.)$/ms )
        {
            $name = "'$1'";
        }

        $name .= q{,};
        $name = sprintf '%-11s', $name;

        my $entry = sprintf '%s', "    ENTRY($name  $scan  $exec  $mn_args),\n";

        if ( $name =~ s/^('[[:upper:]]',)/\L$1/msx )
        {
            $entry .= sprintf '%s',
              "    ENTRY($name  $scan  $exec  $mn_args),\n";
        }

        if ( $_->{name} =~ /^E.$/msx )
        {
            $e_cmds .= $entry;
        }
        elsif ( $_->{name} =~ /^F.$/msx )
        {
            $f_cmds .= $entry;
        }
        else
        {
            $cmds .= $entry;
        }
    }

    chomp $cmds;
    chomp $e_cmds;
    chomp $f_cmds;

    return ( $cmds, $e_cmds, $f_cmds );
}

#
#  Create macro line for each command defined in commands.h
#

sub make_exec
{
    my ($ref) = @_;
    my @commands = @{$ref};
    my %scan_funcs;
    my %exec_funcs;

    foreach (@commands)
    {
        my $scan = $_->{scan};
        my $exec = $_->{exec};

        if ( $scan ne 'NULL' )
        {
            $scan_funcs{$scan} = 1;     # We found a real scan function
        }

        if ( defined $exec )
        {
            $exec_funcs{$exec} = 1;     # We found a real exec function
        }
    }

    my $scan_list;
    my $exec_list;

    foreach my $key ( sort keys %scan_funcs )
    {
        $scan_list .= "extern bool $key(struct cmd *cmd);\n\n";
    }

    chomp $scan_list;

    foreach my $key ( sort keys %exec_funcs )
    {
        $exec_list .= "extern void $key(struct cmd *cmd);\n\n";
    }

    chomp $exec_list;

    return ( $scan_list, $exec_list );
}

#
#  Parse command-line options.
#

sub parse_options
{
    GetOptions( 'output=s' => \$args{output} );

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
    my ( $xml, $type ) = @_;
    my $dom = XML::LibXML->load_xml( string => $xml, line_numbers => 1 );

    ## no critic (ProhibitInterpolationOfLiterals)

    my $teco = $dom->findvalue(qq{/teco/\@name});

    ## use critic

    die "Missing name attribute\n"         if !defined $teco;
    die "Invalid name attribute '$teco'\n" if $teco ne 'teco';

    my @commands = ();

    foreach my $section ( $dom->findnodes('/teco/section') )
    {
        my $line  = $section->line_number();
        my $title = $section->getAttribute('title');

        if ( !defined $title )
        {
            die "Missing title at line $line\n";
        }

        foreach my $command ( $section->findnodes('./command') )
        {
            my $name    = $command->getAttribute('name');
            my $scan    = $command->getAttribute('scan');
            my $exec    = $command->getAttribute('exec');
            my $mn_args = 'NO_ARGS';

            if ( defined $exec && $exec =~ / (.+) ! /msx )
            {
                $mn_args = 'MN_ARGS';
                $exec =~ s/ (.+) ! /$1/msx;
            }

            if ( defined $scan )
            {
                $scan = "scan_$scan";
            }
            else
            {
                $scan = 'NULL';
            }

            if ( defined $exec )
            {
                $exec = "exec_$exec";
            }

            if ( $name =~ /^\[(.)\]/msx )
            {
                $name = $1;             # Convert [^] to ^
            }

            push @commands,
              {
                name    => $name,
                scan    => $scan,
                exec    => $exec,
                mn_args => $mn_args
              };
        }
    }

    return \@commands;
}

#
#  Create new version of commands.h.
#

sub write_commands
{
    my ( $template, $cmds, $e_cmds, $f_cmds ) = @_;
    my %changes = (
        'GENERAL COMMANDS' => $cmds,
        'E COMMANDS'       => $e_cmds,
        'F COMMANDS'       => $f_cmds,
    );

    teco_write( $template, $args{output}, %changes );

    return;
}

#
#  Create new version of exec.h.
#

sub write_exec
{
    my ( $template, $scan_list, $exec_list ) = @_;
    my %changes = (
        'SCAN FUNCTIONS' => $scan_list,
        'EXEC FUNCTIONS' => $exec_list
    );

    teco_write( $template, $args{output}, %changes );

    return;
}
