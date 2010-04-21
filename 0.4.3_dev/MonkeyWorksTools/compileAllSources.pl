#!/usr/bin/perl -w

use strict;
use warnings;
use File::Basename;
use Getopt::Std;
require Cwd;

my %Options;
my $ok = getopts('n', \%Options);

unless($#ARGV == 1) {
    print "Usage\n";
    print "\tcompileAllSources.pl [-n] MW_HOME COXLAB_MW_HOME\n";
    exit;
}

my $orig_dir = Cwd::cwd();

my $MW_HOME=$ARGV[0];
chdir $MW_HOME;
$MW_HOME=Cwd::cwd();

chdir $orig_dir;
my $CL_HOME=$ARGV[1];
chdir $CL_HOME;
$CL_HOME=Cwd::cwd();

chdir $MW_HOME;

push(@INC, "$MW_HOME/MonkeyWorksTools");
require cleanMW;
require buildMW;

my (undef, undef, undef, $day, $month, $year, undef, undef, undef) = localtime;
$year = $year + 1900;
$month = $month + 1;
if(length($month) < 2) {
    $month = "0" . $month;
}
if(length($day) < 2) {
    $day = "0" . $day;
}


my $dateString = $year . $month . $day;

cleanMW();

my @buildResults;
my @clBuildResults;
my @dlBuildResults;
my $clBuildOutput;
my $dlBuildOutput;

if($Options{'n'}) {
    ($dlBuildOutput, @dlBuildResults) = buildMW($MW_HOME, 0, $MW_HOME . "/MonkeyWorksTools/ProjectsToBuild.xml");
    ($clBuildOutput, @clBuildResults) = buildMW($CL_HOME, 0, $MW_HOME . "/MonkeyWorksTools/CoxlabProjectsToBuild.xml");
} else {
    ($dlBuildOutput, @dlBuildResults) = buildMW($MW_HOME, 1, $MW_HOME . "/MonkeyWorksTools/ProjectsToBuild.xml");
    ($clBuildOutput, @clBuildResults) = buildMW($CL_HOME, 1, $MW_HOME . "/MonkeyWorksTools/CoxlabProjectsToBuild.xml");
}

@buildResults = (@dlBuildResults, @clBuildResults);

my $output = "Build Results:\n";
$output = $output . "Date: $year-$month-$day\n\n";
$output = $output . "-" x 80;
$output = $output . "\n";

my $failure_flag = 0;

for my $i (0 .. $#buildResults) {
    my $name = $buildResults[$i]{name} . " " . $buildResults[$i]{type};    
    my $test_output = $name . " " x (65-length($name)) . ":: ";

    if($buildResults[$i]{result} == 0) {
	$test_output = $test_output . "success";
    } else {
	$failure_flag++;
	$test_output = $test_output . "FAILURE";
    }
    $output = $output . "$test_output\n";    
}

print $output . "\n";
