#!/usr/bin/perl

#directory where 
my $RunDir = "/afs/cern.ch/user/d/doglioni/massiveLauncher/206/";
	my $SrcDir = "/afs/cern.ch/user/d/doglioni/Tesi/CMSSW/CMSSW_2_0_6/src/";
	my $OutputDir = "/afs/cern.ch/user/d/doglioni/scratch0/206/";
	#in this directory: _plain_ file lists called FileListRUNNUMBER.dat
	my $FileDir = $RunDir."fileLists/root/";
	my $castorDir = "/castor/cern.ch/user/d/doglioni/";

	my $numArgs = $#ARGV + 1;

	#todo: check for other weirdness other than 5-digit ints
	if ( $numArgs == 0 ) {
		print "usage: perl massiveLauncher.pl RUNNUMBER RUNNUMBER ...\n";
	}

	foreach $argnum (0 .. $#ARGV) {

		### WRITING CFG FILE

		my $runNumber = $ARGV[$argnum];
		my $fileListName = $FileDir."FileList".$runNumber.".dat";

	my $cfgFile = open(CFG,">ecalMipHists".$runNumber.".cfg") || die "cannot open cfg file for writing\n";
	my $fileListFile = open(FILELISTFILE,$fileListName) || die "cannot open $fileListName";
	print CFG "process ECALMIPHISTS = { \n\n";
		print CFG "\tinclude 'FWCore/MessageService/data/MessageLogger.cfi'\n";
	        print CFG "\treplace MessageLogger.cerr.FwkSummary.reportEvery = 100 \n\n"; 

	        print CFG "\tinclude 'EventFilter/EcalRawToDigiDev/data/EcalUnpackerMapping.cfi'\n";
	        print CFG "\tinclude 'EventFilter/EcalRawToDigiDev/data/EcalUnpackerData.cfi'\n\n";

		print CFG "\tinclude 'Geometry/CaloEventSetup/data/CaloTopology.cfi'\n";
		print CFG "\tinclude 'Geometry/EcalCommonData/data/EcalOnly.cfi'\n";
		print CFG "\tinclude 'Geometry/CaloEventSetup/data/CaloGeometry.cff'\n\n";

	        print CFG "\tinclude 'EventFilter/EcalRawToDigiDev/data/EcalUnpackerData.cfi'\n\n";

	        print CFG "\tuntracked PSet maxEvents = {untracked int32 input = -1}\n";

		
		#uncomment this line for .dat files (careful with version! If you get a LCGReflex error, convert them into .root)
	        #print CFG "\tsource = NewEventStreamFileReader { \n";

		print CFG "\tsource = PoolSource {\n";
		print CFG "\t\tuntracked vstring fileNames = {\n";

		my $beginFile = 1; 
		while (<FILELISTFILE>) {

			        if ($beginFile==1) {
					print CFG "\n\t\t\t'".trim($_)."'";
					$beginFile=0;
				}
				else {
					print CFG ",\n\t\t\t'".trim($_)."'";
				}	

		}

		print CFG "\n\t\t} \n";#end fileNames

		print CFG "\t} \n";#end PoolSource 
		

		print CFG "\tinclude 'CaloOnlineTools/EcalTools/data/ecalMipHists.cfi'\n\n";

		print CFG "\tmodule ecalUncalibHit = ecalMaxSampleUncalibRecHit from 'RecoLocalCalo/EcalRecProducers/data/ecalMaxSampleUncalibRecHit.cfi'\n";
		print CFG "\treplace ecalUncalibHit.EBdigiCollection = ecalEBunpacker:ebDigis\n";
		print CFG "\treplace ecalUncalibHit.EEdigiCollection = ecalEBunpacker:eeDigis\n\n";

        	print CFG "\tservice = TFileService {\n";

		                print CFG "\t\tstring fileName = '".$OutputDir."histo".$runNumber.".root'\n";

        	print CFG "\t} \n\n";
		
		print CFG "\tpath p = {ecalEBunpacker, ecalUncalibHit, ecalMipHists}\n";

        print CFG "} \n";
	
	close(CFG);

	### WRITING LAUNCHER

	my $launcherFile = open(LAUNCHER,">launcher".$runNumber.".sh") || die "cannot open file";

	print LAUNCHER "#!/bin/tcsh\n";

	print LAUNCHER "echo WORKDIR\n";
	print LAUNCHER "echo \$WORKDIR\n";

	print LAUNCHER "echo ls before\n";
	print LAUNCHER "ls\n"; 

	print LAUNCHER "setenv CMSSWDIR ".$SrcDir."\n";
	print LAUNCHER "cd \$CMSSWDIR\n";
	print LAUNCHER "eval `scramv1 runtime -csh`\n";

	print LAUNCHER "cd \$WORKDIR\n";

	print LAUNCHER "cmsRun ".$RunDir."ecalMipHists".$runNumber.".cfg\n";
	
	print LAUNCHER "cp tree*.root ".$OutputDir."tree".$runNumber.".root\n";
	
	print LAUNCHER "rfcp ".$OutputDir."histo".$runNumber.".root ".$castorDir."\n";	
	print LAUNCHER "rm ".$OutputDir."histo".$runNumber.".root\n";

	print LAUNCHER "echo ls after\n";
	print LAUNCHER "ls\n"; 

	close(CFG);

	#making launcher executable
	system("chmod 755 launcher".$runNumber.".sh"); 
	system("export STAGE_SVCCLASS cmscaf");
	### MASSIVE SUBMISSION
	print "submitting job on CAF\n";
	system("bsub -q cmscaf launcher".$runNumber.".sh");
	print "tree$runNumber.root will be in directory $OutputDir\nhisto$runNumber.root will be in castor directory $castorDir\n";
}

sub trim
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}
