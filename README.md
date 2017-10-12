# ZeverSolarDataCollector
Simple data collecting program for ZeverSolar Sxxxx "smart" inverters with a network connection (referred to as "combox" by the manufacturer). These are build in for the Zeverlution Sxxxx series.

# Data Collection
The data collector can be build by using the makefile in the `/collector/` folder. Build the tool using `make test` with the existing makefile or adapt the makefile to your requirements and environment. Make your own configuration file by using `/collector.conf` as a template. Supply the program with this configuration file as a first argument when calling the program like so `./test /link-to/your/configuration/file.conf`. Be sure that the configuration file has the correct permissions set. Use crontab to run the program at your desired interval (see `sudo crontab -u user_name -e`) or use a bash script.

The program by default creates folders with permission 755 and files with permission 644 to be compatible with the data displaying website below. Configurable permissions are in the pipeline but are not supported at this time.

If you only want to keep the of today you could add a line to your `crontab` that deletes the contents of the root archive folder (set in your configuration file) at the desired time.

Dependencies for this program are:
`libcurl4-openssl-dev` which can be installed using your package manager.

# Data Displaying
Along with the data collector a small website working on the collected data can be found in the `/www/` folder of this repository. The `/www/solarpanel` folder should be replaced by a symlink to the configured root archive folder of the data collector program. 
