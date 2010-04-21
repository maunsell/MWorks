#!/usr/bin/perl -w

use strict;
use warnings;
use File::Basename;
use Getopt::Std;
use Getopt::Long;

my %Options;
#my $ok = getopts('mibt', \%Options);
my $mail_address;
my $install_flag;
my $build_flag;
my $test_flag;

GetOptions("mailto=s" => \$mail_address,
	   "install|i" => \$install_flag, 
	   "build|b" => \$build_flag,
	   "test|t" => \$test_flag);

unless($#ARGV == 0) {
    print "Usage\n";
    print "\tMonkeyWorksRegression.pl [-i -b -t --mailto=<recipient>] MW_HOME\n";
    exit;
}


my $MW_HOME=$ARGV[0];
chdir $MW_HOME;
$MW_HOME=`pwd`;
$MW_HOME =~ s/\n//g;


push(@INC, $MW_HOME . "/MonkeyWorksTools");

if($build_flag) {
    require cleanMW;
    require buildMW;
}
if($test_flag) {
    require testMW;
}


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
my $filename = "MW" . $dateString . "-" . $$;
my $logfilename = "$filename.log";
my $filepath = "/tmp";
my $logfile = "$filepath/$logfilename";

my $buildOutputString = "";
my @buildResults = ();
if($build_flag) {
    cleanMW();
    ($buildOutputString, @buildResults) = buildMW($MW_HOME, 1);
}

my @testResults = ();
if($test_flag) {
    @testResults = testMW($MW_HOME);
}


my $output = "MonkeyWorks Regression Testing\n";
$output = $output . "Date: $year-$month-$day\n\n";
$output = $output . "-" x 80;
$output = $output . "\n";

$output = $output . "BUILD RESULTS:\n";

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

$output = $output . "-" x 80;
$output = $output . "\n";


$output = $output . "TEST RESULTS:\n";
for my $j (0 .. $#testResults) {
    my $name = $testResults[$j]{name};
    my $result = $testResults[$j]{result};
    my $test_output = $name . " " x (65-length($name)) . ":: ";

    unless($result eq "success") {
	$failure_flag++;
	$test_output = $test_output . "FAILED::";
    }
    $test_output = $test_output . $result;
    $output = $output . "$test_output\n";
}

my $username = `whoami`;
$username =~ s/\n//g;


#open LOG, ">$logfile" or die "can't create $logfile: $!";
#print LOG $mail_header . $output;
#close LOG;
#system "scp $logfile bkennedy\@dicarlo7.mit.edu:temp/MW$year-$month-$day.log";

my $installer_status;

if($output =~ /FAIL/) {
    $output = $output . "\nErrors, no installer built\n\n";
    $output = $output . "BUILD DUMP:\n" . $buildOutputString . "\n";
} else {
    if($install_flag) {
	my $installerURL = "http://dicarloserver2.mit.edu/MonkeyWorks/MonkeyWorks-NIGHTLY.mpkg.zip";
	$output = $output . "\nBleeding edge installer:\n\t$installerURL\n";    
	chdir "$MW_HOME/MonkeyWorksInstaller";
	system "./buildAllPackages.sh NIGHTLY";
	system "./uploadPackages.sh NIGHTLY";
    }
}

my $test_num = $#buildResults + $#testResults + 2;

if(length($mail_address) > 0) {
    my $mailfilename = "$filename.txt";
    my $mailfile = "$filepath/$mailfilename";

    my $mail_header = "To: $mail_address\n";
#my $mail_header = "To: bkennedy\@mit.edu\n";
    $mail_header = $mail_header . "From: $username\@mit.edu\n";
    $mail_header = $mail_header . "Subject: MW Regression Testing ($year-$month-$day): ";
    if($failure_flag == 0) {
	$mail_header = $mail_header . "$test_num tests PASSED\n";
    } else {
	$mail_header = $mail_header . "$failure_flag FAILURES out of $test_num tests\n";
    }

    open MAIL, ">$mailfile" or die "can't create $mailfile: $!";
    print MAIL $mail_header . $output;
    close MAIL;
    system "cat $mailfile | /usr/sbin/sendmail -t";
} else {
    print "$output\n";
}


