#pragma once

#include <cstdio>
#include <ctime>
#include <curl/curl.h> // tested with libcurl4-openSSH
#include <sstream>
#include <string>

namespace detail
{
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
}

/*
	This class holds all the data retrieved from the Zeverlution combox /
	Zeverlution Sxxxx smart inverters / ZeverSolar box.

	For output formatting it uses a timestamp to log the datetime alongside the
	watts and kilowatts.
*/

struct ZeverData
{
	int number0, number1;
	std::string registeryID, registeryKey, hardwareVersion, appVersion, wifiVersion;
	std::string timeValue, dateValue, zeverCloudStatus;
	int number3;
	std::string inverterSN;
	int currentPower; // Watts
	double powerToday; // KWh
	std::string OKmsg, ERRORmsg;

	// Parses the data string coming from the zeverlution home.cgi webpage
	bool ParseString(const std::string & str)
	{
		bool isValid = true;
		std::stringstream ss;
		ss.str(str);

		ss >> number0;
		ss >> number1;
		ss >> registeryID;
		ss >> registeryKey;
		ss >> hardwareVersion;
		std::string versionInfo;
		ss >> versionInfo;
		size_t splitPos = versionInfo.find('+');
		appVersion = versionInfo.substr(0, splitPos);
		++splitPos;
		wifiVersion = versionInfo.substr(splitPos, versionInfo.size());

		ss >> timeValue;
		ss >> dateValue;
		ss >> zeverCloudStatus;
		ss >> number3;
		ss >> inverterSN;
		isValid = (ss >> currentPower && ss >> powerToday);
		ss >> OKmsg;
		ss >> ERRORmsg;

		if(!isValid)
		{
			std::fprintf(stderr, "Error during parsing of zever data <%s>!\n", str.c_str());
			return false;
		}

		return true;
	}

	bool FetchDataFromURL(const std::string & url, const unsigned int timeout)
	{
		curl_global_init(CURL_GLOBAL_ALL);

		CURL * curl = curl_easy_init();

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		std::string buffer;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, detail::write_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		buffer.reserve(256);
		if (curl_easy_perform(curl) != CURLE_OK)
		{
			currentPower = 0;
			powerToday = 0.0;

			std::fprintf(stderr, "Failed to fetch zever data from URL <%s>!\n", url.c_str());

			curl_easy_cleanup(curl);
			return false;
		}
		
		buffer.shrink_to_fit();

		curl_easy_cleanup(curl);

		return ParseString(buffer);
	}

	ZeverData()
		: currentPower(0),
		powerToday(0.0)
	{
	}
};
