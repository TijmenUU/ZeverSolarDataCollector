#include "fileutils.hpp"
#include "configuration.hpp"
#include "zeverdata.hpp"

#include <ctime>
#include <fstream>
#include <stdexcept>
#include <string>

/* MAIN */
int main(int argc, char ** argv)
{
	Configuration config;
	config.SetTimestamp(time(nullptr));

	if(argc != 2)
	{
		throw std::runtime_error("ERROR: Expected configuration file as sole launch paremeter.\n");
		return -1;
	}

	if(!config.LoadFromFile(argv[1]))
	{
		throw std::runtime_error(config.GetErrorMsg());
	}
	//config.Print(); // debug

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
