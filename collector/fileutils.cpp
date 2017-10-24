#include "fileutils.hpp"
#if defined _WIN32
#include <Windows.h>

#elif defined __linux
#include <sys/types.h> // include before stat.h
#include <sys/stat.h> // stat
#include <fcntl.h> // open
#include <unistd.h> // close

#else
#error Platform not supported
#endif

namespace FileUtils
{
#if defined _WIN32
	bool Exists(const std::string & path)
	{
		DWORD fileAttributes;
		try
		{
			fileAttributes = ::GetFileAttributes(path.c_str());
		}
		catch (...)
		{
			return false;
		}
		return (fileAttributes != INVALID_FILE_ATTRIBUTES);
	}

#undef CreateDirectory
	bool CreateDirectory(const std::string & dirpath,
		const unsigned int dirPerm)
	{
		return ::CreateDirectoryA(dirpath.c_str(), nullptr);
	}

#undef CreateFile
	bool CreateFile(const std::string & filePath,
		const unsigned int filePerm)
	{
		HANDLE result = ::CreateFileA(filePath.c_str(),
			GENERIC_WRITE,
			FILE_SHARE_READ,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);

		if(result == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		return ::CloseHandle(result);
	}

#elif defined __linux
	// Path can be a directory or file
	bool Exists(const std::string & path)
	{
		struct stat st = {0};
		if(stat(path.c_str(), &st) == 0)
		{
			return true;
		}
		return false;
	}

	// Default permission is 755 (rwxr-xr-x)
	bool CreateDirectory(const std::string & dirpath,
		const unsigned int dirPerm)
	{
		if(mkdir(dirpath.c_str(), dirPerm) == 0)
		{
			return true;
		}
		return false;
	}

	bool CreateFile(const std::string & filePath,
		const unsigned int filePerm)
	{
		int fileDescriptor = open(filePath.c_str(), O_RDWR | O_CREAT, filePerm);
		if(fileDescriptor == -1)
		{
			return false;
		}
		close(fileDescriptor);
		return true;
	}
#endif
}
