// stl
#include <algorithm> // find, transform, tolower
#include <array>
#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip> // skipws
#include <sstream> // stringstream
#include <string>
#include <vector>

// linux
#include <sys/types.h> // include before stat.h
#include <sys/stat.h> // stat
#include <fcntl.h> // open
#include <unistd.h> // close

// 3rd party
#include <curl/curl.h> // tested with libcurl4-openSSH

/* CONFIGURATION FILE */

// This file should be in the same location as the executable
// Can be overridden by launch parameter
const std::string cConfigFile = "collecting.conf";

// Settings
const char cCommentSymbol = '#';

enum class ConfigurationTag
{
	UNKNOWN = -1,
	FETCHURL,
	FETCHTIMEOUT,
	WRITEARCHIVE,
	WRITEZERO,
	ARCHIVELOCATION,
	ARCHIVEFILEEXTENSION,
	WRITEREPORT,
	REPORTLOCATION
};

const std::array<const std::string, 8> cConfigurationTags = {
	"fetchURL",
	"fetchTimeout",
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

class Configuration
{
protected:
	std::string fetchURL;
	unsigned int fetchTimeoutMs;
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

						case ConfigurationTag::FETCHTIMEOUT:
						fetchTimeoutMs = std::stoul(valuestr);
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
	unsigned int FetchTimeoutInMs() const { return fetchTimeoutMs; }
	bool WriteArchive() const { return writeToArchive; }
	bool WriteOnFailure() const { return writeOnFailure; }
	std::string ArchiveStorageLocation() const { return archiveDirectory; }
	std::string ArchivearchiveFileExtension() const { return archiveFileExtension; }
	bool WriteReport() const { return writeReport; }
	std::string ReportFileLocation() const { return reportFile; }

	Configuration()
	: fetchTimeoutMs(5000),
	writeToArchive(false),
	writeOnFailure(false),
	writeReport(false),
	isValid(false)
	{
	}

	Configuration(const std::string & configurationFile)
	: fetchTimeoutMs(5000),
	writeToArchive(false),
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

	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, config.FetchTimeoutInMs());

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
inline std::string GetTimeStr(const std::time_t & _time, const char * format)
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

// TODO move into class
// Helper method
// Returns whether a file OR directory exists
inline bool FileExists(const char * filepath)
{
	struct stat st = {0};
	if(stat(filepath, &st) < 0)
	{
		return false;
	}
	return true;
}

// TODO move into class
// Helper method
// Creates a directory in the given location with default permissions 0755
inline bool CreateDirectory(const char * dirpath, const unsigned int dirPerm = 0755)
{
	if(mkdir(dirpath, dirPerm) != 0)
	{
		return false;
	}

	std::cout << "Creating dir " << dirpath << std::endl; // DEBUG
	return true;
}

// TODO move into class
// Helper method
// Creates the file specified with default permissions 0644
inline bool CreateFile(const char * filePath, const unsigned int filePerm = 0644)
{
	auto fileDescriptor = open(filePath,
		O_WRONLY | O_CREAT, filePerm);
	if(fileDescriptor == -1)
	{
		return false;
	}
	std::cout << "Creating file " << filePath << std::endl; // DEBUG

	close(fileDescriptor);
	return true;
}

// TODO move into class
// Helper method
// Writes the datetime, currentPower and powerToday to the file stream
// followed by a line ending. Uses config to check if it should write 0s.
void WriteLine(std::ostream & outputStream,
	const std::time_t & now,
	const ZeverData & data,
	const Configuration & config)
{
	const unsigned int cFormattingWidth = 6U;
	const std::string isotime = GetTimeStr(now,"%Y-%m-%dT%H:%M:%S");

	if(data.IsValid())
	{
		outputStream << isotime << std::setw(cFormattingWidth)
			 << data.currentPower;
		outputStream << std::setw(cFormattingWidth)
			 << data.powerToday << std::endl;
	}
	else if(config.WriteOnFailure())
	{
		outputStream << isotime << std::setw(cFormattingWidth) << 0;
		outputStream << std::setw(cFormattingWidth) << 0.0 << std::endl;
	}
}

// TODO Move into class
// Creates the file and subdirectory if necessary. Files are created in a
// subdirectory matching the year (YYYY) and the files have the month_day
// format (MM_DD).
bool WriteArchive(const Configuration & config,
	const ZeverData & data)
{
	const std::time_t now = time(0);
	const std::string subfolder = config.ArchiveStorageLocation() +
		GetTimeStr(now, "%Y");
	const std::string fileName = GetTimeStr(now, "%m_%d");

	if(!FileExists(subfolder.c_str()))
	{
		if(!CreateDirectory(subfolder.c_str()))
		{
			return false;
		}
	}

	const std::string storagePath = subfolder +
		'/' +
		fileName +
		config.ArchivearchiveFileExtension();

	if(!FileExists(storagePath.c_str()))
	{
		if(!CreateFile(storagePath.c_str()))
		{
			return false;
		}
	}

	std::ofstream archiveOut;
	archiveOut.open(storagePath, std::fstream::out | std::fstream::app);
	if(archiveOut.is_open())
	{
		WriteLine(archiveOut, now, data, config);

		archiveOut.close();

		return true;
	}

	return false;
}

// TODO Move into class
// Creates the file if it doesn't exist already. If the file exists and is not
// empty it checks if the date is still the same. If the year, month and day
// match it appends to the file. If it does not it empties the file and appends.
bool WriteReport(const Configuration & config,
	const ZeverData & data)
{
	const std::time_t now = time(0);
	const std::string isotime = GetTimeStr(now,"%Y-%m-%dT%H:%M:%S");

	auto reportFileLocation_cstr = config.ReportFileLocation().c_str();
	if(!FileExists(reportFileLocation_cstr))
	{
		if(!CreateFile(reportFileLocation_cstr))
		{
			return false;
		}
	}

	std::ofstream reportFileOut;
	std::ifstream reportFileIn;
	reportFileIn.open(config.ReportFileLocation());
	if(reportFileIn.is_open() &&
		reportFileIn.peek() != std::ifstream::traits_type::eof()) // not empty
	{
		std::string previousEntry;
		std::getline(reportFileIn, previousEntry);
		const std::string date = isotime.substr(0, 10); // Does the date match?
		if(date.compare(0, date.size(), previousEntry, 0, date.size()) == 0)
		{
			reportFileIn.close();
			reportFileOut.open(config.ReportFileLocation(),
				std::fstream::out | std::fstream::app); // append to the file
		}
		else
		{
			reportFileIn.close();
			reportFileOut.open(config.ReportFileLocation(),
				std::fstream::out | std::fstream::trunc); // overwrite file
		}
	}
	else
	{
		reportFileOut.open(config.ReportFileLocation(),
			std::fstream::out | std::fstream::trunc); // overwrite file
	}

	if(reportFileOut.is_open())
	{
		WriteLine(reportFileOut, now, data, config);

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
