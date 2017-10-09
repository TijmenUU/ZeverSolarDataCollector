#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP
#pragma once

#include <string>

namespace FileUtils
{
	// Path can be a directory or file
	bool Exists(const std::string & path);

	// Default permission is 755 (rwxr-xr-x)
	bool CreateDirectory(const std::string & dirpath,
		const unsigned int dirPerm = 0755);

	bool CreateFile(const std::string & filePath,
		const unsigned int filePerm = 0644);
}

#endif
