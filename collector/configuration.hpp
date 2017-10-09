#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP
#pragma once

#include <ctime>
#include <string>

class Configuration
{
protected:
	std::string fetchURL;
	unsigned int fetchTimeoutMs;
	bool writeOnFailure;
	std::string archiveDirectory;
	std::string archiveFileExtension;
	bool isValid;

	std::string errorMsg;

public:
	// Validates the current configuration values for not being empty
	// sets errorMsg, therefore not const
	bool Validate();

	bool LoadFromFile(const std::string & fileLocation);

	// Getters
	bool IsValid() const { return isValid; }
	std::string GetURL() const { return fetchURL; }
	// in milliseconds
	unsigned int GetFetchTimeOut() const { return fetchTimeoutMs; }
	bool WriteOnFailure() const { return writeOnFailure; }
	std::string GetErrorMsg() const { return errorMsg; }

	std::string GetArchiveDirectory(const std::time_t & timestamp) const;
	std::string GetArchiveFilePath(const std::time_t & timestamp) const;

	Configuration();

	Configuration(const std::string & configurationFile);
};

#endif
