#include <algorithm> // find, transform, tolower
#include <array>
#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip> // skipws
#include <curl/curl.h> // tested with libcurl4-openSSH
#include <sstream> // stringstream
#include <string>
#include <sys/types.h> // include before stat.h
#include <sys/stat.h>
#include <vector>

/* CONFIGURATION FILE */

// This file should be in the same location as the executable
// Can be overridden by launch parameter
const std::string cConfigFile = "collecting.conf";

// Settings
const char cCommentSymbol = '#';

const std::array<const std::string, 7> cConfigurationTags = {
	"fetchURL",
	"writeArchive",
	"writeZeroOnFailedFetch",
	"archiveLocation",
	"archiveFileExtension",
	"writeDailyReport",
	"reportLocation"
};

const int cFirstNoElement = 2;
const std::array<const std::string, 4> cConfigurationBinaryValues = {
	"yes",
	"y",
	"no",
	"n"
};

enum class ConfigurationTag
{
	UNKNOWN = -1,
	FETCHURL,
	WRITEARCHIVE,
	WRITEZERO,
	ARCHIVELOCATION,
	ARCHIVEFILEEXTENSION,
	WRITEREPORT,
	REPORTLOCATION
};

class Configuration
{
protected:
	std::string fetchURL;
	bool writeToArchive;
	bool writeOnFailure;
	std::string archiveDirectory;
	std::string archiveFileExtension;
	std::string reportFile;
	bool writeReport;
	bool isValid;

	// Returns UNKNOWN if the tag is unknown
	ConfigurationTag GetTag(const std::string & tagstr) const
	{
		auto result = std::find(cConfigurationTags.begin(),
			cConfigurationTags.end(),
			tagstr);
		if(result == cConfigurationTags.end())
		{
			return ConfigurationTag::UNKNOWN;
		}
		else
		{
			return static_cast<ConfigurationTag>(
				result - cConfigurationTags.begin());
		}
	}

	// Defaults to false
	bool GetBinaryValue(std::string valuestr) const
	{
		std::transform(valuestr.begin(), valuestr.end(), valuestr.begin(),
      	[](unsigned char c) -> unsigned char { return std::tolower(c); });

		auto result = std::find(cConfigurationBinaryValues.begin(),
			cConfigurationBinaryValues.end(),
			valuestr);
		if(result == cConfigurationBinaryValues.end())
		{
			return false;
		}
		else
		{
		 	return (result - cConfigurationBinaryValues.begin()) < cFirstNoElement;
		}
	}

public:
	// Validates the current configuration values for not being empty
	bool Validate() const
	{
		if(fetchURL.size() == 0)
		{
			return false;
		}
		if(writeToArchive)
		{
			return archiveDirectory.size() != 0;
		}
		if(writeReport)
		{
			return reportFile.size() != 0;
		}
		return true;
	}

	bool LoadFromFile(const std::string & fileLocation)
	{
		isValid = false;

		std::ifstream input;
		input.open(fileLocation);
		if(input.is_open())
		{
			while(input.good())
			{
				std::string rawLine;
				std::getline(input, rawLine);
				if(rawLine.size() < 1 || rawLine[0] == cCommentSymbol)
				{
					continue;
				}

				std::stringstream line;
				line.str(rawLine);
				line >> std::skipws; // ignore whitespaces

				std::string tagstr, valuestr;
				if(line >> tagstr && line >> valuestr)
				{
					auto tag = GetTag(tagstr);
					switch(tag)
					{
						case ConfigurationTag::FETCHURL:
						fetchURL = valuestr;
						break;

						case ConfigurationTag::WRITEARCHIVE:
						writeToArchive = GetBinaryValue(valuestr);
						break;

						case ConfigurationTag::WRITEZERO:
						writeOnFailure = GetBinaryValue(valuestr);
						break;

						case ConfigurationTag::ARCHIVELOCATION:
						archiveDirectory = valuestr;
						break;

						case ConfigurationTag::ARCHIVEFILEEXTENSION:
						if(valuestr[0] != '*')
						{
							archiveFileExtension = valuestr;
						}
						break;

						case ConfigurationTag::WRITEREPORT:
						writeReport = GetBinaryValue(valuestr);
						break;

						case ConfigurationTag::REPORTLOCATION:
						reportFile = valuestr;
						break;

						case ConfigurationTag::UNKNOWN:
						default:
						break;
					}
				}
			}

		}
		else
		{
			return false;
		}

		isValid = Validate();
		return true;
	}

