#include "parameters.hpp"
#include "zeverdata.hpp"

#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>

int main(int argc, char ** argv)
{
	Configuration config;
	if(!UnpackParameters(argv, argc, config))
	{
		return -1;
	}

	ZeverData zeverData;
	if(!zeverData.FetchDataFromURL(config.fetchURL, config.fetchTimoutMs))
	{
		return -1;
	}

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2);
	if(config.datetime.size() > 0)
	{
		ss << config.datetime;
	}
	ss << std::setw(8) << zeverData.currentPower;
	ss << std::setw(8) << zeverData.powerToday;

	std::puts(ss.str().c_str());

	return 0;
}
