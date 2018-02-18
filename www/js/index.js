"use strict";

/* Strings */
const chartTitle = 'Overzicht'; // Overview
const yaxis1Title = 'Momentopname'; // Production Snapshot
const yaxis2Title = 'Dagopbrengst Cummulatief'; // Cumulative Production
const tileNoDataMsg = 'Er is nog te weinig data verzameld.'; // Not enough data
const alertNoDataMsg = 'De zonnepanelen data kon niet worden opgehaald voor de datum '; // Data could not be fetched from:
const alertTryNextDayFailMsg = "De data voor de volgende dag kon niet worden gevonden op de server.";
const alertTryPreviousDayFailMsg = "De data voor de dag hiervoor kon niet worden gevonden op de server.";
const chartNoDataMsg = 'Er is niet genoeg data om een grafiek te laten zien.'; // Not enough data to draw a graph
const cvsHeader = 'tijd, momentopname in watt, cummulatieve opbrengst in kilowatt/uur'; // time, production snapshot, cumulative production in kilowatt/hour

window.onload = GetDateFile(moment(), DisplayData, DisplayError);

function TryLoadDate()
{
    var date = moment(document.getElementById('chartdate').value);
    GetDateFile(date, DisplayData, DisplayError);
}

function TryPreviousDay()
{
	var date = moment(document.getElementById('chartdate').value);
	GetDateFile(date.subtract(1, 'day'), DisplayData, DisplayError);
}

function TryNextDay()
{
	var date = moment(document.getElementById('chartdate').value);
    GetDateFile(date.add(1, 'day'), DisplayData, DisplayError);
}

function DisplayError(momentDate)
{
	alert(alertNoDataMsg + momentDate.format("DD-MM-YYYY"));
	document.getElementById("chart-col").innerHTML = chartNoDataMsg;
	DrawTiles(null);
    InitDownloads(null);
    UpdateDatePicker();
}

function DisplayData(data, momentDate)
{
    if(data === null || data === undefined || data.length === 0)
    {
        DisplayError(momentDate)
        return;
    }

    var statistics = ParseData(data);
	DrawChart(statistics);

	DrawTiles(statistics);

	InitDownloads(statistics);

	UpdateDatePicker(statistics.date[0]);
}

function UpdateChartWidth()
{
	var newWidth = parseInt($("#chart-col").css("width"));
	var existingPlot = document.getElementById('chart-col');
	if(existingPlot.layout.width !== newWidth)
	{
		Plotly.relayout('chart-col', { width: newWidth /*, legend: { x: GetLegendPosX(newWidth) }*/ });
	}
}

function DrawChart(stats)
{
	document.getElementById("chart-col").innerHTML = ""; // clear possible error msg
	var trace1 = {
		x: stats.date,
		y: stats.watt,
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
		x: stats.date,
		y: stats.kilowatthour,
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
		title: chartTitle + moment(stats.date[0]).format(" D MMMM, YYYY"),
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
			range: [0, stats.max_watt],
			fixedrange: true,
			showgrid: true,
		},
		yaxis2:
		{
			title: yaxis2Title + ' (KWh)',
			range: [0, stats.max_kilowatthour],
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

	var stats = [trace1, trace2];

	Plotly.newPlot('chart-col', stats, layout, {displayModeBar: false});

	window.onresize = UpdateChartWidth;
}

function DrawTiles(stats)
{
	if(stats === undefined || stats === null || stats.date.length < 2)
	{
		document.getElementById('kilowatt-stats').innerHTML = tileNoDataMsg;
		document.getElementById('watt-stats').innerHTML = tileNoDataMsg;
		document.getElementById('activity-stats').innerHTML = tileNoDataMsg;
		return;
	}

	var cummulativeWatts = 0;
	var wattLow = stats.max_watt;
	var wattHigh = 0;
	for(var i = 0; i < stats.watt.length; ++i)
	{
		cummulativeWatts += stats.watt[i];
		if(stats.watt[i] < wattLow)
		{
			wattLow = stats.watt[i];
		}
		else if(stats.watt[i] > wattHigh)
		{
			wattHigh = stats.watt[i];
		}
	}
	cummulativeWatts /= stats.watt.length;

	// Get some workable solution here for locale (string format)
	document.getElementById('kilowatt-stats').innerHTML = "Een totaal van " + stats.kilowatthour[stats.kilowatthour.length - 1] + " Kilowatt/uur opgewekt op deze dag.";

	document.getElementById('watt-stats').innerHTML = "Gemiddeld genomen " + Math.round(cummulativeWatts) + " watt met een laagste waarde van " + wattLow + " watt en een hoogste waarde van " + wattHigh +" watt.";

	document.getElementById('activity-stats').innerHTML = "Actief van " + moment(stats.date[0]).format("HH:mm") + " tot en met " + moment(stats.date[stats.date.length - 1]).format("HH:mm") + ".";
}

function InitDownloads(stats)
{
	if(stats === null || stats === undefined || stats.date.length < 1)
	{
		return;
	}

	const lineEnd = '\r\n';
	const csvSeperator = ',';
	var csvStr = cvsHeader + lineEnd;
	for(var i = 0; i < stats.date.length; ++i)
	{
		csvStr += stats.date[i] + csvSeperator + stats.watt[i] + csvSeperator + stats.kilowatthour[i] + lineEnd;
	}

	var downloadButton = document.getElementById('csvDownload');
	downloadButton.setAttribute('href', 'data:text/csv;charset=utf-8,' + encodeURIComponent(csvStr));
	downloadButton.setAttribute('download', moment(stats.date[0]).format("YYYY_MM_DD[.csv]"))
}

function UpdateDatePicker(date)
{
	if(date === undefined || date === null)
	{
		document.getElementById('chartdate').value = moment().format("YYYY-MM-DD");
	}
	document.getElementById('chartdate').value = moment(date).format("YYYY-MM-DD");
}