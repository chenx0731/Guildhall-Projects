#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

typedef std::vector<std::string> Strings;

//-----------------------------------------------------------------------------------------------
const std::string Stringf( char const* format, ... );
const std::string Stringf( int maxLength, char const* format, ... );

Strings SplitStringOnDelimiter(std::string const& originalString, std::string delimiterToSplitOn);
// Ignore delimiters inside double quotes
Strings SplitStringWithQuotes(const std::string& originalString, std::string delimiterToSplitOn);
// Removes any occurrence of delimiter from the front and back of the string
void TrimString(std::string& originalString, std::string delimiterToTrim);

bool AreStringsEqualCaseInsensitive(const std::string& stringA, const std::string& stringB);
bool AreStringsEqualCaseSensitive(const std::string& stringA, const std::string& stringB);