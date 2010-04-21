require XML::Simple;
use Data::Dumper;

sub buildMW {

    my ($MW_HOME, $clean, $file) = @_;
    our $outputString = "";
    my $buildOutput = "";
    my @buildResults;
    my $name;
    my $type;
    my $result;


    # create object
    my $xml = new XML::Simple;

    # read XML file
    my $data = $xml->XMLin($file);

    my $projects = $data->{project};

    foreach $cur_proj (@$projects) {
	if($clean || !($cur_proj->{type} eq "clean")) {
	    print "$cur_proj->{type} $cur_proj->{proj_name}\n";
	    my $result = build($cur_proj->{proj_name}, $MW_HOME . "/" . $cur_proj->{dir}, $cur_proj->{cmd});
	    push @buildResults, {name => $cur_proj->{proj_name}, type => $cur_proj->{type}, result => $result };
	}
    }

    return ($outputString, @buildResults);
}



sub build {
    my ($name, $dir, $build_cmd) = @_;

    chdir $dir;
    my $buildOutput = `$build_cmd`;
    my $result = $? >> 8;
    if($result != 0) {
	$outputString = "$outputString\n********************************\n$name ($build_cmd) FAILED:\n$buildOutput\n";
    } else {
	print "SUCCEEDED\n";
    }
    return $result;    
}


1;

