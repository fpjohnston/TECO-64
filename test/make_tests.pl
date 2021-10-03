#!/usr/bin/perl -w

#
#  make_tests.pl - Make scripts for testing TECO text editor.
#
#  @copyright 2021 Franklin P. Johnston / Nowwith Treble Software
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
use Cwd qw(getcwd abs_path);
use Getopt::Long;
use File::Basename;
use File::Find;
use File::Glob;
use File::Slurp;

#  Set up basic TECO command we will issue for each test script.

my %dirs;
my $verbose  = q{};
my $nscripts = 0;
my $auxdir = dirname(abs_path($0)) . '/aux';
my $outdir;
my $target = 'teco64';

#
#  Parse our command-line options
#

GetOptions(
    'output=s' => \$outdir,
    'target=s' => \$target,
    'verbose'  => \$verbose,
);

$target = lc $target;

if ($target eq 'vms' || $target eq 'teco-32')
{
    $target = 'teco32';
}
elsif ($target eq 'teco-64')
{
    $target = 'teco64';
}
elsif ($target ne 'teco32' && $target ne 'teco64' && $target ne 'tecoc')
{
    print "Invalid target: $target\n";

    exit;
}

#  [[FAIL]] - Unconditional failure.
#  [["L]]   - Failure if successful/less than zero.
#  [["E]]   - Failure if unsuccessful/zero.
#  [["N]]   - Failure if non-zero.
#  [[I]]    - Insert newline (LF or CR/LF).
#  [[^T]]   - Type newline (LF or CR/LF).

my %tokens =
(
   'FAIL'  => '!FAIL! [[^T]] ^C',
   '^T'    => '10^T',
   '"E'    => '"E [[FAIL]] \'',
   '"L'    => '"L [[FAIL]] \'',
   '"N'    => '"N [[FAIL]] \'',
   '"S'    => '"S [[FAIL]] \'',
   '"U'    => '"U [[FAIL]] \'',
   'I'     => '10@I//',
   'PASS'  => '!PASS! [[^T]]',
   'exit'  => '^D EK HK [[PASS]] EX',
   'enter' => '0,128ET HK',
   'in1'   => 'in_1.tmp',
   'in2'   => 'in_2.tmp',
   'out1'  => 'out_1.tmp',
   'out2'  => 'out_2.tmp',
   'cmd1'  => 'cmd_1.tmp',
   'cmd2'  => 'cmd_2.tmp',
   'log1'  => 'log_1.tmp',
   'bad'   => '/dev/teco',
);

my @filespecs = @ARGV;

# Use current directory as default if no arguments

if ( $#filespecs + 1 == 0 )
{
    @filespecs = q{.};
}

#
#  Main program start
#

# Search specified file specifications

foreach my $filespec (@filespecs)
{
    if ( -d $filespec )    # If directory, recurse through tree
    {
        open_dir($filespec);
    }
    else                   #  else assume specific file(s)
    {
        open_file($filespec);
    }
}

printf "Total of $nscripts test script%s created\n", $nscripts == 1 ? q{} : 's';

exit;

sub make_test
{
    my ($indir, $file, $key) = @_;
    my $text = read_file("$indir/$file");

    if (defined $key)
    {
        $file =~ s/[.]test/.key/;
    }
    else
    {
        $file =~ s/[.]test/.tec/;
    }

    my $outfile;

    if ($outdir)
    {
        $outfile = "$outdir/$file";
    }
    else
    {
        $outfile = "$indir/$file";
    }

    print "Creating test script: $outfile\n" if $verbose;

    open(FH, ">", $outfile) or croak "Can't open output file $outfile";

    while ($text =~ / (.*) \[ \[ (.+) \] \] (.*) /msx)
    {
        croak "Can't translate token: $2" unless exists $tokens{$2};

        $text = $1 . $tokens{$2} . $3;
    }

    print FH $text;
    print FH "\e\e";

    close(FH);

    ++$nscripts;
}

sub open_dir
{
    my ($filespec) = @_;

    find( { wanted => \&wanted }, $filespec );

    # Sort the directories, then sort the files in each one

    foreach my $indir ( sort { uc $a cmp uc $b } keys %dirs )
    {
        my $cwd = getcwd;
        my @infiles = @{ $dirs{$indir} };

        foreach my $infile ( sort { uc $a cmp uc $b } @infiles )
        {
            my ( $abstract, $redirect ) = read_header( "$indir/$infile" );

            if ( defined $abstract )
            {
                make_test($indir, $infile, $redirect);
            }
        }
    }

    return;
}

sub open_file
{
    my ($filespec) = @_;

    my @infiles = glob $filespec;

    foreach my $infile (@infiles)
    {
        my $indir = dirname($infile);

        $infile = basename($infile);

        my $cwd = getcwd;
        my ( $abstract, $redirect ) = read_header( "$indir/$infile" );

        if ( defined $abstract )
        {
            make_test($indir, $infile, $redirect);
        }
    }

    return;
}

sub read_header
{
    my ( $file ) = @_;

    open my $fh, '<', $file or croak "Can't open file: $file";

    chomp(my @lines = <$fh>);

    close $fh or croak "Can't close file $file";

    my $abstract;
    my $key;

    foreach my $line (@lines)
    {
        if ( $line =~ s/! \s+ TECO \s test: \s (.+?) \s ! /$1/x )
        {
            $abstract = $1;
        }
        elsif ( $line =~ /! \s Requires: \s (.+) \s ! /x )
        {
            my $requires = $1;

            if ($requires =~ /(.+), Redirect/)
            {
                $requires = $1;
                $key = 'yes';
            }
        }
        elsif ( $line =~ /Execution:/ )
        {
            undef $abstract;
        }
    }

    return ( $abstract, $key );
}

sub wanted
{
    my $infile = $_;

    # Only look for TECO script files

    if ( $infile =~ / [.] test $ /msx )
    {
        # Create a hash of arrays, with directory names as keys, and the
        # matching files as elements in the array used as the hash's value.

        push @{ $dirs{$File::Find::dir} }, $infile;
    }

    return;
}
