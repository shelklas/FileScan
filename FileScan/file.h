#pragma once

/*
	Sheldon Klassen
	October 26 2018

	File.h
	
	Object that contains file matches for incorrect extension
*/
#include <boost/assert.hpp>
#include <filesystem>
#include <utility>
#include <vector>
#include <string>
#include <map>

using matchFormat = std::map<std::string, std::pair<size_t,size_t>>; // Holds unique records of extensions and matches
using matchData = std::vector<std::pair<std::string, float_t>>;
class File
{
public:
	std::experimental::filesystem::path  _path;
	size_t _size;
	matchData _data;
	matchFormat  _matchedFormat;	// Vector< filetype, matches >

	// Constructors
	File() {}

	// Desctructors
	~File() {}

	// Methods

	/*
		Name: AddMatch
		Parameters: String Extension, Integer FileMatch
		Returns: Nothing!
		Description: Sets Values of _matchedFormat. This is all the findings that the library can find
	*/
	inline void addMatch(std::string ext, size_t match)
	{ 
		++_matchedFormat[ext].first;		  // Increment times found extension
		_matchedFormat[ext].second += match;  // Sum values of matches for extension
	}

	/*
		Name: setPercentage
		Parameters: Nothing!
		Returns: Nothing!
		Description: Calculates the percentage of likelyhood of file type
	*/
	void processData();
};