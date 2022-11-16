#include <iostream>
#include <fstream>

class Encryptor {
	std::string pathOne;
	std::string pathTwo;
	std::string pathKey;
	int megabyte = 1048576;
public:
	Encryptor(std::string& pathOne, std::string& pathTwo, std::string& pathKey);
	void encrypt() const;
};


