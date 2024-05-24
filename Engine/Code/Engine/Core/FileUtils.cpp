#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Window.hpp"
#include <stdio.h>
#include <filesystem>
#include <Windows.h>

int FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& filename)
{
	// open file
	int res = 0;
	FILE* readFile = nullptr;
	res = fopen_s(&readFile, filename.c_str(), "rb");
	if (readFile == nullptr) {
		ERROR_AND_DIE(Stringf("Can't read file %s", filename.c_str()));
	}

	// obtain size of the file
	long size;
	fseek(readFile, 0, SEEK_END);
	size = ftell(readFile);
	rewind(readFile);
	outBuffer.resize(size);

	// read file
	size_t readSize;
	readSize = fread(outBuffer.data(), 1, size, readFile);
	if (readSize != size) {
		ERROR_AND_DIE(Stringf("Reading error: read file %s", filename.c_str()));
	}

	// close
	fclose(readFile);

	return 0;
}

int FileReadToString(std::string& outString, const std::string& filename)
{
	std::vector<uint8_t> buffer;
	FileReadToBuffer(buffer, filename);
	std::string tempString = std::string((char*)buffer.data(), buffer.size());
	outString = tempString;
	return 0;
}

int WriteToFile(std::vector<uint8_t> const& buffer, const std::string& filename)
{
	FILE* pFile = nullptr;
	std::filesystem::path filePath = filename;
	std::filesystem::path directoryPath = filePath.remove_filename();
	if (!std::filesystem::exists(directoryPath)) {
		std::filesystem::create_directory(directoryPath);
	}
	if (fopen_s(&pFile, filename.c_str(), "wb") == 0) {
		fwrite(buffer.data(), 1, buffer.size(), pFile);
		fclose(pFile);
	}
	else
	{
		ERROR_AND_DIE(Stringf("Error: can't open file %s", filename.c_str()));
	}
	return 0;
}

bool IsFileExist(const std::string& filename)
{
	int res = 0;
	FILE* readFile = nullptr;
	res = fopen_s(&readFile, filename.c_str(), "rb");
	if (readFile == nullptr) {
		return false;
	}
	// close
	fclose(readFile);
	return true;
}

