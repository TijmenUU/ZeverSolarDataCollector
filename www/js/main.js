"use strict";

const MaxSolarPowerWatts = 3000;
const dataDir = 'solarpanel/';
const archiveFile = 'YYYY/MM_DD[.txt]';

async function CheckDateFile(momentDate) {
	const path = dataDir + momentDate.format(archiveFile);

	var request = new Request(path, { method: "HEAD" });
	const response = await fetch(request);

	if (response.ok) {
		return true;
	}
	return false;
}

async function GetDateFile(momentDate, successMethod, errorMethod) {
	const path = dataDir + momentDate.format(archiveFile);

	var request = new Request(path);
	const response = await fetch(request);

	//console.log("fetch " + path);// debug

	if (response.ok) {
		let text;
		try {
			text = await response.text();
			successMethod(text);
		}
		catch (error) {
			errorMethod(momentDate);
		}
	}
	else {
		errorMethod(momentDate);
	}
}

function ParseLine(linestr) {
	var linedata = {
		watt: 0,
		kilowatthour: 0.0,
		date: ""
	};

	if (linestr === undefined || linestr === null || linestr.length === 0) {
		return linedata;
	}

	var values = linestr.split(/\s+/);

	if (values === null || values.length === 0) {
		return linedata;
	}

	linedata.date = values[0];
	linedata.watt = parseInt(values[1]);
	linedata.kilowatthour = parseFloat(values[2]);

	return linedata;
}

function ParseData(datastr) {
	var statistics = {
		watt: [],
		kilowatthour: [],
		date: [],
		max_watt: 10,
		max_kilowatthour: 10
	};

	var lines = datastr.split('\n');
	for (var i = 0; i < lines.length; ++i) {
		var linedata = ParseLine(lines[i]);
		if (linedata.watt === 0 && linedata.kilowatthour === 0.0) {
			continue;
		}

		statistics.watt.push(linedata.watt);
		statistics.kilowatthour.push(linedata.kilowatthour);
		statistics.date.push(linedata.date);

		if (linedata.watt > statistics.max_watt) {
			statistics.max_watt = linedata.watt;
		}
		if (linedata.kilowatthour > statistics.max_kilowatthour) {
			statistics.max_kilowatthour = linedata.kilowatthour;
		}
	}

	return statistics;
}
