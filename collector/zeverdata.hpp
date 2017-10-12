#ifndef ZEVERDATA_HPP
#define ZEVERDATA_HPP
#pragma once

#include <ctime>
#include <string>

class ZeverData
{
protected:
	bool isValid;
	int number0, number1;
	std::string registeryID, registeryKey, hardwareVersion, appVersion, wifiVersion;
	std::string timeValue, dateValue, zeverCloudStatus;
	int number3;
	std::string inverterSN;
	int currentPower; // Watts
	double powerToday; // KWh
	std::string OKclass, ERRORclass;

	std::string errorMsg;

public:
	// Returns whether currentPower and powerToday are valid
	bool IsValid() const { return isValid; }
	/* Fetched data values */
	std::string GetRegisteryID() const { return registeryID; }
	std::string GetRegisteryKey() const { return hardwareVersion; }
	std::string GetAppVersion() const { return appVersion; }
	std::string GetWifiVersion() const { return wifiVersion; }
	std::string GetTime() const { return timeValue; }
	std::string GetDate() const { return dateValue; }
	std::string GetZeverCloudStatus() const { return zeverCloudStatus; }
	std::string GetInverterSerialNumber() const { return inverterSN; }
	int GetCurrentPower() const { return currentPower; }
	double GetCumulativePower() const { return powerToday; }
	std::string GetOkString() const { return OKclass; }
	std::string GetErrorString() const { return ERRORclass; }
	/* Error handling */
	std::string GetErrorMsg() const { return errorMsg; }
	// Returns a space seperated local iso datetime (not the fetched datatime),
	// currentPower and powerToday string
	std::string GetOutputStr(const std::time_t & timestamp,
		const unsigned int collumnPadding = 8U) const;
	// Parses the data string coming from the zeverlution home.cgi webpage
	bool ParseString(const std::string & str);
	bool FetchDataFromURL(const std::string & url, const unsigned int timeout);

	ZeverData();

	ZeverData(const std::string & rawData);
};
#endif
