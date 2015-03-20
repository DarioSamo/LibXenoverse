#include "LibXenoverse.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: hlslasm file\n");
		getchar();
		return 1;
	}

	LibXenoverse::initializeDebuggingLog();

	string shader_name = ToString(argv[1]);
	string folder = LibXenoverse::folderFromFilename(shader_name);
	string name = LibXenoverse::nameFromFilenameNoExtension(shader_name, true);
	string extension = LibXenoverse::extensionFromFilename(shader_name, true);

	if ((extension == "xpu") || (extension == "xvu")) {
		size_t data_size = 0;
		char *data = LibXenoverse::HLSLASM::disassembleFromFile(shader_name, data_size);
		if (data) {
			string new_filename = shader_name + ".asm";
			LibXenoverse::writeTextTo(new_filename, data, data_size);
			delete data;
		}
	}
	else {
		size_t data_size = 0;
		char *data = LibXenoverse::HLSLASM::assembleFromFile(shader_name, data_size);
		if (data) {
			string new_filename = folder + name;
			LibXenoverse::writeBytesTo(new_filename, data, data_size);
		}
	}

	return 0;
}