	// Getters
	bool IsValid() const { return isValid; }
	std::string URLtoFetch() const { return fetchURL; }
	bool WriteArchive() const { return writeToArchive; }
	bool WriteOnFailure() const { return writeOnFailure; }
	std::string ArchiveStorageLocation() const { return archiveDirectory; }
	std::string ArchivearchiveFileExtension() const { return archiveFileExtension; }
	bool WriteReport() const { return writeReport; }
	std::string ReportFileLocation() const { return reportFile; }

	Configuration()
	: writeToArchive(false),
	writeOnFailure(false),
	writeReport(false),
	isValid(false)
	{
	}

	Configuration(const std::string & configurationFile)
	: writeToArchive(false),
	writeOnFailure(false),
	writeReport(false),
	isValid(false)
	{
		LoadFromFile(configurationFile);
	}
};

/* DATA COLLECTOR */

class ZeverData
{
protected:
	bool isValid;

public:
	int number0, number1;
	std::string registeryID, registeryKey, hardwareVersion, softwareVersion;
	std::string timeValue, dateValue, zeverCloudStatus;
	int number3;
	std::string inverterSN;
	int currentPower; // Watts
	double powerToday; // KWh
	std::string OKclass, ERRORclass;

	bool IsValid() const { return isValid; }

	ZeverData()
	: isValid(false)
	{
	}

	ZeverData(const std::string & rawData)
	{
		std::stringstream ss;
		ss.str(rawData);

		ss >> number0;
		ss >> number1;
		ss >> registeryID;
		ss >> registeryKey;
		ss >> hardwareVersion;
		ss >> softwareVersion;
		ss >> timeValue;
		ss >> dateValue;
		ss >> zeverCloudStatus;
		ss >> number3;
		ss >> inverterSN;
		isValid = (ss >> currentPower && ss >> powerToday);
		ss >> OKclass;
		ss >> ERRORclass;
	}
};

// libcurl callback
size_t write_to_string(void * ptr, size_t size,
	size_t nmemb,
	void * stream)
{
	std::string line((char *)ptr, nmemb);
	std::string * buffer = (std::string *)stream;
	buffer->append(line);

	return nmemb * size;
}

// libcurl callback
size_t headerdata2str_callback(char * buffer,
	size_t size,
	size_t nitems,
	void * userdata)
{
	static_cast<std::string *>(userdata)->insert(0, buffer, nitems);
	return size * nitems;
}

// TODO Move into class
ZeverData FetchData(const Configuration & config)
{
	ZeverData result;

	curl_global_init(CURL_GLOBAL_ALL);

	CURL * curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	std::string buffer;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

	curl_easy_setopt(curl, CURLOPT_URL, config.URLtoFetch().c_str());

	buffer.reserve(256);
	if (curl_easy_perform(curl) == CURLE_OK)
	{
		buffer.shrink_to_fit();

		result = ZeverData(buffer);
	}

	curl_easy_cleanup(curl);

	return result;
}

// TODO Move into class
// Helper method. Limited to 64 char length strings for now.
std::string GetTimeStr(const std::time_t & _time, const char * format)
{
	const size_t strBufferSize = 64U;

	tm * timeInfo;
	char buffer[strBufferSize];
	timeInfo = localtime(&_time);
	strftime(buffer, strBufferSize, format, timeInfo);

	std::string result;
	result.assign(buffer);
	return result;
}

