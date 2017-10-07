"use strict";

const MaxSolarPowerWatts = 3000;
const TodayData = '/rsc/dayresults.txt';
const chartHeight = 600; // in px
/* Strings */
const chartTitle = 'Overzicht'; // Overview
const yaxis1Title = 'Momentopname'; // Production Snapshot
const yaxis2Title = 'Dagopbrengst Cummulatief'; // Cumulative Production
const tileNoDataMsg = 'Er is nog te weinig data verzameld.'; // Not enough data
const alertNoDataMsg = 'De zonnepanelen data kon niet worden opgehaald, is alles wel goed met de server?'; // Data could not be fetched, is the server OK?
const chartNoDataMsg = 'Er is niet genoeg data om een grafiek te laten zien.'; // Not enough data to draw a graph
const cvsHeader = 'tijd, momentopname in watt, cummulatieve opbrengst in kilowatt/uur'; // time, production snapshot, cumulative production in kilowatt/hour

window.onload = LoadFile();

function LoadFile()
{
	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function() {
		if (xhr.readyState == 4) {
			// The request is done; did it work?
			if (xhr.status == 200) {
				// ***Yes, use `xhr.responseText` here***
				ParseData(xhr.responseText);
			}
			else {
				// ***No, tell the callback the call failed***
				DisplayError();
			}
		}
	};

	xhr.open("GET", TodayData, true);
	xhr.send();
}

function DisplayError()
{
	alert(alertNoDataMsg);
	document.getElementById("chart-col").innerHTML = chartNoDataMsg;
	DrawTiles(null, null, [], null);
	InitDownloads([], [], []);
}

function ParseData(data)
{
	var powerValues = [];
	var cummulativeValues = [];
	var dates = [];
	var upperBoundPower = 10;
	var upperBoundCummulative = 10;

	var lines = data.split('\n');
	for(var i = 0; i < lines.length; ++i)
	{
		if(lines[i].length == 0)
		{
			continue;
		}
		var values = lines[i].split(/\s+/);
		if(values != null)
		{
			var cummulativeValue = parseFloat(values[2]);
			var powerValue = parseInt(values[1]);
			if(cummulativeValue == 0 && powerValue == 0)
			{
				continue;
			}
			powerValues.push(powerValue);
			cummulativeValues.push(cummulativeValue);
			dates.push(values[0]);

			if(powerValue > upperBoundPower)
			{
				upperBoundPower = powerValue;
			}
			if(cummulativeValue > upperBoundCummulative)
			{
				upperBoundCummulative = cummulativeValue;
			}
		}
	}

	upperBoundPower += 10;
	upperBoundCummulative += 1;

	DrawChart(dates, powerValues, cummulativeValues,
		upperBoundPower, upperBoundCummulative);

	DrawTiles(dates[0], dates[dates.length - 1],
		powerValues, cummulativeValues[cummulativeValues.length - 1]);

	InitDownloads(dates, powerValues, cummulativeValues);
}

function UpdateChartWidth()
{
	var newWidth = parseInt($("#chart-col").css("width"));
	var existingPlot = document.getElementById('chart');
	if(existingPlot.layout.width != newWidth)
	{
		Plotly.relayout('chart', { width: newWidth /*, legend: { x: GetLegendPosX(newWidth) }*/ });
	}
}

function DrawChart(dateLabels,
	powerProduction,
	cummulativeProduction,
	upperBoundPower,
	upperBoundCummulative)
{
	var trace1 = {
		x: dateLabels,
		y: powerProduction,
		name: 'Watt',
		line:
		{
			shape: 'linear',
			color: '#2ECC71',
		},
		type: 'scatter',
		connectgaps: true
	};

	var trace2 = {
		x: dateLabels,
		y: cummulativeProduction,
		name: 'KWh',
		line:
		{
			shape: 'spline',
			color: '#F39C12',
		},
		type: 'scatter',
		connectgaps: true,
		yaxis: 'y2',
	};

	var width = parseInt($("#chart-col").css("width"));

	var layout = {
		title: chartTitle + moment(dateLabels[0]).format(" D MMMM, YYYY"),
		titlefont:
		{
			//family: 'Courier New, monospace',
			size: 36,
			//color: '#7f7f7f'
		},
		width: width,
		height: chartHeight,
		xaxis:
		{
			fixedrange: true,
			//dtick: 20,
		},
		yaxis:
		{
			title: yaxis1Title + ' (Watt)',
			range: [0, upperBoundPower],
			fixedrange: true,
			showgrid: true,
		},
		yaxis2:
		{
			title: yaxis2Title + ' (KWh)',
			range: [0, upperBoundCummulative],
			fixedrange: true,
			overlaying: 'y',
			side: 'right',
			showgrid: false,
		},
		legend:
		{
			orientation: "v",
			x: 1,
			xanchor: 'left',
			y: 1,
		},
	};

	var data = [trace1, trace2];

	Plotly.newPlot('chart', data, layout, {displayModeBar: false});

	window.onresize = UpdateChartWidth;
}

function DrawTiles(startDate,
	endDate,
	powerProduction,
	lastCumProduction)
{
	if(powerProduction.length < 2)
	{
		document.getElementById('powerProductionStats').innerHTML = tileNoDataMsg;
		document.getElementById('powerProduced').innerHTML = tileNoDataMsg;
		document.getElementById('activityStats').innerHTML = tileNoDataMsg;
		return;
	}

	var cumPower = 0;
	var powerLow = MaxSolarPowerWatts;
	var powerHigh = 0;
	for(var i = 0; i < powerProduction.length; ++i)
	{
		cumPower += powerProduction[i];
		if(powerProduction[i] < powerLow)
		{
			powerLow = powerProduction[i];
		}
		else if(powerProduction[i] > powerHigh)
		{
			powerHigh = powerProduction[i];
		}
	}
	cumPower /= powerProduction.length;

	// Get some workable solution here for locale (string format)
	document.getElementById('powerProductionStats').innerHTML = "Momenteel al " + lastCumProduction + " Kilowatt/uur opgewekt.";

	document.getElementById('powerProduced').innerHTML = "Gemiddeld genomen " + Math.round(cumPower) + " watt met een laagtepunt van " + powerLow + " watt en een piek van " + powerHigh +" watt.";

	document.getElementById('activityStats').innerHTML = "Actief sinds " + moment(startDate).format("HH:mm") + " tot en met " + moment(endDate).format("HH:mm") + ".";
}

function InitDownloads(dates, powerValues, cummulativeValues)
{
	if(dates.length < 1)
	{
		return;
	}

	const lineEnd = '\r\n';
	const csvSeperator = ',';
	var csvStr = cvsHeader + lineEnd;
	for(var i = 0; i < dates.length; ++i)
	{
		csvStr += dates[i] + csvSeperator + powerValues[i] + csvSeperator + cummulativeValues[i] + lineEnd;
	}

	var downloadButton = document.getElementById('csvDownload');
	downloadButton.setAttribute('href', 'data:text/csv;charset=utf-8,' + encodeURIComponent(csvStr));
	downloadButton.setAttribute('download', moment(dates[0]).format("YYYY_MM_DD[.csv]"))
}
