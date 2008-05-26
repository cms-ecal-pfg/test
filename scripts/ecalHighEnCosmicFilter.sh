#! /bin/bash

preferred_dir=`pwd`
log_dir=$preferred_dir/HighEnlog/
conf_dir=$preferred_dir/HighEnconf/
cmssw_dir=`pwd`

# in case you wanted to force  execution inside the preferred_dir
#if [ "$PWD" != $preferred_dir ]; then
#
# echo ""
# echo "this script should be executed from $preferred_dir"
# echo "please do:"
# echo "            cd $preferred_dir"
# echo ""
## exit
#
#fi

mkdir -p  $preferred_dir/HighEnlog/
mkdir -p  $preferred_dir/HighEnconf/


if [ ! -n "$1" ]

then
echo ""
echo "This script produces a root file containing the Ecal High Energy Cosmic events skimmed."
echo ""
echo "Options:"
echo ""
echo "      -p|--path_file        file_path       data file (from Castor) to be analyzed preceeded by path"
echo ""
echo ""
echo ""
exit

fi




data_path="/data/ecalod-22/daq-data/"
data_file="none"

cfg_path="$conf_dir"


first_event=1
last_event=9999



  while [ $# -gt 0 ]; do    # Finché ci sono parametri . . .
    case "$1" in

      -p|--path_file)
                data_path="$2"
                ;;

    esac
    shift

done

data_file=${data_path##*/} 
extension=${data_file##*.}

echo ""
echo ""
echo "data to be analyzed:                          $data_file"
echo ""
echo ""

if [[ $extension == "root" ]]; then
  input_module="
# if getting data from a .root pool file
  source = PoolSource {
    untracked vstring fileNames = { '$data_path' }
    untracked bool   debugFlag     = true
   }"
else
  input_module="
     source = NewEventStreamFileReader{
       untracked vstring fileNames = { '$data_path' }
       untracked uint32 debugVebosity = 10
       untracked bool   debugFlag     = true
     }" 
fi

cat > "$cfg_path$data_file".HighEnCosmic.$$.cfg <<EOF

process HighEnergyCosmicSkim ={

    $input_module

    #number of event to be processed
    untracked PSet maxEvents = {untracked int32 input = -1}
		

    # Messages
    include "FWCore/MessageLogger/data/MessageLogger.cfi"

    # skim
    include "CaloOnlineTools/EcalTools/data/ecalHighEnCosmicFilter.cfi"

 
    # output module
	module out = PoolOutputModule
	{
	 untracked PSet SelectEvents = 
		{
                 vstring SelectEvents = { "p" }
                }
	 untracked string filterName = "skimming"
         untracked vstring outputCommands = 
		{
		# keep all infos about the event, after the skim
                "keep *" 
		}
         untracked string fileName ="/tmp/HighEnEventSkim.root"
        }

    # paths
	path p = { skimming }
     	endpath e = { out }
 
 
 }

/////


EOF

echo "initializing cmssw..."
#. /nfshome0/cmssw/cmsset_default.sh
cd $cmssw_dir;
eval `scramv1 ru -sh`;
cd -;
echo "... running" 
cmsRun "$cfg_path$data_file".HighEnCosmic.$$.cfg >& "$log_dir$data_file".$$.HighEnCosmic.log

echo ""
echo ""
echo ""
echo "-------------------------------------------------------------------------------------------------------------------------"
echo "cosmic data skimmed, now run the Cosmic Analysis to see the results"
echo "-------------------------------------------------------------------------------------------------------------------------"
echo ""
echo ""
