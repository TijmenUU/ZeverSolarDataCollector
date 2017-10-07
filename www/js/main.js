"use strict";

var MaxSolarPowerWatts = 3000;
var TodayData = '/rsc/dayresults.txt';

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
	var height = 600;

	var layout = {
		title: 'Overzicht' + moment().format(" D MMMM, YYYY"),
		titlefont:
		{
			//family: 'Courier New, monospace',
			size: 36,
			//color: '#7f7f7f'
		},
		width: width,
		height: height,
		xaxis:
		{
			fixedrange: true,
			//dtick: 20,
		},
		yaxis:
		{
			title: 'Momentopname (Watt)',
			range: [0, upperBoundPower],
			fixedrange: true,
			showgrid: true,
		},
		yaxis2:
		{
			title: 'Dagopbrengst Cummulatief (KWh)',
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
