#ifndef TIMEUTILS_HPP
#define TIMEUTILS_HPP

#include <ctime>
#include <string>

/*
	This is a small set of functions that help with formatting time
*/

namespace TimeUtils
{
	// Creates a string using the ctime function strftime. This is then resized
	// and converted to a string object. The length argument specifies the
	// buffer size to be passed to strftime, not the resulting length
	std::string GetFormattedTimeStr(const std::time_t & timestamp,
		const char * format,
		const size_t length = 64U);

	std::string GetIsoDateTimeStr(const std::time_t & timestamp);
}

#endif
