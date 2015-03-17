#include "LibXenoverse.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: emzpack file\n");
		getchar();
		return 1;
	}

	LibXenoverse::initializeDebuggingLog();

	string pack_name = ToString(argv[1]);
	string folder = LibXenoverse::folderFromFilename(pack_name);
	string name = LibXenoverse::nameFromFilenameNoExtension(pack_name);

	if (pack_name.find(".emz") != string::npos) {
		LibXenoverse::EMZ *emz_pack = new LibXenoverse::EMZ(pack_name);
		string new_extension = emz_pack->detectNewExtension();
		string new_filename = folder + name + new_extension;
		emz_pack->saveUncompressed(new_filename);
		delete emz_pack;
	}
	else {
	}

	return 0;
}