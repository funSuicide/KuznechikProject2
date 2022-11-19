#include "Encryptor.h"
#include <fstream>
#include <chrono>
#include <iostream>
#include <time.h>

Encryptor::Encryptor(const std::string& pathToInputText, const std::string& pathToOutputText, const std::string& pathKey) {
	char tmpKey[32];
	key masterKey;
	std::ifstream keyF(pathKey, std::ios::binary);
	keyF.read(tmpKey, 32);
	keyF.close();
	std::copy((uint8_t*)(tmpKey), (uint8_t*)(tmpKey + 31), masterKey.bytes);
	this->cryptoAlgorithm = Kuznechik(masterKey);
	this->pathToInputText = pathToInputText;
	this->pathToOutputText = pathToOutputText;
}

void Encryptor::encrypt() const {
	std::ifstream in(pathToInputText, std::ios::binary);
	in.seekg(0, std::ios::end);
	size_t sizeFile = in.tellg();
	size_t countMegabytes = sizeFile / MEGABYTE;
	in.seekg(0, std::ios::beg);
	std::ofstream out(pathToOutputText, std::ios::binary);
	byteVector* buffer = new byteVector[65536];
	byteVector* result = new byteVector[65536];
	for (size_t i = 0; i < countMegabytes; i++) {
		for (size_t j = 0; j < 65536; ++j) {
			in.read((char*)buffer[j].bytes, 16);
		}
		auto begin = std::chrono::steady_clock::now();
		this->cryptoAlgorithm.encryptText(buffer, result, 65536, i);
		auto end = std::chrono::steady_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
		std::cout << elapsed_ms << std::endl;
		for (size_t j = 0; j < 65536; ++j) {
			out.write((const char*)result[j].bytes, 16);
		}
	}
	in.close();
	out.close();
}


