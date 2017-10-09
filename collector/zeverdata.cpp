#include "zeverdata.hpp"
#include "timeutils.hpp"

#include <curl/curl.h> // tested with libcurl4-openSSH
#include <sstream>

std::string ZeverData::GetOutputStr(const std::time_t & timestamp,
	const unsigned int collumnPadding) const
{
	std::stringstream ss;

	ss << isotime << std::setw(collumnPadding) << std::to_string(currentPower);
	ss << std::setw(collumnPadding) << std::to_string(powerToday) << std::endl;

	return ss.str();
}

bool ZeverData::ParseString(const std:string & str)
{
	std::stringstream ss;
	ss.str(str);

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

	if(!isValid)
	{
		errorMsg = "Error during parsing of zever data.";
	}

	return isValid;
}

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

bool ZeverData::FetchDataFromURL(const std::string & url,
	const unsigned int timeout)
{
	ZeverData result;

	curl_global_init(CURL_GLOBAL_ALL);

	CURL * curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	std::string buffer;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	buffer.reserve(256);
	if (curl_easy_perform(curl) == CURLE_OK)
	{
		buffer.shrink_to_fit();

		curl_easy_cleanup(curl);
		ParseString(buffer);

		return ParseString(buffer);
	}
	else
	{
		currentPower = 0;
		powerToday = 0.0;

		errorMsg = "Failed to fetch zever data from URL.";
	}

	curl_easy_cleanup(curl);
	return false;
}

ZeverData::ZeverData()
: isValid(false),
currentPower(0),
powerToday(0.0)
{
}

ZeverData::ZeverData(const std::string & rawData)
{
	if(!ParseString(rawData))
	{
		currentPower = 0;
		powerToday = 0.0;
	}
}
