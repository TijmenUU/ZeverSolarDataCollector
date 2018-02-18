"use strict";

const chartHeight = 600; // in px

/* Strings */
const chartTitle = "Overzicht periode";
const noDataMsg = 'Er is geen data om te laten zien voor de geselecteerde dagen.'; // No data available for the selected day
const helpMsg = 'Selecteer een begin en eind dag voor de weer te geven data en druk op Laad Data.';

window.onload = OnLoad;

function OnLoad() {
    UpdateDatePickers();
    DisplayHelp();
}

function UpdateDatePickers() {
    var date = moment();
    document.getElementById('end-chartdate').value = date.format("YYYY-MM-DD");
    document.getElementById('start-chartdate').value = date.subtract(1, 'month').format("YYYY-MM-DD");
}

async function TryLoad() {
    const startDate = moment(document.getElementById('start-chartdate').value);
    const endDate = moment(document.getElementById('end-chartdate').value);

    var statistics = {
        kilowatthour: [],
        date: [],
        max_kilowatthour: 0.0,
        max_kilowatthour_date: "",
        average_kilowatthour: 0.0
    };

    var date = startDate;
    while (date <= endDate) {
        var data = "";
        function setdata(datastr) {
            data = datastr;
        };

        await GetDateFile(date, setdata, function (date) { return; });

        if (data.length > 0) {
            var lines = data.split('\n');
            var i = lines.length - 1;
            while (lines[i].length < 1 && i >= 0) {
                --i;
            }
            if (i >= 0) {
                var linedata = ParseLine(lines[i]);

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
    else
    {
        DisplayError();
    }
}

function DisplayHelp() {
    document.getElementById("chart-col").innerHTML = helpMsg;
    document.getElementById("kilowatt-stats").innerHTML = helpMsg;
    //document.getElementById("watt-stats").innerHTML = helpMsg;
}

function DisplayError() {
    document.getElementById("chart-col").innerHTML = noDataMsg;
    document.getElementById("kilowatt-stats").innerHTML = noDataMsg;
    //document.getElementById("watt-stats").innerHTML = noDataMsg;
}

function DisplayData(statistics) {
    DrawChart(statistics);
    UpdateTiles(statistics);
}

function UpdateTiles(stats)
{
    document.getElementById('kilowatt-stats').innerHTML = "Hoogste dagopbrengst gemeten was op " + moment(stats.max_kilowatthour_date).format("DD-MM-YYYY") + " met een waarde van " + stats.max_kilowatthour + " kilowatt/uur. De gemiddelde opbrengst over deze periode is " + stats.average_kilowatthour.toFixed(1) + " kilowatt/uur.";

    document.getElementById('watt-stats').innerHTML = "Hier is nog niks te zien."; // debug

}

function UpdateChartWidth() {
    var newWidth = parseInt($("#chart-col").css("width"));
    var existingPlot = document.getElementById('chart-col');
    if (existingPlot.layout.width !== newWidth) {
        Plotly.relayout('chart-col', { width: newWidth /*, legend: { x: GetLegendPosX(newWidth) }*/ });
    }
}

function DrawChart(stats) {
    document.getElementById("chart-col").innerHTML = ""; // clear possible error msg

    var dates = [];
    for(var i = 0; i < stats.date.length; ++i)
    {
        dates.push(moment(stats.date[i]).format("DD-MM-YYYY"));
    }

    var trace1 = {
        x: dates,
        y: stats.kilowatthour,
        name: 'Dagopbrengst',
        type: 'bar',
        marker: {
            color: '#2ECC71'
        }
    };

    var average_trace = [];
    for(var i = 0; i < dates.length; ++i)
    {
        average_trace.push(stats.average_kilowatthour);
    }

    var trace2 = {
        x: dates,
        y: average_trace,
        name: 'Gemiddelde',
        line:
            {
                shape: 'spline',
                color: '#8E44AD',
            },
        type: 'scatter',
        connectgaps: true
    };

    var width = parseInt($("#chart-col").css("width"));

    var layout = {
        title: chartTitle + moment(stats.date[0]).format(" DD-MM-YYYY") + " tot en met" + moment(stats.date[stats.date.length - 1]).format(" DD-MM-YYYY"),
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
                //title: 'Datum',
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

    window.onresize = UpdateChartWidth;
}

function DrawTiles(stats) {
    if (stats === undefined || stats === null || stats.date.length < 1) {
        document.getElementById('kilowatt-stats').innerHTML = noDataMsg;
        document.getElementById('watt-stats').innerHTML = noDataMsg;
        document.getElementById('activity-stats').innerHTML = noDataMsg;
        return;
    }

    var cummulativeWatts = 0;
    var wattLow = stats.max_watt;
    var wattHigh = 0;
    for (var i = 0; i < stats.watt.length; ++i) {
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