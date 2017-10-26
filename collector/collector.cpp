#include "fileutils.hpp"
#include "configuration.hpp"
#include "zeverdata.hpp"

#include <ctime>
#include <stdexcept>
#include <fstream>
#include <string>

const std::string cConfigFile = "collecting.conf";

/* MAIN */
int main(int argc, char ** argv)
{
	Configuration config;
	const std::time_t now = time(0);
	if(argc > 1)
	{
		config.LoadFromFile(argv[1], now);
	}
	else
	{
		config.LoadFromFile(cConfigFile, now);
	}

	if(!config.IsValid())
	{
		throw std::runtime_error(config.GetErrorMsg());
	}

	ZeverData data;
	if(!data.FetchDataFromURL(config.GetURL(), config.GetFetchTimeOut()) &&
		!config.WriteOnFailure())
	{
		return 0;
	}

	// Check working directory (YEAR)
	const auto archiveDirectory = config.GetArchiveDirectory();
	if(!FileUtils::Exists(archiveDirectory))
	{
		if(!FileUtils::CreateDirectory(archiveDirectory))
		{
			throw std::runtime_error("Cannot create archive directory "
				+ archiveDirectory);
		}
	}
	// Check file (YEAR/MM_DD)
	archiveFile = config.GetArchiveFilePath();
	if(!FileUtils::Exists(archiveFile))
	{
		if(!FileUtils::CreateFile(archiveFile))
		{
			throw std::runtime_error("Cannot create file " + archiveFile);
		}
	}

	// Write results
	std::ofstream outputFile;
	outputFile.open(archiveFile, std::fstream::out | std::fstream::app);
	if(!outputFile.is_open())
	{
		throw std::runtime_error("Cannot access " + archiveFile + " for writing.");
	}
	outputFile << data.GetOutputStr(now);
	outputFile.close();

	return 0;
}
