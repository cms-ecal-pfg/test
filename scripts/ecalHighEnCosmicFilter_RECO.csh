#!/bin/tcsh

set conffile = $1 
set logfile  = $2
set workdir  = $3
set outfile  = $4
set destdir  = $5

cd $workdir
eval `scramv1 runtime -csh`

pwd
echo  "ls -l /tmp/" `ls -l /tmp/`
ls -l /tmp/
hostname

limit coredumpsize 0

#here to keep  log files
echo "Running cmsRun  $conffile >& $logfile" 
cmsRun $conffile >& $logfile

setenv logfilebase `basename $logfile`
setenv logfiledir  `dirname $logfile`

setenv protocol `echo $destdir | cut -d '/' -f2 `
setenv destpath `echo $destdir | cut -d ':' -f2 `

echo "copying $outfile to $destdir"
echo "printing destpath $destpath protocol $protocol"

if ($protocol == 'castor') then
   rfcp $outfile $destpath
   rfcp $logfile $destpath
else
   echo "ls outfile destdir"     `$outfile $destdir`
   scp $outfile $destdir 
   scp $logfile $destdir 
endif
