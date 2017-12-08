#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include <string>

/*
	A small helper class that abstracts the Windows and Linux file API
	differences. The permissions supplied in the functions are thus not used
	on Windows platforms, but are there for easy compatability between the
	platform specific implementations
*/

namespace FileUtils
{
	// Path can be a directory or file
	bool Exists(const std::string & path);

	// Default permission is 755 (rwxr-xr-x)
	bool CreateDirectory(const std::string & dirpath,
		const unsigned int dirPerm = 0755);

	bool CreateFile(const std::string & filePath,
		const unsigned int filePerm = 0644);

	bool DeleteFile(const std::string & filepath);
}

#endif
