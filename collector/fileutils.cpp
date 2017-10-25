#include "fileutils.hpp"
#if defined _WIN32
#include <Windows.h>

#elif defined __linux
#include <sys/types.h> // include before stat.h
#include <sys/stat.h> // stat
#include <fcntl.h> // open
#include <unistd.h> // close, unlink

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

// This is undoing the macro colliding with out name from Windows.h
#undef CreateDirectory
	bool CreateDirectory(const std::string & dirpath,
		const unsigned int dirPerm)
	{
		return ::CreateDirectoryA(dirpath.c_str(), nullptr);
	}

// This is undoing the macro colliding with out name from Windows.h
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

// This is undoing the macro colliding with out name from Windows.h
#undef DeleteFile
	bool DeleteFile(const std::string & filepath)
	{
		return DeleteFileA(filepath.c_str());
	}

#elif defined __linux
	// Path can be a directory or file
	bool Exists(const std::string & path)
	{
		struct stat st = {0};
		return stat(path.c_str(), &st) == 0;
	}

	// Default permission is 755 (rwxr-xr-x)
	bool CreateDirectory(const std::string & dirpath,
		const unsigned int dirPerm)
	{
		return mkdir(dirpath.c_str(), dirPerm) == 0;
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

	bool DeleteFile(const std::string & filepath)
	{
		return unlink(filepath.c_str()) == 0;
	}
#endif
}
