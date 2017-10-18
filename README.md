# ZeverSolarDataCollector
Simple data collecting program for ZeverSolar Sxxxx "smart" inverters with a network interface (referred to as "combox" by the manufacturer). These are build in for the Zeverlution Sxxxx series. Currently supports only (GNU) Linux, tested on a Raspberry Pi 1 running `Linux DietPi 4.9.52+ Mon 2 Oct 2017 build` and a Lenovo Thinkpad T410 running `Linux 4.10.0-37-generic #41~16.04.1 Ubunutu SMP`.

# Data Collection
The data collector can be build by using the makefile in the `/collector/` folder. Build the tool using `make collector` with the existing makefile or adapt the makefile to your requirements and environment. Make your own configuration file by using `/collector.conf` as a template. Supply the program with this configuration file as a first argument when calling the program like so `./collector /link-to/your/configuration/file.conf`. Be sure that the configuration file has the correct permissions set. Use crontab to run the program at your desired interval (see `sudo crontab -u user_name -e`) or use a bash script.

The program by default creates folders with permission 755 and files with permission 644 to be compatible with the data displaying website below. Configurable permissions are in the pipeline but are not supported at this time.

If you only want to keep the of today you could add a line to your `crontab` that deletes the contents of the root archive folder (set in your configuration file) at the desired time.

Dependencies for this program are:
`libcurl4-openssl-dev` which can be installed using your package manager.

# Data Displaying
Along with the data collector a small website working on the collected data can be found in the `/www/` folder of this repository. The `/www/solarpanel` folder should be replaced by a symlink to the configured root archive folder of the data collector program.

# Data Correction
Zeverlution Smart Inverters currently have a strange bug that causes leading zeroes in a decimal number to be ignored. Concretely this means that the value 0.01 becomes 0.10, 3.09 becomes 3.9, etcetera. This is causing strange peaks in the logged data where sequences go from 2.80 to 2.90 to 2.10. The "decimalfixer" program attempts to correct out the most obvious wrong values, but cannot completely solve the issue without making dangerous assumptions about log interval and the maximum power that can be produced in a given interval. Therefore it can fix any of these errors surrounded by normal values with small enough differences, e.g. the listed example sequence before. It cannot solve erroneous results when the logging interval is too big or when the production capacity causes large increments between two consecutive logged data points.  

Make it using `make decimalfixer` and add it, for instance, to your crontab. It will subtract a day from the current time and check the file that belongs to that date (yesterday). A good time to execute this program would be for instance every 1st minute of the day: `1 0 * * * cd /path/to/decimalfixer/binary && ./decimalfixer /link-to/your/configuration/file.conf`.

This program has no external dependencies.
