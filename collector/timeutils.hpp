#ifndef TIMEUTILS_HPP
#define TIMEUTILS_HPP
#pragma once

#include <ctime>
#include <string>

namespace TimeUtils
{
	// Length does not equal the format necessarily! String will not be equal
	// to length necessarily
	std::string GetFormattedTimeStr(const std::time_t & timestamp,
		const char * format,
		const size_t length = 64U)
	{
		tm * timeInfo;
		char buffer[length];
		timeInfo = localtime(&_time);
		strftime(buffer, length, format, timeInfo);

		std::string result;
		result.assign(buffer);

		return result;
	}

	std::string GetIsoDateTimeStr(const std::time_t & timestamp)
	{
		return GetFormattedTimeStr(timestamp,"%Y-%m-%dT%H:%M:%S");
	}
}

#endif
