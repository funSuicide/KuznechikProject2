#include <string>
#include "Kuznechik.h"
#include <chrono>
#include "Encryptor.h"
#include <string_view>
#include <algorithm>

void test(std::string_view path1, size_t size) {
	std::fstream output(path1.data(), std::ios::binary | std::ios::out);
	std::vector<char> data;
	data.reserve(size);
	std::generate_n(std::back_inserter(data), size, rand);
	output.write(data.data(), data.size());
}

int main() {
	std::string key = "key.txt";
	std::string path1 = "test.txt";
	std::string path2 = "out_2.txt";
	std::string path3 = "out2_2.txt";

	//test(path1, 128*1024 * 1024);

	Encryptor C(path1, path2, key);
	C.encrypt();
	Encryptor D(path2, path3, key);
	D.encrypt();
}