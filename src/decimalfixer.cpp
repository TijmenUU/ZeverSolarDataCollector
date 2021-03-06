#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <utility> // swap
#include <vector>

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
			if(line.size() > 1)
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
			file << content[i] << '\n';
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
bool DecimalFix(std::vector<std::string> & content)
{
	if(content.size() < 2)
	{
		return false;
	}

	bool madeChange = false;
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
			madeChange = true;
			// 0.10 becomes 0.01
		}
	}

	return madeChange;
}

int main(int argc, char ** argv)
{
	std::string fileToFix;
	if(argc < 2)
	{
		std::fputs("Expected a filepath to the file to be checked as launch parameter.", stderr);
		return -1;
	}
	else
	{
		fileToFix = argv[1];
	}

	std::vector<std::string> fileContents;
	if(GetFileContents(fileToFix, fileContents))
	{
		if(DecimalFix(fileContents) && !WriteFileContents(fileToFix, fileContents))
		{
			std::fprintf(stderr, "Could not write to file <%s>!\n", fileToFix.c_str());
			return -1;
		}
	}
	else
	{
		std::fprintf(stderr, "Could not read file <%s>!\n", fileToFix.c_str());
		return -1;
	}

	return 0;
}
