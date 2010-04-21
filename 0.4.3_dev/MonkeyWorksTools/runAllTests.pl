#!/usr/bin/perl -w

use strict;
use warnings;
use File::Basename;

push(@INC, "./MonkeyWorksTools");

require testMW;


my @testResults = testMW();
my $failure_flag=0;

print "MonkeyWorks Testing\n";
print "-" x 80;
print "\n";

print "TEST RESULTS:\n";
for my $j (0 .. $#testResults) {
    my $name = $testResults[$j]{name};
    my $result = $testResults[$j]{result};
    my $test_output = $name . " " x (65-length($name)) . ":: ";

    unless($result eq "success") {
	$failure_flag++;
	$test_output = $test_output . "FAILED::";
    }
    $test_output = $test_output . $result;
    print "$test_output\n";
}

