#!/usr/bin/perl

use strict;
use File::Basename;
use Data::Dumper;
$|=1;

my($sourceDir, $binaryDir) = @ARGV;
my $yapTestExe = "$binaryDir/demos/yaptest/yaptest";
my $yapTestOutputDir = "$binaryDir/tests";
my @tests = sort glob("$sourceDir/tests/*.yap");

mkdir $yapTestOutputDir;
print("\n");

my $i = 1;
my $passed = 0;
my @failed;
for my $test (@tests)
{
    my ($testBasename) = fileparse($test, qr/\Q.yap\E/);
    my $yapTestOutput = "$yapTestOutputDir/$testBasename.txt";
    my $expected = 0;
    if($testBasename =~ /_E(\d+)$/)
    {
        $expected = $1;
    }

    printf("                   \rRunning tests [%3d / %3d]: %s", $i++, scalar(@tests), $testBasename);
    my $cmd = "$yapTestExe $test 2>&1 > $yapTestOutput";
    my $ret = system($cmd);
    my $code = $ret >> 8;
    if($code != $expected)
    {
        print(" - FAILED\nEnd of $testBasename output:\n-------\n");
        system("tail $yapTestOutput");
        print("-------\n");
        push(@failed, {
            name => $testBasename,
            expected => $expected,
            code => $code,
        });
    }
    else
    {
        $passed++;
    }
}
printf("             \nTest results: [%3d / %3d] passed.\n", $passed, scalar(@tests));

if(scalar(@failed))
{
    print("Failed Tests:\n");
    for my $f (@failed)
    {
        if($f->{'expected'})
        {
            printf("* %s : expected [%d] (%s), got [%d] (%s)\n", $f->{'name'}, $f->{'expected'}, codeToReason($f->{'expected'}), $f->{'code'}, codeToReason($f->{'code'}));
        }
        else
        {
            printf("* %s : %s\n", $f->{'name'}, codeToReason($f->{'code'}));
        }
    }
}

sub codeToReason
{
    my($code) = @_;
    my @reasons;

    if($code & 1)
    {
        push(@reasons, 'Compile/Eval');
    }

    if($code & 2)
    {
        push(@reasons, 'Memory Leaks');
    }

    if($code & 4)
    {
        push(@reasons, 'Bad Refcounts');
    }

    unless(scalar(@reasons))
    {
        push(@reasons, "Unknown");
    }

    return join(', ', @reasons);
}
