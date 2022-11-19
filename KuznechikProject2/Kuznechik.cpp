#include "Kuznechik.h"
#include <omp.h>

byteVector Kuznechik::transformationS(const byteVector& src) {
	byteVector tmp;
	for (size_t i = 0; i < 16; i++) {
		tmp.bytes[i] = sTable[src.bytes[i]];
	}
	return tmp;
}

uint8_t Kuznechik::multiplicationGalua(uint8_t first, uint8_t second) {
	uint8_t result = 0;
	uint8_t hiBit;
	for (int i = 0; i < 8;i++) {
		if (second & 1) {
			result ^= first;
		}
		hiBit = first & 0x80;
		first <<= 1;
		if (hiBit) {
			first ^= 0xc3;
		}
		second >>= 1;
	}
	return result;
}

void Kuznechik::transformationR(byteVector& src) {
	uint8_t a_15 = 0;
	byteVector internal;
	for (int i = 15; i >= 0; i--) {
		if (i - 1 >= 0) {
			internal.bytes[i - 1] = src.bytes[i];
		}
		a_15 ^= multiplicationGalua(src.bytes[i], lVector[i]);
	}
	internal.bytes[15] = a_15;
	std::copy(internal.bytes, internal.bytes + 16, src.bytes);
}

void Kuznechik::transformaionL(uint8_t* in_data, uint8_t* out_data) {
	byteVector internal;
	std::copy(in_data, in_data + 16, internal.bytes);
	for (int i = 0; i < 16; i++) {
		transformationR(internal);
	}
	std::copy(internal.bytes, internal.bytes + 16, out_data);
}

void Kuznechik::getConstTable() {
	byteVector numberIter;
	numberIter.bytes[0] += 0x01;
	for (int i = 0; i < 32; i++) {
		byteVector result;
		transformaionL(numberIter.bytes, result.bytes);
		constTable[i] = result;
		numberIter.bytes[0] += 0x01;
	}
}

void Kuznechik::printConstTable() const {
	for (size_t j = 0; j < 32; j++) {
		for (size_t q = 0; q < 16; q++) {
			printf("%02x", constTable[j].bytes[q]);
		}
		std::cout << std::endl;
	}
}

void Kuznechik::getRoundKeys(const key& mainKey) {
	uint8_t left[16];
	uint8_t right[16];
	std::copy(mainKey.bytes, mainKey.bytes + 16, left);
	std::copy(mainKey.bytes + 16, mainKey.bytes + 32, right);
	byteVector leftPart(left);
	byteVector rightPart(right);
	roundKeys[0] = leftPart;
	roundKeys[1] = rightPart;
	for (size_t i = 0; i < 8; i++) {
		int iter = 0;
		for (size_t j = 0; j < 8; j++) {
			leftPart = rightPart;
			byteVector tmp = transformationF(rightPart, constTable[iter]);
			rightPart = xOR(tmp, leftPart);
			iter++;
		}
		roundKeys[2 + i] = leftPart;
		roundKeys[2 + i + 1] = rightPart;
	}
}

byteVector Kuznechik::transformationF(const byteVector& src, const byteVector& cons) {
	byteVector tmp;
	tmp = xOR(src, cons);
	transformationS(tmp);
	byteVector d;
	transformaionL(tmp.bytes, d.bytes);
	return d;
}

byteVector Kuznechik::xOR(const byteVector& src1, const byteVector& src2) const {
	halfVector left = src1.halfs[0].half ^ src2.halfs[0].half;
	halfVector right = src1.halfs[1].half ^ src2.halfs[1].half;
	return byteVector(left, right);
}

byteVector Kuznechik::encryptBlock(const byteVector& block) const {
	byteVector copyBLock = block;
	for (size_t i = 0; i < 9; i++) {
		copyBLock = xOR(copyBLock, roundKeys[i]);
		byteVector tmp;
		for (size_t j = 0; j < 16; j++) {
			tmp = xOR(tmp, startByteT[j][copyBLock.bytes[j]]);
		}
		copyBLock = xOR(tmp, copyBLock);
	}
	copyBLock = xOR(copyBLock, roundKeys[9]);
	return copyBLock;
}

void Kuznechik::getStartTable() {
	for (int j = 0; j < 16; j++) {
		byteVector tmp;
		for (int i = 0; i < 256; i++) {
			byteVector c = transformationS(tmp);
			byteVector d;
			transformaionL(c.bytes, d.bytes);
			startByteT[j][i] = d;
			tmp.bytes[j] += 0x01;
		}
	}
}

void Kuznechik::encryptText(const byteVector* data, byteVector* dest, const int size, const int iV) const{
	#pragma omp parallel for num_threads(4)
	for (int i = 0; i < size; ++i) {
		halfVector left = (uint32_t)(iV + i);
		halfVector right = (uint32_t)(iV + i);
		byteVector block(left, right);
		byteVector chiperBlock = encryptBlock(block);
		block = data[i];
		block = xOR(block, chiperBlock);
		dest[i] = block;
	}
}