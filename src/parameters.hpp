#pragma once

#include "configuration.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <string>

enum class ParameterType
{
	Unknown = -1,
	FetchURL = 0,
	FetchTimeOut,
	DateString
};

namespace detail
{
	// Single letter arguments (-u, -t, etc.)
	const std::string shortArguments("utd");
	// Long form arguments (--outputpath, etc.)
	const std::array<std::string, 4> longArguments = {
		"fetch-url",
		"fetch-timeout",
		"date-string"
	};

	ParameterType ToEnum(const char argument)
	{
		for(size_t i = 0; i < shortArguments.size(); ++i)
		{
			if(argument == shortArguments[i])
				return static_cast<ParameterType>(i);
		}

		return ParameterType::Unknown;
	}

	ParameterType ToEnum(const std::string & argument)
	{
		for(size_t i = 0; i < longArguments.size(); ++i)
		{
			const auto & longArgument = longArguments[i];
			if(longArgument.size() > argument.size())
				continue;
			
			bool match = true;
			for(size_t j = 0; j < argument.size(); ++j)
			{
				if(argument[j] != longArgument[j])
				{
					match = false;
					break;
				}
			}
			
			if(match)
				return static_cast<ParameterType>(i);
		}

		return ParameterType::Unknown;
	}
}

bool UnpackParameters(char ** argv, const int argc, Configuration & out_config)
{
	// skip the first argument, which should be our own exec filepath
	for(int i = 1; i < argc; ++i)
	{
		const std::string argument(argv[i]);
		if(argument.size() < 2)
			continue;
		
		if(argument[0] == '-')
		{
			size_t argSize = 0; // size of the argument string, to check if argument is concatenated to it
			ParameterType t = ParameterType::Unknown;
			if(argument[1] == '-')
			{
				argSize += 2; // two leading '-'
				t = detail::ToEnum(argument.substr(2, argument.size()));
				
				if(t != ParameterType::Unknown)
					argSize += detail::longArguments[static_cast<int>(t)].size();
			}
			else
			{
				argSize += 2; // one for '-' and one for the single arg char
				t = detail::ToEnum(argument[1]);
			}

			if(t == ParameterType::Unknown)
			{
				std::fprintf(stderr, "Unknown parameter <%s>.\n", argument.c_str());
				return false;
			}

			std::string value;
			if(argument.size() > argSize)
			{
				// value is concatenated to the parameter
				// e.g. -o/home/user/log
				value = argument.substr(argSize, argument.size());
			}
			else
			{
				++i;
				if(i >= argc)
					break;
				
				value = argv[i];
			}

			switch(t)
			{
				case ParameterType::FetchURL:
				out_config.fetchURL = value;
				break;

				case ParameterType::FetchTimeOut:
				try
				{
					out_config.fetchTimoutMs = std::abs(std::stoi(value));
				}
				catch(std::invalid_argument & e)
				{
					std::fprintf(stderr, "Fetch timeout parameter value <%s> could not be converted to an unsigned integer!\n", value.c_str());
					return false;
				}
				catch(std::out_of_range & e)
				{
					std::fprintf(stderr, "Fetch timeout parameter value <%s> is too big to be converted to an unsigned integer!\n", value.c_str());
					return false;
				}
				break;

				case ParameterType::DateString:
				out_config.datetime = value;
				break;

				default:
				break;
			}
		}
		else
		{
			std::fprintf(stderr, "Unknown purpose for argument <%s>.\n", argument.c_str());
			return false;
		}
	}

	// Check whether out_config is "minimally" complete
	if(out_config.fetchURL.size() == 0)
	{
		std::fputs("Missing URL paremeter to fetch data from! (-u [URL])", stderr);
		return false;
	}

	return true;
}