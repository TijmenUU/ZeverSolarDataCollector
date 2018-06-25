#!/bin/bash
# Example decimalfixer bash script, which expects launch parameters:
# $1 = the decimalfixer executable (e.g. /usr/bin/decimalfixer)
# $2 = root directory of where the logs can be found

directoryPath="$2/$(date +%Y/)"
outputFile="$directoryPath$(date +%m_%d.txt)"
errorLog="$2/decimalfixer.log"

# It fixes the file that corresponds to now
$1 $outputFile 2>> $errorLog