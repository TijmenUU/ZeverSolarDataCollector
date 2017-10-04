//#include <curl/curl.h> // libcurl is a dependency

#include <algorithm> // find, transform, tolower
#include <array>
#include <exception>
#include <fstream>
#include <sstream> // stringstream
#include <string>
#include <vector>

// This file should be in the same location as the executable
// Can be overridden by launch parameter
const std::string cConfigFile = "collecting.conf";

// Settings
const char cCommentSymbol = '#';

const std::array<const std::string, 5> cConfigurationTags = {
	"fetchURL",
	"writeZeroOnFailedFetch",
	"storageLocation",
	"writeDailyReport",
	"dailyReportLocation"
};

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
	WRITEZERO,
	STORAGELOCATION,
	WRITEREPORT,
	REPORTLOCATION
};

class Configuration
{
protected:
	std::string fetchURL;
	bool writeOnFailure;
	std::string storageDirectory;
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
			return (result - cConfigurationBinaryValues.begin()) > 1;
		}
	}

public:
	// Validates the current configuration values for not being empty
	bool Validate() const
	{
		if(fetchURL.size() == 0 || storageDirectory.size() == 0)
		{
			return false;
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

				std::string tagstr, valuestr;
				if(line >> tagstr && line >> valuestr)
				{
					auto tag = GetTag(tagstr);
					switch(tag)
					{
						case ConfigurationTag::FETCHURL:
						fetchURL = valuestr;
						break;

						case ConfigurationTag::WRITEZERO:
						writeOnFailure = GetBinaryValue(valuestr);
						break;

						case ConfigurationTag::STORAGELOCATION:
						storageDirectory = valuestr;
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
		return isValid;
	}

	// Getters
	bool IsValid() const { return isValid; }
	std::string URLtoFetch() const { return fetchURL; }
	std::string DataStorageLocation() const { return storageDirectory; }
	bool ShouldWriteReport() const { return writeReport; }
	std::string ReportFileLocation() const { return reportFile; }

	Configuration()
	: writeOnFailure(false),
	writeReport(false),
	isValid(false)
	{
	}

	Configuration(const std::string & configurationFile)
	: writeOnFailure(false),
	writeReport(false),
	isValid(false)
	{
		LoadFromFile(configurationFile);
	}

	// Pass an empty string as third argument if you do not want a daily report
	Configuration(const std::string & urlToFetch,
		const std::string & dataStorageLocation,
		const std::string & opt_dailyReportLocation,
		const bool writeOnFailedFetch = false)
	: fetchURL(urlToFetch),
	writeOnFailure(writeOnFailedFetch),
	storageDirectory(dataStorageLocation),
	reportFile(opt_dailyReportLocation),
	writeReport(reportFile.size() != 0U),
	isValid(true)
	{

	}
};

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

	if(!config.IsValid())
	{
		throw std::runtime_error("The given configuration is invalid.");
	}

	return 0;
}
