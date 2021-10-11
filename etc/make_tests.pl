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
use File::Copy;
use File::Find;
use File::Glob;
use File::Slurp;

#  Set up basic TECO command we will issue for each test script.

my %dirs;
my $verbose  = q{};
my $clean;
my $nscripts = 0;
my $target = 'teco64';
my @tecfiles = ();
my $nskipped = 0;

#
#  Parse our command-line options
#

GetOptions(
    'clean!'   => \$clean,
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
elsif ($target ne 'teco32' && $target ne 'teco64'
       && $target ne 'tecoc' && $target ne 'teco')
{
    print "Invalid target: $target\n";

    exit;
}

my %tokens =
(
   'FAIL'  => '!FA IL! [[^T]] ^C',
   '^T'    => '10^T',
   '-8'    => '4096,0 ET',
   '"E'    => '"E [[FAIL]] \'',
   '"L'    => '"L [[FAIL]] \'',
   '"N'    => '"N [[FAIL]] \'',
   '"S'    => '"S [[FAIL]] \'',
   '"U'    => '"U [[FAIL]] \'',
   'I'     => '10@I//',
   'PASS'  => '!PA SS! [[^T]]',
   'exit'  => '^D EK HK [[PASS]] EX',
   'enter' => 'HK 0,128ET',
   'error' => '!FA IL! [[^T]]',
   'in1'   => 'in_1.tmp',
   'in2'   => 'in_2.tmp',
   'out1'  => 'out_1.tmp',
   'out2'  => 'out_2.tmp',
   'cmd1'  => 'cmd_1.tmp',
   'cmd2'  => 'cmd_2.tmp',
   'log1'  => 'log_1.tmp',
   'bad'   => '/dev/teco',
);

croak "Input and output arguments required" if ( $#ARGV != 1 );

my $input  = $ARGV[0];
my $outdir = $ARGV[1]; 

$input =~ s[/$][];
$outdir =~ s[/$][];

# If requested, delete any existing .tec files in output directory

if ($clean)
{
    my @files = glob "$outdir/*.tec";

    foreach my $file (@files)
    {
        unlink $file;
    }
}

#
#  Main program start
#

if ( -d $input )                        # If directory, recurse through tree
{
    open_dir($input);
}
else                                    #  else assume specific file(s)
{
    open_file($input);
}

my $total = $nscripts + $nskipped;

print "\n" if $verbose;
printf "%u file%s found in $input/, ", $total, $total == 1 ? q{} : 's';
printf "%u skipped, ", $nskipped;
printf "%u copied to $outdir/\n", $nscripts;

exit;

sub make_test
{
    my ($indir, $file) = @_;
    my $text = read_file("$indir/$file");

    $file =~ s/[.]test$/.tec/;

    my $outfile;

    if ($outdir)
    {
        $outfile = "$outdir/$file";
    }
    else
    {
        $outfile = "$indir/$file";
    }

    if ($target eq 'tecoc')
    {
        $outfile =~ s/\-/_/g;
    }

    print "Creating file: $outfile\n" if $verbose;

    open(FH, ">", $outfile) or croak "Can't open output file $outfile";

    while ($text =~ / (.*) \[ \[ (.+) \] \] (.*) /msx)
    {
        croak "Can't translate token: $2" unless exists $tokens{$2};

        my $before = $1;
        my $token = $2;
        my $middle = $tokens{$token};
        my $after = $3;

        if ($target eq 'tecoc')
        {
            if ($token eq '^T')
            {
                $middle = " 13^T $middle";
            }
            elsif ($token eq 'FF' || $token eq '-8')
            {
                $middle = q{};
            }
            elsif ($token eq 'I')
            {
                $middle = ' 13@I// ' . $middle;
            }
        }
        elsif ($target eq 'teco64' && $token eq 'enter')
        {
            $middle .= ' 0E1 1,0E3',
        }

        if ($token eq 'enter')
        {
            $middle .= "\e\e\n";
        }
        elsif ($token eq 'exit')
        {
            $middle = "\e\e\n$middle";
        }

        $text = $before . $middle . $after;
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

    if ($outdir)
    {
        while (defined (my $file = pop(@tecfiles)))
        {
            copy($file, $outdir) or croak "Failed to copy '$file' to '$outdir': $!";
        }
    }               

    # Sort the directories, then sort the files in each one

    foreach my $indir ( sort { uc $a cmp uc $b } keys %dirs )
    {
        my $cwd = getcwd;
        my @infiles = @{ $dirs{$indir} };

        foreach my $infile ( sort { uc $a cmp uc $b } @infiles )
        {
            my ( $abstract ) = read_header( "$indir/$infile" );

            if ( defined $abstract )
            {
                make_test($indir, $infile);
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
        my ( $abstract ) = read_header( "$indir/$infile" );

        if ( defined $abstract )
        {
            make_test($indir, $infile);
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

    foreach my $line (@lines)
    {
        if ( $line =~ s/! \s+ TECO \s test: \s (.+?) \s ! /$1/x )
        {
            $abstract = $1;
        }
        elsif ( $line =~ /! \s Requires: \s (.+) \s ! /x )
        {
            my $requires = $1;

            if ($requires eq 'TECO C')
            {
                if ($target ne 'tecoc' && $target ne 'teco64')
                {
                    ++$nskipped;

                    print "Skipping file: $file\n" if $verbose;

                    return ( undef, undef );
                }
            }
            elsif ($requires eq 'TECO-10')
            {
                if ($target ne 'teco64')
                {
                    ++$nskipped;

                    print "Skipping file: $file\n" if $verbose;

                    return ( undef, undef );
                }
            }
            elsif ($requires eq 'TECO-32')
            {
                if ($target ne 'teco32' && $target ne 'teco64')
                {
                    ++$nskipped;

                    print "Skipping file: $file\n" if $verbose;

                    return ( undef, undef );
                }
            }
            elsif ($requires eq 'TECO-64')
            {
                if ($target ne 'teco64')
                {
                    ++$nskipped;

                    print "Skipping file: $file\n" if $verbose;

                    return ( undef, undef );
                }
            }
        }
    }

    return ( $abstract );
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
    elsif ( $infile =~ / [.] tec $ /msx && $outdir)
    {
        $infile = getcwd . '/' .  $infile;

        push(@tecfiles, $infile);
    }

    return;
}
