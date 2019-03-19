# ZeverSolarDataCollector

**NOTE** This project has been moved to https://git.valkendaal.duckdns.org/tijmen/solar-logger . This repository is no longer maintained.

Simple data collecting program for ZeverSolar's Zeverlution Sxxxx "smart" inverters with a network interface (referred to as "combox" by the manufacturer). It collects all the exposed data of the Zeverlution, including current power generation (watts) and today's cummulative power production (kilowatt/hours).

## Data Collection
The data collector can be build by running the makefile (`make all`) in the project root. It will create a `build` folder where it puts the newly created binaries. Move these to wherever you keep your binaries.

The program `collector` outputs to stdout in the format `[datestring] current-power-in-watts current-cumulative-power-generated-in-kwh`, with the datestring being optional. The launch parameters it supports are:
- `-u` mandatory parameter with the URL to the smart inverter's website (e.g. `-u http://192.168.2.23/home.cgi`). Long form is `--fetch-url`.
- `-t` optional parameter with the timeout in milliseconds (e.g. `-t 5000`). Some smart inverters can respons really slowly. By default this is 1000 (1 second), but you can increase this value if you're experiencing a lot of intermittent failed fetches. Long form is `--fetch-timeout`.
- `d` optional parameter with a date string that gets put before the watts and kilowatts figures in stdout (e.g. `-d 2017/01/30`). Long form is `--date-string`. You may put whatever you want here, but a datetime value seems most appropiate.

An example implementation is provided in the bash script `/collect.sh`. This script outputs the fetched data to file structure `[path]/YYYY/MM_DD.txt`. See the script source for more details and explanation.

Dependencies for this program are:
- `libcurl` which can be installed using your package manager. It has been tested with the `openSSL` flavour, libcurl version 4.

# Data Correction
Zeverlution Smart Inverters currently have a strange bug that causes leading zeroes in a decimal number to be ignored. Concretely this means that the value 0.01 becomes 0.10, 3.09 becomes 3.9, etcetera. This is causing strange peaks in the logged data where sequences go from 2.80 to 2.90 to 2.10. The `decimalfixer` program attempts to correct out the most obvious wrong values, but cannot completely solve the issue without making dangerous assumptions about log interval and the maximum power that can be produced in a given interval. Therefore it can fix any of these errors surrounded by normal values with small enough differences, e.g. the listed example sequence before. It cannot solve erroneous results when the logging interval is too big or when the production capacity causes large increments between two consecutive logged data points.

**NOTE:** using the decimalfixer utility and logging to a single file (over multiple days) is a really bad idea! The decimalfixer program only looks at the kilowatthour values.

The decimalfixer can be build by running the makefile (`make all`) in the project root. It will create a `build` folder where it puts the newly created binaries. Move these to wherever you keep your binaries.

The decimalfixer program has one launch parameter: the file to fix. An example implementation for how to use it can be found in `/decimalfix.sh`. For more information please see its source code.

This program has no external dependencies.

# Branches
- `master` branch is your goto branch for stable builds.
- `development` branch may contain unstable builds and/or broken code.

- `windows-compatible-stale` contains an older version of the project which was still build and tested for Windows systems. The master and development branch are explicitly NOT compatible with Windows.
- `website-version-stale` contains a folder `www/` in the project root which contains a small website that can display the collected logs. It is no longer supported but may serve as an inspiration if you feel like creating a frontend for the data collection. This branch is also still Windows compatible.
