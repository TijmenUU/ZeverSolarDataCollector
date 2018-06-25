# ZeverSolarDataCollector
Simple data collecting program for ZeverSolar's Zeverlution Sxxxx "smart" inverters with a network interface (referred to as "combox" by the manufacturer). It collects all the exposed data of the Zeverlution, including current power generation (watts) and today's cummulative power production (kilowatt/hours). This data is then stored per day in a text file. The included website allows for easy viewing of this collected data. Currently supported are the (GNU) Linux and Windows 10 platforms, tested on a Raspberry Pi 1 running `Linux DietPi 4.9.52+ Mon 2 Oct 2017 build`, a Lenovo Thinkpad T410 running `Linux 4.10.0-37-generic #41~16.04.1 Ubunutu SMP` and a modern desktop running `Windows 10 x64 version 1703`. Other version of Windows may be supported but haven't been tested yet.

## Data Collection on Linux
The data collector can be build by using the makefile in the `/collector/` folder. Build the tool using `make collector` with the existing makefile or adapt the makefile to your requirements and environment. Make your own configuration file by using `/collector.conf` as a template. Supply the program with this configuration file as a first argument when calling the program like so `./collector /path/to/your/configuration/file.conf`. Be sure that the configuration file has the correct permissions set. Use crontab to run the program at your desired interval (see `sudo crontab -u user_name -e`) or use a bash script.

The program by default creates folders with permission 755 and files with permission 644. You can change this in the `/collector/fileutils.hpp` if you want different default permissions. If today's date is 22-11-2017 (DD-MM-YYYY) then the collector will create a folder `2017` in the configured root storage folder and within this newly created folder a file `11_22` (extension depends on what you configured, defaults to `.txt`). Any additional recordings on the same date will be appended to the created file.

See the template configuration file for all the options.

Dependencies for this program are:
- `libcurl4-openssl-dev` which can be installed using your package manager.

# Data Correction
Zeverlution Smart Inverters currently have a strange bug that causes leading zeroes in a decimal number to be ignored. Concretely this means that the value 0.01 becomes 0.10, 3.09 becomes 3.9, etcetera. This is causing strange peaks in the logged data where sequences go from 2.80 to 2.90 to 2.10. The `decimalfixer` program attempts to correct out the most obvious wrong values, but cannot completely solve the issue without making dangerous assumptions about log interval and the maximum power that can be produced in a given interval. Therefore it can fix any of these errors surrounded by normal values with small enough differences, e.g. the listed example sequence before. It cannot solve erroneous results when the logging interval is too big or when the production capacity causes large increments between two consecutive logged data points.

The decimalfixer program can be called in three ways:
- `decimalfixer -s /path/to/file/you/want/fixed` to fix the specified file
- `decimalfixer -t /path/to/your/configuration/file.conf` to fix today's file (whatever file belongs to the current date)
- `decimalfixer -y /path/to/your/configuration/file.conf` to fix yesterday's file (current date subtracted by 1 day)

**NOTE:** using the decimalfixer utility and a single archiving file is not a good idea!

## Data Correction on Linux
Make it using `make decimalfixer` and add it, for instance, to your crontab.

This program has no external dependencies.

# Branches
- `master` branch is your goto branch for stable builds.
- `development` branch may contain unstable builds and/or broken code.

- `windows-compatible-stale` contains an older version of the project which was still build and tested for Windows systems. The master and development branch are explicitly NOT compatible with Windows.
- `website-version-stale` contains a folder `www/` in the project root which contains a small website that can display the collected logs. It is no longer supported but may serve as an inspiration if you feel like creating a frontend for the data collection. This branch is also still Windows compatible.