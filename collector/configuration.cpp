#include "configuration.hpp"
#include "timeutils.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <sstream>

// Settings
const char cCommentSymbol = '#';

enum class ConfigurationTag
{
	UNKNOWN = -1,
	FETCHURL,
	FETCHTIMEOUT,
	WRITEZERO,
	ARCHIVEDIRECTORY,
	ARCHIVEFILEEXTENSION,
	ARCHIVEFILENAME
};

const std::array<const std::string, 6> cConfigurationTags = {
	"fetchURL",
	"fetchTimeout",
	"writeZeroOnFailedFetch",
	"archiveDirectory",
	"archiveFileExtension",
	"archiveFilename"
};

const int cFirstNoElement = 2;
const std::array<const std::string, 4> cConfigurationBinaryValues = {
	"yes",
	"y",
	"no",
	"n"
};

ConfigurationTag GetTag(const std::string & tagstr)
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

bool GetBinaryValue(std::string valuestr)
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

bool Configuration::Validate()
{
	if(fetchURL.size() == 0)
	{
		errorMsg = "Empty URL in configuration file.";
		return false;
	}
	if(archiveDirectory.size() == 0)
	{
		errorMsg = "Missing archive directory in configuration file.";
		return false;
	}
	if(archiveFile.size() == 0)
	{
		errorMsg = "Missing archive filename.";
		return false;
	}
	return true;
}

bool Configuration::LoadFromFile(const std::string & fileLocation,
	const std::time_t & timestamp)
{
	isValid = false;

	std::ifstream input;
	input.open(fileLocation);
	bool filenameSpecified = false;
	std::string archiveFileExtension;
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

			auto splitPos = rawLine.find(' ');
			if(splitPos != std::string::npos)
			{
				std::string tagstr = rawLine.substr(0, splitPos);
				++splitPos;
				std::string valuestr = rawLine.substr(splitPos, rawLine.size());

				auto tag = GetTag(tagstr);
				switch(tag)
				{
					case ConfigurationTag::FETCHURL:
					fetchURL = valuestr;
					break;

					case ConfigurationTag::FETCHTIMEOUT:
					fetchTimeoutMs = std::stoul(valuestr);
					break;

					case ConfigurationTag::WRITEZERO:
					writeOnFailure = GetBinaryValue(valuestr);
					break;

					case ConfigurationTag::ARCHIVEDIRECTORY:
					archiveDirectory = valuestr;
					break;

					case ConfigurationTag::ARCHIVEFILEEXTENSION:
					if(valuestr[0] != '*')
					{
						archiveFileExtension = valuestr;
					}
					break;

					case ConfigurationTag::ARCHIVEFILENAME:
					archiveFile = valuestr;
					filenameSpecified = true;
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
		errorMsg = "Could not open configuration file " + fileLocation;
		return false;
	}

	if(!filenameSpecified)
	{
		archiveDirectory += TimeUtils::GetFormattedTimeStr(timestamp, "%Y");
		archiveFile = TimeUtils::GetFormattedTimeStr(timestamp, "%m_%d") +
			archiveFileExtension;
	}

	isValid = Validate();
	return true;
}

Configuration::Configuration()
: fetchTimeoutMs(5000),
writeOnFailure(false),
isValid(false)
{
}

Configuration::Configuration(const std::string & configurationFile)
: fetchTimeoutMs(5000),
writeOnFailure(false),
isValid(false)
{
	LoadFromFile(configurationFile);
}
