#!/bin/bash 
#verificar argumentos----------
if [[ "$3" -lt 2 ]] || [[ "$4" -lt 1 ]] || [ $# -lt 4 ];
	then
    	echo "Invalid arguments"
    	exit 1
fi
inputFolder="$1"
outputFolder="$2"
maxNumThreads="$3"
numBuckets="$4"
mkdir "$outputFolder" 
for input in "$inputFolder"/*.txt 
	do
		inputVar="$input"
		inputVar="${inputVar/$inputFolder/}"
		inputVar="${inputVar/.txt/}"
		inputVar="${inputVar/\//}" 
		echo "InputFile=$inputVar NumThreads=1"
		fileNsync="$inputVar-1.txt"
	    ./tecnicofs-nosync $input "$outputFolder"/$fileNsync 1 1 | grep "TecnicoFS completed in"
	for i in $(seq 2 $maxNumThreads)
		do    
			fileSync="$inputVar-$i.txt"
			echo "InputFile=$inputVar NumThreads=$i"
			./tecnicofs-mutex $input "$outputFolder"/$fileSync $i $numBuckets | grep "TecnicoFS completed in"
	done
done


