#pragma once

#include <string>

#include <sys/types.h> // include before stat.h
#include <sys/stat.h> // stat
#include <fcntl.h> // open
#include <unistd.h> // close, unlink

namespace FileUtils
{
	// Path can be a directory or file
	bool Exists(const std::string & path)
	{
		struct stat st = {0};
		return stat(path.c_str(), &st) == 0;
	}

	// Default permission is 755 (rwxr-xr-x)
	bool CreateDirectory(const std::string & dirpath,
		const unsigned int dirPerm = 755)
	{
		return mkdir(dirpath.c_str(), dirPerm) == 0;
	}

	bool CreateFile(const std::string & filePath,
		const unsigned int filePerm = 644)
	{
		int fileDescriptor = open(filePath.c_str(), O_RDWR | O_CREAT, filePerm);
		if(fileDescriptor == -1)
		{
			return false;
		}
		close(fileDescriptor);
		return true;
	}
}
