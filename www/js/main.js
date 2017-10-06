window.onload = LoadFile();

var MaxSolarPowerWatts = 3000;

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

	xhr.open("GET", "/rsc/dayresults.txt", true);
	xhr.send();
}

function DisplayError()
{

}

var upperBoundPower = 10;
var upperBoundCummulative = 10;
var powerValues = [];
var cummulativeValues = [];
var dates = [];

function ParseData(data)
{
	var lines = data.split('\n');
	for(i = 0; i < lines.length; ++i)
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

	DrawChart(dates, powerValues, cummulativeValues);
}

function DrawChart(dateLabels, powerProduction, cummulativeProduction)
{
	var trace1 = {
		x: dateLabels,
		y: powerProduction,
		name: 'Watt',
		line:
		{
			shape: 'linear',
			color: '#F4D03F',
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
			color: '#58D68D',
		},
		type: 'scatter',
		connectgaps: true,
		yaxis: 'y2',
	};

	var data = [trace1, trace2];

	var layout = {
		title: 'Zonnepanelen Opbrengst',
		titlefont:
		{
			//family: 'Courier New, monospace',
			size: 36,
			//color: '#7f7f7f'
		},
		height: 600,
		width: 800,
		yaxis:
		{
			title: 'Momentopname (Watt)',
			range: [0, upperBoundPower]
		},
		yaxis2:
		{
			title: 'Dagopbrengst Cummulatief (KWh)',
			range: [0, upperBoundCummulative],
			overlaying: 'y',
			side: 'right'
		},
		legend:
		{
			orientation: "v",
			x: 1.05,
			xanchor: 'left',
			y: 1,
		},
	};

	Plotly.newPlot('chart', data, layout, {displayModeBar: false});

	window.onresize = function() { Plotly.Plots.resize(gd) };
}
