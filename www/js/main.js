"use strict";

const MaxSolarPowerWatts = 3000;
const dataDir = '/solarpanel/';
const archiveFile = 'YYYY/MM_DD[.txt]';
const chartHeight = 600; // in px

async function GetFile(file, successMethod, errorMethod)
{
	const path = dataDir + file;

	var request = new Request(path);
	const response = await fetch(request);
	console.log("request method " + request.method + " url " + request.url);// debug

	if(response.ok)
	{
		response.text().then(function(text) { successMethod(text, path); });
	}
	else
	{
		errorMethod(path);
	}
}

function ParseData(datastr)
{
    var statistics = {
        watt: [],
        kilowatthour: [],
        date: [],
        max_watt: 10,
        max_kilowatthour: 10
    };
    
	var lines = datastr.split('\n');
	for(var i = 0; i < lines.length; ++i)
	{
		if(lines[i].length === 0)
		{
			continue;
		}
		var values = lines[i].split(/\s+/);
		if(values !== null)
		{
			var cummulativeValue = parseFloat(values[2]);
			var powerValue = parseInt(values[1]);
			if(cummulativeValue === 0 && powerValue === 0)
			{
				continue;
			}
			statistics.watt.push(powerValue);
			statistics.kilowatthour.push(cummulativeValue);
			statistics.date.push(values[0]);

			if(powerValue > statistics.max_watt)
			{
				statistics.max_watt = powerValue;
			}
			if(cummulativeValue > statistics.max_kilowatthour)
			{
				statistics.max_kilowatthour = cummulativeValue;
			}
		}
	}

	return statistics;
}
