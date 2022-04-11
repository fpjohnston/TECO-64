#!/usr/bin/perl

#
#  version.pl - Build-process tool for TECO-64 text editor.
#
#  @copyright 2022 Franklin P. Johnston / Nowwith Treble Software
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
use Readonly;

Readonly my $WARNING =>
  '///  *** Automatically generated file. DO NOT MODIFY. ***';

# Command-line arguments

my %args = (
    update => undef,            # Update type (major, minor, patch)
    file   => undef,            # Version header file
);

#
#  Parse our command-line options
#

GetOptions(
    'release:s' => \$args{release},
);

die "Missing header file name\n" unless defined $ARGV[0];
die "Too many arguments\n" if $#ARGV > 0;

my $file = $ARGV[0];

#
#  Main program
#

my %version = ();

read_version($file);

update_version($args{release});

write_version($file);

exit 0;

#
#  Read current release version information from header file.
#

sub read_version
{
    my ($infile) = @_;

    open my $fh, '<', $infile or die "Cannot open input version file $infile\n";

    local $/ = undef;
    my $data = <$fh>;

    close $fh;

    my %header = ();

    if ($data =~ /\s+major_version\s+=\s+(\d+)/ms)
    {
        $version{major} = $1;
    }

    die "Cannot find major version in $infile\n" unless defined $version{major};

    if ($data =~ /\s+minor_version\s+=\s+(\d+)/ms)
    {
        $version{minor} = $1;
    }

    die "Cannot find minor version in $infile\n" unless defined $version{minor};

    if ($data =~ /\s+patch_version\s+=\s+(\d+)/ms)
    {
        $version{patch} = $1;
    }

    die "Cannot find patch version in $infile\n" unless defined $version{patch};

    return;
}

#
#  Update release version number.
#

sub update_version
{
    my ($release) = @_;

    if (!defined $release)
    {
        print 'Current TECO release version: '
            . "$version{major}.$version{minor}.$version{patch}\n";

        exit 1;
    }

    die "Required --release option is missing\n"
        if (!defined $release || length($release) == 0);

    if ($release =~ /major/ims)
    {
        ++$version{major};

        $version{minor} = 0;
        $version{patch} = 0;
    }
    elsif ($release =~ /minor/ims)
    {
        ++$version{minor};

        $version{patch} = 0;
    }
    elsif ($release =~ /patch/ims)
    {
        ++$version{patch};
    }
    else
    {
        print "release = $release\n";

        die "--release option must be one of: (major,minor,patch)\n";
    }

    return;
}

#
#  Write new release version information to header file.
#

sub write_version
{
    my ($outfile) = @_;
    my @data =
        (
         '///',
         '///  @' . 'file    version.h',
         '///  @' . 'brief   Release version numbers for TECO-64 text editor.',
         '///',
         "$WARNING",
         '///',
         '///  @' . 'copyright 2020-2022 Franklin P. Johnston / Nowwith Treble Software',
         '///',
         '///  Permission is hereby granted, free of charge, to any person obtaining a',
         '///  copy of this software and associated documentation files (the "Software"),',
         '///  to deal in the Software without restriction, including without limitation',
         '///  the rights to use, copy, modify, merge, publish, distribute, sublicense,',
         '///  and/or sell copies of the Software, and to permit persons to whom the',
         '///  Software is furnished to do so, subject to the following conditions:',
         '///',
         '///  The above copyright notice and this permission notice shall be included in',
         '///  all copies or substantial portions of the Software.',
         '///',
         '///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR',
         '///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,',
         '///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE',
         '///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIA-',
         '///  BILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,',
         '///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN',
         '///  THE SOFTWARE.',
         '///',
         '////////////////////////////////////////////////////////////////////////////////',
         q{},
         '#if     !defined(_VERSION_H)',
         q{},
         '#define _VERSION_H',
         q{},
         'enum release_version',
         '{',
         "    major_version = $version{major},",
         "    minor_version = $version{minor},",
         "    patch_version = $version{patch}",
         '};',
         q{},
         '#endif  // !defined(_VERSION_H)',
         q{},
        );

    open my $fh, '>', $outfile or die "Cannot open output version file $outfile\n";

    print {$fh} join "\n", @data;

    close $fh;

    print 'Updated TECO release version to '
        . "$version{major}.$version{minor}.$version{patch}\n";

    return;
}
