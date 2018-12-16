#include <nifparse/NIFFile.h>
#include <fstream>

int main(int argc, char *argv[]) {
	std::fstream nifFileStream;
	nifFileStream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
	nifFileStream.open("C:\\projects\\nifparse\\meshes\\ex_hlaalu_b_01.nif", std::ios::in | std::ios::binary);

	nifparse::NIFFile file;
	file.parse(nifFileStream);

	return 0;
}
