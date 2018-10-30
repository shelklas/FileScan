#include <boost/assert.hpp>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <memory>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <locale>
#include <regex>
#include <tuple>

#include "tridlib.h"
#include "file.h"


using namespace std;
using namespace std::experimental::filesystem;

using file_container = std::vector<File>;

void scan(path const& f, trid * trd, std::vector<File> &files)
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
					if (ret == 0) // If no matches in Library for file extension
						continue;
					File fle; // Initialize object
					fle._path = filepath;
					fle._size = std::experimental::filesystem::file_size(filepath);
					std::string str = d->path().extension().string().erase(0,1);
					std:transform(str.begin(), str.end(), str.begin(), ::toupper);
					bool isMatch = false; // Defines if match is found for correct extension
					for (int i = ret + 1; --i;)
					{					
						ret = trd->GetInfo(TRID_GET_RES_POINTS, i, buf);
						int matchPoints = trd->GetInfo(TRID_GET_RES_POINTS, i, buf);
						trd->GetInfo(TRID_GET_RES_FILEEXT, i, buf);
						if(buf != "")
							fle.addMatch(buf, matchPoints);
						if (str == buf) // Check extension to returned types
							isMatch = true;
					}
					//std::cout << isMatch << std::endl;
					if (!isMatch)
					{
						fle.processData();
						files.push_back(fle);
					}
				}
			}
		}
		catch (filesystem_error & e)
		{
			std::cout << "FILESYSTEM ERROR: " << e.what() << std::endl;
			std::cout << "FILE: " << d->path().string() << std::endl;
			std::cout << std::endl;
		}
		catch (std::exception & e)
		{
			std::cout << "ERROR: " << e.what() << std::endl;
			std::cout << "FILE: " << d->path().string() << std::endl;
			std::cout << std::endl;
		}
		catch (...)
		{
			std::cout << "UNRECOVERABLE ERROR" << std::endl;
			std::cout << "FILE: " << d->path().string() << std::endl;
			std::cout << std::endl;
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

void getWidths(file_container const &files, size_t &longestPath, size_t &longestFile, size_t &longestMatch)
{
	size_t currentPathLength;
	size_t currentFileSize;
	size_t currentMatchLength;
	size_t currentFileLength;
	for (file_container::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		currentPathLength = it->_path.string().size();
		currentFileSize = std::to_string(it->_size).size();
		if (longestPath < currentPathLength)
			longestPath = currentPathLength;
		for (matchData::const_iterator i = it->_data.begin(); i < it->_data.end(); ++i)
		{
			currentMatchLength = std::to_string(i->second).size();
			currentFileLength = i->first.size();
			if (longestMatch < currentMatchLength)
				longestMatch = currentMatchLength;
			if (longestFile < currentFileLength)
				longestFile = currentFileLength;
		}
	}
}

void printPaths(file_container &files)
{
	size_t longestPath = 0;
	size_t longestFile = 0;
	size_t longestMatch = 0;
	getWidths(files, longestPath, longestFile, longestMatch);
	std::cout << left  << setw(longestPath) << "PATH" << "    " << setw(longestFile) << "EXT" << "  " << "MATCH"  << std::endl;
	std::cout << std::endl;
	for (file_container::iterator it = files.begin(); it != files.end(); ++it)
	{
		for (matchData::iterator ext = it->_data.begin(); ext != it->_data.end(); ++ext)
			std::cout << left << setw(longestPath) << it->_path << "    " << setw(longestFile) << ext->first << "  " << ext->second << "%" << std::endl;
		std::cout << std::endl;
	}
}

size_t getSizeOfFiles(file_container const &files)
{
	size_t totalCount = 0;
	for (file_container::const_iterator it = files.begin(); it != files.end(); ++it)
		totalCount += it->_size;
	return totalCount;
}

void copyFiles(file_container const &files, path &saveFolder)
{
	
	for (file_container::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		path filePath = it->_path;
		try 
		{
			// Generate a path with an appended filename to keep everything within the folder structure
			copy_file(filePath, path(saveFolder.string().append("\\" + filePath.filename().string())), copy_options(0));
		}
		catch (filesystem_error & e) {
			std::cout << "FILESYSTEM ERROR: " << e.what() << " ON FILE: " << it->_path << std::endl;
		}
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
		std::cout << "Bad Path? Nah, bad you!" << endl;
		return 0;
	}

	do
	{
		trd = new trid();
	} while (!trd->LoadDefsPack(c_defLoc)); // File doesn't like to load. This will force it

	assert("File Loaded");

	std::vector<File> files;

	scan(currPath, trd, files);
	std::cout << std::endl;
	std::cout << "File Type Verification Failures: " << files.size() << std::endl;
	std::cout << std::endl;

	if (!files.empty())
		printPaths(files);
	else
		std::cout << "No Files Found." << std::endl;
	
	//std::sort(files.begin(), files.end(), [](std::tuple<std::string, std::string, std::string> & lhs, std::tuple<std::string, std::string, std::string> & rhs) {return std::get<0>(lhs) < std::get<0>(rhs); });

	size_t totalFileSize = getSizeOfFiles(files);

	std::cout << std::endl;
	cout.imbue(locale(""));
	if (totalFileSize < 1024)
		std::cout << "There were " << files.size() << " Files found (" << totalFileSize << " B)" << std::endl;
	else if (totalFileSize < pow(1024, 2))
		std::cout << "There were " << files.size() << " Files found (" << (static_cast<float>(totalFileSize) / static_cast<float>(1024)) << " KiB)" << std::endl;
	else if (totalFileSize < pow(1024, 3))
		std::cout << "There were " << files.size() << " Files found (" << (static_cast<float>(totalFileSize) / static_cast<float>(pow(1024, 2))) << " MiB)" << std::endl;
	else
		std::cout << "There were " << files.size() << " Files found (" << (static_cast<float>(totalFileSize) / static_cast<float>(pow(1024, 3))) << " GiB)" << std::endl;

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
			copyFiles(files, currSavePath);
	}

	std::cout << std::endl;
	trd->~trid();
}