// TODO Move into class
bool WriteArchive(const Configuration & config,
	const ZeverData & data)
{
	const std::time_t now = time(0);
	const std::string subfolder = config.ArchiveStorageLocation() +
		GetTimeStr(now, "%Y");
	const std::string fileName = GetTimeStr(now, "%m_%d");

	umask(0); // set mask for file creation
	struct stat st = {0};
	if(stat(subfolder.c_str(), &st) != 0) // does it exist?
	{
		if(mkdir(subfolder.c_str(), 0777) != 0) // no, create it
		{
			return false; // cannot create subfolder, abort
		}
	}

	const std::string storagePath = subfolder +
		'/' +
		fileName +
		config.ArchivearchiveFileExtension();

	std::ofstream archiveOut;
	archiveOut.open(storagePath, 
		std::fstream::out | std::fstream::app);
	if(archiveOut.is_open())
	{
		archiveOut << GetTimeStr(now, "%H:%M:%S");
		if(data.IsValid())
		{
			archiveOut << ' ' << data.currentPower << ' ' << data.powerToday << std::endl;
		}
		else if(config.WriteOnFailure())
		{
			archiveOut << ' ' << 0 << ' ' << 0.0 << std::endl;
		}

		archiveOut.close();

		return true;
	}

	return false;
}

// TODO Move into class
bool WriteReport(const Configuration & config,
	const ZeverData & data)
{
	const std::time_t now = time(0);
	const std::string isotime = GetTimeStr(now,"%Y-%m-%dT%H:%M:%SZ");

	umask(0); // set mask for file creation
	std::ofstream reportFileOut;
	std::ifstream reportFileIn;
	reportFileIn.open(config.ReportFileLocation());
	if(reportFileIn.is_open() &&
		reportFileIn.peek() != std::ifstream::traits_type::eof()) // not empty
	{
		std::string previousEntry;
		std::getline(reportFileIn, previousEntry);
		const std::string date = isotime.substr(0, 10);
		if(date.compare(0, date.size(), previousEntry, 0, date.size()) == 0)
		{
			// the file is up to date
			reportFileIn.close();
			reportFileOut.open(config.ReportFileLocation(),
				std::fstream::out | std::fstream::app); // append to the file
		}
		else
		{
			reportFileIn.close();
			reportFileOut.open(config.ReportFileLocation()); // overwrite file
		}
	}
	else
	{
		reportFileOut.open(config.ReportFileLocation(),
			std::fstream::out | std::fstream::trunc); // create file or truncate
	}

	if(reportFileOut.is_open())
	{
		if(data.IsValid())
		{
			reportFileOut << isotime << data.currentPower << ' ';
			reportFileOut << data.powerToday << std::endl;
		}
		else if(config.WriteOnFailure())
		{
			reportFileOut << isotime << ' ' << 0 << ' ' << 0.0 << std::endl;
		}

		reportFileOut.close();

		return true;
	}
	else
	{
		return false;
	}

}

/* MAIN */

int main(int argc, char ** argv)
{
	Configuration config;
	if(argc > 1)
	{
		config.LoadFromFile(argv[1]);
	}
	else
	{
		config.LoadFromFile(cConfigFile);
	}

	// DEBUG
	//std::cout << "Fetching " << config.URLtoFetch() << std::endl;
	//std::cout << "Write on failure: " << config.WriteOnFailure() << std::endl;
	//std::cout << "Write to archive: " << config.WriteArchive() << std::endl;
	//std::cout << "\tArchiving at " << config.ArchiveStorageLocation() << std::endl;
	//std::cout << "Write a report: " << config.WriteReport() << std::endl;
	//std::cout << "\tReporting to " << config.ReportFileLocation() << std::endl;
	// END DEBUG

	if(!config.IsValid())
	{
		throw std::runtime_error("The loaded configuration is invalid.");
		//std::cerr << "The loaded configuration is invalid." << std::endl;
	}

	auto data = FetchData(config);
	if(config.WriteArchive() && !WriteArchive(config, data))
	{
		throw std::runtime_error("Error during archiving, check your config file.");
		//std::cerr << "Error during archiving, check your config file." << std::endl;
	}

	if(config.WriteReport() && !WriteReport(config, data))
	{
		throw std::runtime_error("Error during writing of the report file.");
		//std::cerr << "Error during writing of the report file." << std::endl;
	}

	return 0;
}
