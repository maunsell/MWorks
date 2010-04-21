use warnings;
use strict;
use Data::Dumper;
require XML::Simple;

sub testMW {
    my @testResults;

    # Hack to get the system to run from the command line:
    chdir "/";

    my $result_string = `/Library/MonkeyWorks/tests/MonkeyWorksCoreTestRunner`;

    @testResults = parseResultString($result_string, "MonkeyWorksCoreTestRunner", @testResults);

    # now run the staged tests
    chdir "/";

    my @tests = `find /Library/MonkeyWorks/tests/StagedTests -type f -perm +400`;
    
    foreach (@tests) {
	$result_string = `$_`;
	@testResults = parseResultString($result_string, $_, @testResults);
    }
    

    return @testResults;
}
1;


sub parseResultString {

    my ($result_string, $testName, @testResults) = @_;

    print "$result_string\n";

    $result_string =~ s/\n//g; # get rid of the newlines
    
    if($result_string =~ m/.*(<TestRun>.*<\/TestRun>)\s*/) {
	my $xmlstring = $1;

	my $xml = new XML::Simple;
	my $data = $xml->XMLin($xmlstring, keyattr => [], forcearray => ['Test', 'FailedTest']);

	foreach my $successful_test (@{$data->{SuccessfulTests}->{Test}}) {
	    push @testResults, {name => $successful_test->{Name}, result => "success"};
	}

	foreach my $failed_test (@{$data->{FailedTests}->{FailedTest}}) {
	    my $result = "$failed_test->{FailureType}::$failed_test->{Message}";
	    push @testResults, {name => $failed_test->{Name}, result => $result};
	}
    }
    return @testResults;
}	

