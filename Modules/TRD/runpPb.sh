configfile=$1
o2-ctf-reader-workflow --ctf-input DatapPb.txt --onlyDet TRD |
o2-qc -b --config json://"$configfile" &> output.log 



