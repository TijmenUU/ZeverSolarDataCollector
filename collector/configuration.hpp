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
	std::string archiveFile;
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
	std::string GetArchiveDirectory() { return archiveDirectory; } const;
	std::string GetArchiveFilePath() { return archiveDirectory + archiveFile } const;
	std::string GetErrorMsg() const { return errorMsg; }

	Configuration();

	Configuration(const std::string & configurationFile);
};

#endif
