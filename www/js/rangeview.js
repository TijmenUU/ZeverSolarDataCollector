"use strict";

const chartHeight = 600; // in px

/* Strings */
const chartTitle = "Overzicht periode";
const noDataMsg = 'Er is geen data om te laten zien voor de geselecteerde dagen.'; // No data available for the selected day

window.addEventListener('load', OnLoad);
window.addEventListener('resize', UpdateChartWidth);

function OnLoad() {
	InitializeDatepickers();
	TryLoad();
}

function InitializeDatepickers() {
	let date = moment();
	document.getElementById('end-chartdate').value = date.format("YYYY-MM-DD");
	document.getElementById('start-chartdate').value = date.subtract(2, 'week').format("YYYY-MM-DD");
}

async function LoadGraph() {
	const startDate = moment(document.getElementById('start-chartdate').value);
	const endDate = moment(document.getElementById('end-chartdate').value);

	let statistics = {
		kilowatthour: [],
		date: [],
		max_kilowatthour: 0.0,
		max_kilowatthour_date: "",
		average_kilowatthour: 0.0
	};

	let date = startDate;
	while (date <= endDate) {
		let data = "";
		function setdata(datastr) {
			data = datastr;
		};

		await GetDateFile(date, setdata, function (date) { return; });

		if (data.length > 0) {
			let lines = data.split('\n');
			let i = lines.length - 1;
			while (lines[i].length < 1 && i >= 0) {
				--i;
			}
			if (i >= 0) {
				let linedata = ParseLine(lines[i]);

				statistics.kilowatthour.push(linedata.kilowatthour);
				statistics.date.push(linedata.date);
				statistics.average_kilowatthour += linedata.kilowatthour;

				if (statistics.max_kilowatthour < linedata.kilowatthour) {
					statistics.max_kilowatthour = linedata.kilowatthour;
					statistics.max_kilowatthour_date = linedata.date;
				}
			}
		}

		date.add(1, 'day');
	}

	if (statistics.kilowatthour.length > 0) {
		statistics.average_kilowatthour /= statistics.kilowatthour.length;
		statistics.average_kilowatthour = Math.round(statistics.average_kilowatthour * 10) / 10;
		DisplayData(statistics);
	}
	else {
		DisplayError();
	}
}

function TryLoad() {
	StartLoad();
	LoadGraph();
}

function DisplayError() {
	document.getElementById("chart-col").innerHTML = noDataMsg;
	document.getElementById("kilowatt-stats").innerHTML = noDataMsg;
	//document.getElementById("watt-stats").innerHTML = noDataMsg;

	EndLoad();
}

function DisplayData(statistics) {
	DrawChart(statistics);
	UpdateTiles(statistics);

	EndLoad();
}

function UpdateTiles(stats) {
	document.getElementById('kilowatt-stats').innerHTML = "Hoogste dagopbrengst gemeten was op " + moment(stats.max_kilowatthour_date).format("DD-MM-YYYY") + " met een waarde van " + stats.max_kilowatthour + " kilowatt/uur. De gemiddelde opbrengst over deze periode is " + stats.average_kilowatthour.toFixed(1) + " kilowatt/uur.";

	document.getElementById('watt-stats').innerHTML = "Hier is nog niks te zien."; // debug
}

function UpdateChartWidth() {
	let newWidth = parseInt($("#chart-col").css("width"));
	let existingPlot = document.getElementById('chart-col');
	if (existingPlot.layout.width !== newWidth) {
		Plotly.relayout('chart-col', { width: newWidth /*, legend: { x: GetLegendPosX(newWidth) }*/ });
	}
}

function DrawChart(stats) {
	document.getElementById("chart-col").innerHTML = ""; // clear possible error msg

	let dates = [];
	for (let i = 0; i < stats.date.length; ++i) {
		dates.push(moment(stats.date[i]).format("DD-MM-YYYY"));
	}

	let trace1 = {
		x: dates,
		y: stats.kilowatthour,
		name: 'Dagopbrengst',
		type: 'bar',
		marker: {
			color: '#FB8C00'
		}
	};

	let average_trace = [];
	for (let i = 0; i < dates.length; ++i) {
		average_trace.push(stats.average_kilowatthour);
	}

	let trace2 = {
		x: dates,
		y: average_trace,
		name: 'Gemiddelde',
		line:
			{
				shape: 'spline',
				color: '#43A047',
			},
		type: 'scatter',
		connectgaps: true
	};

	let width = parseInt($("#chart-col").css("width"));

	let layout = {
		title: chartTitle + moment(stats.date[0]).format(" DD-MM-YYYY") + " tot en met" + moment(stats.date[stats.date.length - 1]).format(" DD-MM-YYYY"),
		titlefont:
			{
				//family: 'Courier New, monospace',
				size: 25,
				//color: '#7f7f7f'
			},
		width: width,
		height: chartHeight,
		xaxis:
			{
				fixedrange: true,
				showticklabels: true,
				tickangle: -90,
				autotick: true
				//dtick: 20
			},
		yaxis:
			{
				title: 'Dagopbrengst (KWh)',
				range: [0, Math.ceil(stats.max_kilowatthour + 1.0)],
				fixedrange: true,
				showgrid: true,
			},
		legend:
			{
				orientation: "v",
				x: 1,
				xanchor: 'left',
				y: 1,
			},
	};

	Plotly.newPlot('chart-col', [trace1, trace2], layout, { displayModeBar: false });
}

function DrawTiles(stats) {
	if (stats === undefined || stats === null || stats.date.length < 1) {
		document.getElementById('kilowatt-stats').innerHTML = noDataMsg;
		document.getElementById('watt-stats').innerHTML = noDataMsg;
		document.getElementById('activity-stats').innerHTML = noDataMsg;
		return;
	}

	let cummulativeWatts = 0;
	let wattLow = stats.max_watt;
	let wattHigh = 0;
	for (let i = 0; i < stats.watt.length; ++i) {
		cummulativeWatts += stats.watt[i];
		if (stats.watt[i] < wattLow) {
			wattLow = stats.watt[i];
		}
		else if (stats.watt[i] > wattHigh) {
			wattHigh = stats.watt[i];
		}
	}
	cummulativeWatts /= stats.watt.length;

	// Get some workable solution here for locale (string format)
	document.getElementById('kilowatt-stats').innerHTML = "";
	document.getElementById('watt-stats').innerHTML = "";
	document.getElementById('activity-stats').innerHTML = "";
}