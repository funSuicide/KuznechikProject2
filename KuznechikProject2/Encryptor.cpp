#include "Encryptor.h"
#include "Kuznechik.h"
#include <fstream>
#include <chrono>
#include <iostream>
#include <time.h>

Encryptor::Encryptor(std::string& pathOne, std::string& pathTwo, std::string& pathKey) {
	this->pathOne = pathOne;
	this->pathTwo = pathTwo;
	this->pathKey = pathKey;
}

void Encryptor::encrypt() const {
	char tmpKey[32];
	std::ifstream keyF(pathKey, std::ios::binary);
	keyF.read(tmpKey, 32);
	keyF.close();
	key a;
	std::copy((uint8_t*)(tmpKey), (uint8_t*)(tmpKey + 31), a.bytes);
	Kuznechik C(a);
	std::ifstream inEnd(pathOne, std::ios::binary);
	inEnd.seekg(0, std::ios::end);
	size_t sizeFile = inEnd.tellg();
	inEnd.close();
	size_t countMegabytes = sizeFile / megabyte;
	std::ifstream in(pathOne, std::ios::binary);
	std::ofstream out(pathTwo, std::ios::binary);
	for (size_t i = 0; i < countMegabytes; i++) {
		char* tmp = new char[megabyte];
		in.read(tmp, megabyte);
		byteVector blocks[64];
		byteVector result[64];
		int iter = 0;
		for (size_t j = 0; j < 64; ++j) {
			blocks[j] = byteVector((uint8_t*)tmp + iter);
			iter += 16;
		}
		auto begin = std::chrono::steady_clock::now();
		C.encryptText(blocks, result, 64, i);
		auto end = std::chrono::steady_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
		std::cout << elapsed_ms << std::endl;
		iter = 0;
		for (size_t j = 0; j < 64; ++j) {
			std::copy_n(result[j].bytes, 16, tmp + iter);
			iter += 16;
		}
		out.write((const char*)tmp, megabyte);
		delete[] tmp;
	}
	in.close();
	out.close();
}


