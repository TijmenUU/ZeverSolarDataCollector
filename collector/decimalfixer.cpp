#include "timeutils.hpp"
#include "configuration.hpp"

#include <ctime>
#include <fstream>
#include <iostream> // DEBUG
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility> // swap
#include <vector>

const unsigned int cDayInUnixTime = 86400;

// Appends the contents of the file to the out_content vector line by line
bool GetFileContents(const std::string & filepath,
	std::vector<std::string> & out_content)
{
	std::ifstream file;
	file.open(filepath, std::fstream::in);
	if(file.is_open())
	{
		out_content.clear();

		std::string line;
		while(std::getline(file, line))
		{
			out_content.push_back(line);
			line.clear();
		}

		file.close();

		return true;
	}

	return false;
}

// Writes to filepath the lines of content seperated by std::endl
bool WriteFileContents(const std::string filepath,
	const std::vector<std::string> & content)
{
	std::ofstream file;
	file.open(filepath, std::fstream::out | std::fstream::trunc);
	if(file.is_open())
	{
		for(unsigned int i = 0; i < content.size(); ++i)
		{
			file << content[i] << std::endl;
		}

		file.close();

		return true;
	}

	return false;
}

// Looks for decimal errors in the logged kilowatts hour that fit the pattern:
// x.1, x.2, ... x.9 x.1 (a random decrease)
// The zeverlution inverter omits the leading zero for values with only a
// nonzero value in the second decimal place. This causes 1.01 to become 1.1,
// 3.05 to become 3.5, etcetera
// We cannot reliably fix data ending with this bug, since we have no
// guarrantees about the interval or maximum power production over time
// Neither can we fix low interval logs, logs with large kwh increments, or a
// combination of this.
void DecimalFix(std::vector<std::string> & content)
{
	if(content.size() < 2)
	{
		return;
	}

	for(unsigned int i = content.size() - 1; i > 0; --i)
	{
		std::string & last = content[i];
		std::string & first = content[i - 1];
		// If the first decimal of the later value is small than the one before it
		// 3.1 < 3.9 => so 3.9 should be 3.09
		// 4.1 < 3.9 => do nothing
		if(last[last.size() - 4] == first[first.size() - 4] &&
			last[last.size() - 2] < first[first.size() - 2])
		{
			// fix it by swapping the two decimal values of the wrong value
			std::swap(first[first.size() - 1], first[first.size() - 2]);
			// 0.10 becomes 0.01
		}
	}
}

int main(int argc, char ** argv)
{
	Configuration config;
	if(argc > 1)
	{
		config.LoadFromFile(argv[1]);
	}
	else
	{
		config.LoadFromFile("collecting.conf");
	}

	if(!config.IsValid())
	{
		throw std::runtime_error(config.GetErrorMsg());
	}

	// Get yesterday's timestamp
	const std::time_t timestamp = time(nullptr) - cDayInUnixTime;
	const std::string fileToFix = config.GetArchiveFilePath(timestamp);

	std::vector<std::string> fileContents;
	if(GetFileContents(fileToFix, fileContents))
	{
		DecimalFix(fileContents);
	}
	else
	{
		throw std::runtime_error("Could not read file " + fileToFix);
	}

	if(!WriteFileContents(fileToFix, fileContents))
	{
		throw std::runtime_error("Could not write to file " + fileToFix);
	}
}
