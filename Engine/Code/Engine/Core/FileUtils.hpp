#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <string>
#include <vector>

int FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& filename);
int FileReadToString(std::string& outString, const std::string& filename);
int WriteToFile(std::vector<uint8_t> const& buffer, const std::string& filename);
bool IsFileExist(const std::string& filename);