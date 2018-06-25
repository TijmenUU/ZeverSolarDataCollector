#!/bin/bash
# Example collector bash script, which expects launch parameters:
# $1 = the collector executable (e.g. /usr/bin/collector)
# $2 = the root dir where the logged data should be stored (e.g. /home/user)
# $3 = the url to fetch data from (e.g http://192.168.2.4/home.cgi)

directoryPath="$2/$(date +%Y/)"
outputFile="$directoryPath$(date +%m_%d.txt)"
datestr="$(date +%Y-%m-%dT%T)"
errorlog="$2/collector.log"

# Make sure the directories & output file exist, then fetch data and redirect to outputfile
mkdir -p "$directoryPath" && \
touch "$outputFile" && \
$1 -u $3 -d $datestr 1>> $outputFile 2>> $errorlog
