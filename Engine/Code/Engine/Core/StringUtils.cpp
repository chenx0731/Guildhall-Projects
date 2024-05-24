#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
constexpr int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( char const* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( int maxLength, char const* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

Strings SplitStringOnDelimiter(std::string const& originalString, std::string delimiterToSplitOn)
{
	size_t pos_start = 0;
	size_t pos_end;
	size_t delim_len = delimiterToSplitOn.length();
	std::string token;
	Strings result;

	while ((pos_end = originalString.find(delimiterToSplitOn, pos_start)) != std::string::npos) {
		token = originalString.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		result.push_back(token);
	}
	result.push_back(originalString.substr(pos_start));
	return result;
}

Strings SplitStringWithQuotes(const std::string& originalString, std::string delimiterToSplitOn)
{
	size_t pos_start = 0;
	size_t pos_end;
	size_t next_start = 0;
	size_t pos_quote_end;
	size_t delim_len = delimiterToSplitOn.length();
	std::string token;
	Strings result;
	std::string quote = "\"";
	std::vector<size_t> quotes;
	while ((pos_quote_end = originalString.find(quote, pos_start)) != std::string::npos) {
		pos_start = pos_quote_end + 1;
		quotes.push_back(pos_quote_end);
	}
	pos_start = 0;
	while ((pos_end = originalString.find(delimiterToSplitOn, pos_start)) != std::string::npos) {
		bool flag_start = true;
		bool flag_end = true;
		for (int i = 1; i < (int)quotes.size(); i+=2) {
			if (pos_start < quotes[i] && pos_start > quotes[i - 1]) {
				flag_start = false;
				//break;
			}
			if (pos_end < quotes[i] && pos_end > quotes[i - 1]) {
				flag_end = false;
			}
		}
		if (flag_start) {
			next_start = pos_start;
		}
		pos_start = pos_end + delim_len;
		if (flag_start && flag_end) {
			//next_start = pos_start;
			token = originalString.substr(next_start, pos_end - next_start);
			result.push_back(token);
		}
		
	}
	bool flag_start = true;
	for (int i = 1; i < (int)quotes.size(); i += 2) {
		if (pos_start < quotes[i] && pos_start > quotes[i - 1]) {
			flag_start = false;
		}

	}
	if (flag_start) {
		next_start = pos_start;
	}
	result.push_back(originalString.substr(next_start));
	return result;
}

void TrimString(std::string& originalString, std::string delimiterToTrim)
{
	size_t pos_start = 0;
	size_t pos_end;
	size_t delim_len = delimiterToTrim.length();
	std::string token;
	std::string result;

	while ((pos_end = originalString.find(delimiterToTrim, pos_start)) != std::string::npos) {
		token = originalString.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		//result.push_back(token);
		result += token;
	}
	result += originalString.substr(pos_start);
	originalString = result;
}

bool AreStringsEqualCaseInsensitive(const std::string& stringA, const std::string& stringB)
{
	int result = _strcmpi(stringA.c_str(), stringB.c_str());
	if (result == 0)
		return true;
	else return false;
}

bool AreStringsEqualCaseSensitive(const std::string& stringA, const std::string& stringB)
{
	int result = strcmp(stringA.c_str(), stringB.c_str());
	if (result == 0)
		return true;
	else return false;
}





