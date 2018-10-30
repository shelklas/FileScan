#include <boost/assert.hpp>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <locale>
#include <regex>
#include <tuple>

#include "tridlib.h"


using namespace std;
using namespace std::experimental::filesystem;

void scan(path const& f, trid * trd, std::vector<std::tuple<std::string, std::string, std::string>> &weirdFile)
{
	recursive_directory_iterator d(f), e;
	for (; d != e; ++d)
	{
		try
		{
			if (!is_directory(d->status()) && !std::experimental::filesystem::is_empty(d->path()))
			{
				std::string filepath = d->path().string();
			
				char * c_filepath = new char[filepath.size() + 1];
				std::copy(filepath.begin(), filepath.end(), c_filepath);

				trd->SubmitFileA(c_filepath);

				int ret = trd->Analyze();

				if (ret)
				{
					char buf[260];
					*buf = 0;

					ret = trd->GetInfo(TRID_GET_RES_NUM, 0, buf);
					if (ret == 0)
						continue;

					std::string str = d->path().extension().string().erase(0,1);
					std:transform(str.begin(), str.end(), str.begin(), ::toupper);

					bool isMatch = false;

					int largestMatch = 0;
					std::string fileType;

					for (int i = ret + 1; --i;)
					{					
						ret = trd->GetInfo(TRID_GET_RES_POINTS, i, buf);

						int currentMatch = trd->GetInfo(TRID_GET_RES_POINTS, i, buf);

						trd->GetInfo(TRID_GET_RES_FILEEXT, i, buf);

						if (largestMatch <= currentMatch)
						{
							fileType = buf;
							largestMatch = currentMatch;
						}

						if (str == buf) // Check extension to returned types
						{
							isMatch = true;
							break;
						}
					}
					//std::cout << isMatch << std::endl;
					if (!isMatch)
					{
						weirdFile.push_back(std::make_tuple(d->path().string(), fileType, to_string(largestMatch)));
					}
					
				}
			}
		}
		catch (filesystem_error & e)
		{
			std::cout << std::endl;
			std::cout << "FILESYSTEM ERROR: " << e.what() << std::endl;
		}
		catch (std::exception & e)
		{
			std::cout << std::endl;
			std::cout << "ERROR: " << e.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "UNRECOVERABLE ERROR" << std::endl;
		}
	}
}

