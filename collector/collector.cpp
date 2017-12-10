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
	config.SetTimestamp(time(nullptr));

	if(argc > 1)
	{
		config.LoadFromFile(argv[1]);
	}
	else
	{
		config.LoadFromFile(cConfigFile);
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
	const auto archiveFilePath = config.GetArchiveFilePath();
	if(!FileUtils::Exists(archiveFilePath))
	{
		if(!FileUtils::CreateFile(archiveFilePath))
		{
			throw std::runtime_error("Cannot create file " + archiveFilePath);
		}
	}

	// Write results
	std::ofstream outputFile;
	outputFile.open(archiveFilePath, std::fstream::out | std::fstream::app);
	if(!outputFile.is_open())
	{
		throw std::runtime_error("Cannot access " + archiveFilePath + " for writing.");
	}
	outputFile << data.GetOutputStr(config.GetTimestamp());
	outputFile.close();

	return 0;
}
