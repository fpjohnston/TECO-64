#!/usr/bin/perl

#
#  make_version.pl - Build-process tool for TECO-64 text editor.
#
#  @copyright 2022-2023 Franklin P. Johnston / Nowwith Treble Software
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

use lib 'etc/';
use Teco qw(teco_read teco_write);

# Command-line arguments

#<<< perltidy: preserve format

my %args = (
    input   => undef,           # Input header file
    output  => undef,           # Output header file
    pattern => undef,           # Pattern file
    version => undef,           # Version type (major, minor, patch)
);

#>>>

my %version;

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

    my $header_data  = teco_read( $args{input} );
    my $pattern_data = teco_read( $args{pattern} );

    find_version($header_data);
    advance_version( $args{version} );

    my %changes = (
        'MAJOR VERSION' => "    major_version = $version{major},",
        'MINOR VERSION' => "    minor_version = $version{minor},",
        'PATCH VERSION' => "    patch_version = $version{patch}",
    );

    teco_write( $pattern_data, $args{output}, %changes );

    print "Updated TECO release version from $version{previous}"
      . " to $version{full}\n";

    return;
}

#
#  Update release version number.
#

sub advance_version
{
    my ($version) = @_;

    if ( !defined $version )            # Just print current version
    {
        print 'Current TECO release version: '
          . "$version{major}.$version{minor}.$version{patch}\n";

        exit 1;
    }
    elsif ( length $version == 0 )      # What user specified was bogus
    {
        die "No argument found for --version\n";
    }

    my $previous = $version{full};

    if ( $version =~ /major/ims )       # User specified major version,
    {
        ++$version{major};              #  so increment it,

        $version{minor} = 0;            #  and reset minor and patch versions
        $version{patch} = 0;
    }
    elsif ( $version =~ /minor/ims )    # User specified minor version,
    {
        ++$version{minor};              #  so increment it,

        $version{patch} = 0;            #  and reset patch version
    }
    elsif ( $version =~ /patch/ims )    # User specified patch version,
    {
        ++$version{patch};              #  so increment it
    }
    else                                # What user specified was bogus
    {
        die "Invalid argument '$version' for --version\n";
    }

    $version{previous} = $version{full};
    $version{full}     = "$version{major}.$version{minor}.$version{patch}";

    return;
}

#
#  Find current release version information in file we just read,
#

sub find_version
{
    my ($data) = @_;

    if ( $data =~ /\s+major_version\s+=\s+(\d+)/ms )
    {
        $version{major} = $1;
    }

    die "Cannot find major version in $args{input}\n"
      if !defined $version{major};

    if ( $data =~ /\s+minor_version\s+=\s+(\d+)/ms )
    {
        $version{minor} = $1;
    }

    die "Cannot find minor version in $args{input}\n"
      if !defined $version{minor};

    if ( $data =~ /\s+patch_version\s+=\s+(\d+)/ms )
    {
        $version{patch} = $1;
    }

    print "Cannot find patch version in $args{input}\n"
      if !defined $version{patch};

    $version{full} = "$version{major}.$version{minor}.$version{patch}";

    return;
}

#
#  Parse command-line options.
#

sub parse_options
{
    GetOptions(
        'output=s'  => \$args{output},
        'version:s' => \$args{version},
    );

    die "Not enough input files\n" if $#ARGV < 1;
    die "Too many input files\n"   if $#ARGV > 1;

    $args{input}   = $ARGV[0];
    $args{pattern} = $ARGV[1];

    return;
}
