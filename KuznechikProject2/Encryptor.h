#include <iostream>
#include <fstream>
#define MEGABYTE 1048576

class Encryptor {
	std::string pathToInputText;
	std::string pathToOutputText;
public:
	Kuznechik cryptoAlgorithm;
	Encryptor(const std::string& pathOne, const std::string& pathTwo, const std::string& pathKey);
	void encrypt() const;
};


