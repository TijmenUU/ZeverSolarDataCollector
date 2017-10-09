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
	std::string registeryID, registeryKey, hardwareVersion, softwareVersion;
	std::string timeValue, dateValue, zeverCloudStatus;
	int number3;
	std::string inverterSN;
	int currentPower; // Watts
	double powerToday; // KWh
	std::string OKclass, ERRORclass;

	std::string errorMsg;

public:
	bool IsValid() const { return isValid; }
	int GetCurrentPower() const { return currentPower; }
	double GetCumulativePower() const { return powerToday; }
	std::string GetErrorMsg() const { return errorMsg; }

	std::string GetOutputStr(const std::time_t & timestamp,
		const unsigned int collumnPadding = 8U) const;
	bool ParseString(const std::string & str);
	bool FetchDataFromURL(const std::string & url, const unsigned int timeout);

	ZeverData();

	ZeverData(const std::string & rawData);
};
#endif
