/*
	Name: File.cpp
	Date: October 26 2018
	Description: Implementation of file.h
*/
#include "file.h"


void File::processData()
{
	float_t sum = 0.0;
	for (matchFormat::const_iterator ext = _matchedFormat.begin(); ext != _matchedFormat.end(); ++ext)
		sum += ext->second.second;
	for (matchFormat::const_iterator ext = _matchedFormat.begin(); ext != _matchedFormat.end(); ++ext)
		_data.push_back(std::make_pair(ext->first, 100 * (ext->second.second / sum)));
}

