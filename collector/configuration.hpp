#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <ctime>
#include <string>

/*
	This class stores the configured parameters found in the configuration file
	and constructs the complete archive paths using this and the timestamp.

	The generated directory is:
	archive_path_supplied/YYYY/
	The generated archive filepath is
	archive_path_supplied/YYYY/MM_DD.extension_supplied
*/

class Configuration
{
protected:
	std::time_t constructTimeStamp;
	std::string fetchURL;
	unsigned int fetchTimeoutMs;
	bool writeOnFailure;
	std::string archiveDirectory;
	std::string archiveFile;
	bool isValid;

	std::string errorMsg;

public:
	// Validates the current configuration values for not being empty
	// sets errorMsg, therefore not const
	bool Validate();

	// Make sure to set the timestamp before calling this! See SetTimestamp or
	// use the appropiate constructor
	bool LoadFromFile(const std::string & fileLocation);

	// Setters
	void SetTimestamp(const std::time_t timestamp);

	// Getters
	bool IsValid() const;
	std::time_t GetTimestamp() const;
	std::string GetURL() const;
	// in milliseconds
	unsigned int GetFetchTimeOut() const;
	bool WriteOnFailure() const;
	std::string GetArchiveDirectory() const;
	std::string GetArchiveFilePath() const;
	std::string GetErrorMsg() const;

	// Returns a blank configuration object
	// Set the timestamp and call LoadFromFile at a later stage to make it a
	// usable object
	Configuration();

	// Construct a configuration for the local time now using clock()
	Configuration(const std::string & configurationFile);

	// Construct a configuration for a specific timestamp
	Configuration(const std::string & configurationFile,
		const std::time_t timestamp);
};

#endif
