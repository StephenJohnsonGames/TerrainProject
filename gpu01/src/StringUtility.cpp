#include <pch.h>
#include "StringUtility.hpp"

using namespace std;

vector<string> StringUtility::splitPath(const string& path, const set<char>& delimiters) {

	vector<string> result;

	char const* charPtr = path.c_str();
	char const* startChar = charPtr;

	while (*charPtr) {

		if (delimiters.find(*charPtr) != delimiters.end())
		{
			if (startChar != charPtr) {

				string pathComponent(startChar, charPtr);
				result.push_back(pathComponent);
			}
			else {

				result.push_back("");
			}

			startChar = charPtr + 1;
		}

		++charPtr;
	}

	result.push_back(startChar);

	return result;
}


string StringUtility::loadStringFromFile(const string& filePath) {
	
	ifstream shaderFile(filePath);

	if (!shaderFile.is_open()) {

		throw StringUtility::StringResult::S_FILE_NOT_FOUND;
	}

	// Get file size and allocate buffer
	streampos startIndex = shaderFile.tellg();
	shaderFile.seekg(0, ios::end);
	streampos endIndex = shaderFile.tellg();

	auto bufferSize = endIndex - startIndex;

	char* src = (char*)calloc(bufferSize + 1, 1); // Ensure buffer ends will null terminator character

	if (!src) {

		shaderFile.close();
		throw StringUtility::StringResult::S_BUFFER_ALLOC_ERROR;
	}

	shaderFile.seekg(0, ios::beg);
	shaderFile.read(src, bufferSize); // Read file into c string
	
	string sourceString(src);

	free(src);
	shaderFile.close();

	return sourceString;
}