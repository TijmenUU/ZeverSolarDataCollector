#pragma once

#include <ctime>
#include <string>

struct Configuration
{
	std::string fetchURL;
	unsigned int fetchTimoutMs;
	std::string datetime;

	Configuration()
		: fetchURL(),
		fetchTimoutMs(1000),
		datetime()
	{}
};
