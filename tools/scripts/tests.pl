#!/usr/bin/perl

use strict;
use File::Basename;
use Data::Dumper;
$|=1;

my($sourceDir, $binaryDir, $cmd) = @ARGV;
my $ekTestExe = "$binaryDir/tools/ektest/ektest";
my $ekTestOutputDir = "$binaryDir/tests";
my @tests = sort glob("$sourceDir/tests/*.ek");

my $mode = 'test';
if($cmd eq 'dot')
{
    $mode = 'dot';
}
if($cmd eq 'ast')
{
    $mode = 'ast';
}

mkdir $ekTestOutputDir;
print("\n");

my $i = 1;
my $passed = 0;
my @failed;
for my $test (@tests)
{
    my ($testBasename) = fileparse($test, qr/\Q.ek\E/);
    my $ekTestExt = 'txt';
    if ($mode eq 'dot') {
        $ekTestExt = 'png';
    } elsif ($mode eq 'ast') {
        $ekTestExt = 'ast';
    }
    my $ekTestOutput = "$ekTestOutputDir/$testBasename.$ekTestExt";
    my $expected = 0;
    my $extraFlags = "";
    if($testBasename =~ /_O_/)
    {
        $extraFlags .= "-o ";
    }
    if($testBasename =~ /_E(\d+)$/)
    {
        $expected = $1;
    }

    if($mode eq 'test')
    {
        printf("                           \rRunning tests [%3d / %3d]: %s", $i++, scalar(@tests), $testBasename);
        my $cmd = "$ekTestExe -d $extraFlags $test 2>&1 > $ekTestOutput";
        my $ret = system($cmd);
        my $code = $ret >> 8;
        if($code != $expected)
        {
            print(" - FAILED\nEnd of $testBasename output:\n-------\n");
            system("tail $ekTestOutput");
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
    elsif($mode eq 'ast')
    {
        if($expected)
        {
            # If we're expecting some kind of error, don't bother dumping
            printf("                           \rSkipping AST dump [%3d / %3d]: %s", $i++, scalar(@tests), $testBasename);
        }
        else
        {
            printf("                      \rGenerating AST dump [%3d / %3d]: %s", $i++, scalar(@tests), $testBasename);
            my $cmd = "$ekTestExe -a $extraFlags $test > $ekTestOutput";
            my $ret = system($cmd);
            my $code = $ret >> 8;
        }
    }
    else
    {
        if($expected)
        {
            # If we're expecting some kind of error, don't bother graphing
            printf("                           \rSkipping dot graph [%3d / %3d]: %s", $i++, scalar(@tests), $testBasename);
        }
        else
        {
            printf("                      \rGenerating dot graph [%3d / %3d]: %s", $i++, scalar(@tests), $testBasename);
            my $cmd = "$ekTestExe -g $extraFlags $test | dot -Tpng > $ekTestOutput";
            my $ret = system($cmd);
            my $code = $ret >> 8;
        }
    }
}
if($mode eq 'test')
{
    printf("                \nTest results: [%3d / %3d] passed.\n", $passed, scalar(@tests));
}

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
