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
	ARCHIVELOCATION,
	ARCHIVEFILEEXTENSION,
	SINGLEFILEARCHIVEPATH
};

const std::array<const std::string, 6> cConfigurationTags = {
	"fetchURL",
	"fetchTimeout",
	"writeZeroOnFailedFetch",
	"archiveLocation",
	"archiveFileExtension",
	"singleFileArchivePath"
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
		errorMsg = "Missing archive path in configuration file.";
		return false;
	}
	return true;
}

bool Configuration::LoadFromFile(const std::string & fileLocation)
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

					case ConfigurationTag::ARCHIVELOCATION:
					archiveDirectory = valuestr;
					break;

					case ConfigurationTag::ARCHIVEFILEEXTENSION:
					if(valuestr[0] != '*')
					{
						archiveFileExtension = valuestr;
					}
					break;

					case ConfigurationTag::SINGLEFILEARCHIVEPATH:
					singleFileArchivePath = valuestr;
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

	isValid = Validate();
	return true;
}

std::string Configuration::GetArchiveDirectory(const std::time_t & timestamp) const
{
	return archiveDirectory + TimeUtils::GetFormattedTimeStr(timestamp, "%Y");
}

std::string Configuration::GetArchiveFilePath(const std::time_t & timestamp) const
{
	return GetArchiveDirectory(timestamp) +
	'/' +
	TimeUtils::GetFormattedTimeStr(timestamp, "%m_%d") +
	archiveFileExtension;
}

std::string Configuration::GetSingleArchiveFilePath() const
{
	return singleFileArchivePath;
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