void printStart()
{
	std::cout << R"(
______ _ _        _____                 
|  ___(_) |      /  ___|                
| |_   _| | ___  \ `--.  ___ __ _ _ __  
|  _| | | |/ _ \  `--. \/ __/ _` | '_ \ 
| |   | | |  __/ /\__/ / (_| (_| | | | |
\_|   |_|_|\___| \____/ \___\__,_|_| |_|        
)" << std::endl;

	std::cout << "Made by Sheldon Klassen - Utilizing TrID API" << std::endl;
	std::cout << "WARNING: Run this on small folders. \nLibrary has a tendancy to create blank files with different extensions. (For Now)" << std::endl;
	std::cout << std::endl;

}

void getWidths(std::vector<std::tuple<std::string, std::string, std::string>> const &weirdFile, size_t &longestPath, size_t &longestFile, size_t &longestMatch)
{
	for (std::vector<std::tuple<std::string, std::string, std::string>>::const_iterator it = weirdFile.begin(); it != weirdFile.end(); ++it)
	{
		size_t currentPathLength = std::get<0>(*it).length();
		size_t currentFileLength = std::get<1>(*it).length();
		size_t currentMatchLength = std::get<2>(*it).length();

		if (longestPath < currentPathLength)
			longestPath = currentPathLength;
		if (longestFile < currentFileLength)
			longestFile = currentFileLength;
		if (longestMatch < currentMatchLength)
			longestMatch = currentMatchLength;
	}
}

void printPaths(std::vector<std::tuple<std::string, std::string, std::string>> const &weirdFile)
{
	size_t longestPath = 0;
	size_t longestFile = 0;
	size_t longestMatch = 0;

	getWidths(weirdFile, longestPath, longestFile, longestMatch);
	
	std::cout << left  << setw(longestPath) << "PATH" << "    " << setw(longestFile) << "FILE" << "     " << setw(longestMatch) << "MATCH" << std::endl;
	std::cout << std::endl;
	for (std::vector<std::tuple<std::string, std::string, std::string>>::const_iterator it = weirdFile.begin(); it != weirdFile.end(); ++it)
		std::cout << left << setw(longestPath) << std::get<0>(*it) << "    " << setw(longestFile) << std::get<1>(*it) << "     " << setw(longestMatch) << std::get<2>(*it) << std::endl;
}

size_t getSizeOfFiles(std::vector<std::tuple<std::string, std::string, std::string>> const &weirdFile)
{
	size_t totalCount = 0;
	for (std::vector<std::tuple<std::string, std::string, std::string>>::const_iterator it = weirdFile.begin(); it != weirdFile.end(); ++it)
		totalCount += static_cast<size_t>(file_size(path(std::get<0>(*it))));

	return totalCount;
}

void copyFiles(std::vector<std::tuple<std::string, std::string, std::string>> const &weirdFile, path &saveFolder)
{
	
	for (std::vector<std::tuple<std::string, std::string, std::string>>::const_iterator it = weirdFile.begin(); it != weirdFile.end(); ++it)
		try {
			copy_file(path(std::get<0>(*it)), path(saveFolder.string().append("\\" + path(std::get<0>(*it)).filename().string())), copy_options(0));
		} catch (filesystem_error & e) {
			std::cout << "FILESYSTEM ERROR: " << e.what() << " ON FILE: " << std::get<0>(*it) << std::endl;
		}
}

int main(size_t argc, char *argv[])
{
	std::string input;				 // User input
	std::string filePath;			 // User input filepath
	std::string savePath;			 // User input save path
	trid * trd = new trid();		 // File extension Parser
	char * c_defLoc = new char('.'); // Location of TridLib.TRD file
	path currPath;					 // Search folder
	path currSavePath;	             // Save folder

	printStart();

	if (argc < 2 || argc > 3)
	{
		std::cout << "Bad Input!" << std::endl;
		return 0;
	}

	filePath = argv[1];
	currPath = path(filePath);

	if (argc == 3)
	{
		savePath = argv[2];
		currSavePath = path(savePath);
		if (!exists(currSavePath))
		{
			std::cout << "Bad Path!" << endl;
			return 0;
		}
	}

	if (!exists(currPath))
	{
		std::cout << "Bad Path!" << endl;
		return 0;
	}

	do
	{
		trd = new trid();
	} while (!trd->LoadDefsPack(c_defLoc)); // File doesn't like to load. This will force it

	assert("File Loaded");

	std::vector<std::tuple<std::string, std::string, std::string>> weirdFile;

	scan(currPath, trd, weirdFile);
	std::cout << std::endl;
	std::cout << "File Type Verification Failures: " << weirdFile.size() << std::endl;
	std::cout << std::endl;

	if (!weirdFile.empty())
		printPaths(weirdFile);
	else
		std::cout << "No Files Found." << std::endl;
	
	std::sort(weirdFile.begin(), weirdFile.end(), [](std::tuple<std::string, std::string, std::string> & lhs, std::tuple<std::string, std::string, std::string> & rhs) {return std::get<0>(lhs) < std::get<0>(rhs); });

	size_t totalFileSize = getSizeOfFiles(weirdFile);

	std::cout << std::endl;
	cout.imbue(locale(""));
	if (totalFileSize < 1024)
		std::cout << "There were " << weirdFile.size() << " Files found (" << totalFileSize << " B)" << std::endl;
	else if (totalFileSize < pow(1024, 2))
		std::cout << "There were " << weirdFile.size() << " Files found (" << (static_cast<float>(totalFileSize) / static_cast<float>(1024)) << " KiB)" << std::endl;
	else if (totalFileSize < pow(1024, 3))
		std::cout << "There were " << weirdFile.size() << " Files found (" << (static_cast<float>(totalFileSize) / static_cast<float>(pow(1024, 2))) << " MiB)" << std::endl;
	else
		std::cout << "There were " << weirdFile.size() << " Files found (" << (static_cast<float>(totalFileSize) / static_cast<float>(pow(1024, 3))) << " GiB)" << std::endl;

	if (argc == 3)
	{
		do
		{
			input = "";
			std::cout << "Copy to \"" << currSavePath.string() << "\"? (Y/N): ";
			std::getline(std::cin, input);
			std:transform(input.begin(), input.end(), input.begin(), ::toupper);
		} while (input != "Y" && input != "N");

		std::cout << endl;
		if (input == "Y")
			copyFiles(weirdFile, currSavePath);
	}

	std::cout << std::endl;
	trd->~trid();
}
