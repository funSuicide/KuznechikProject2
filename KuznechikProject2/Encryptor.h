#pragma once
#include <iostream>
#include <fstream>
#include "Kuznechik.h"
#define MEGABYTE 1048576

class Encryptor {
	std::string pathToInputText;
	std::string pathToOutputText;
	Kuznechik cryptoAlgorithm;
public:
	Encryptor(const std::string& pathOne, const std::string& pathTwo, const std::string& pathKey);
	void encrypt() const;
};


