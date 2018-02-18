"use strict";

const noDataMsg = 'Er is geen data om te laten zien voor de geselecteerde dagen.'; // No data available for the selected day

window.onload = OnLoad;

function OnLoad()
{
    UpdateDatePickers();
    //GetDateFile(moment(), SuccessMethod, ErrorMethod);
}

function UpdateDatePickers()
{
    var date = moment();
    document.getElementById('end-chartdate').value = date.format("YYYY-MM-DD");
    document.getElementById('start-chartdate').value = date.subtract(1, 'month').format("YYYY-MM-DD");
}

function TryLoad()
{
    const startDate = moment(document.getElementById('start-chartdate').value);
    const endDate = moment(document.getElementById('end-chartdate').value);
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
    if(stats === undefined || stats === null || stats.date.length < 2)
    {
        document.getElementById("chart-col").innerHTML = noDataMsg;
        return;
    }

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
		document.getElementById('kilowatt-stats').innerHTML = noDataMsg;
		document.getElementById('watt-stats').innerHTML     = noDataMsg;
		document.getElementById('activity-stats').innerHTML = noDataMsg;
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