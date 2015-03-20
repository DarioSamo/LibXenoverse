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
	string name = LibXenoverse::nameFromFilenameNoExtension(pack_name, true);

	if (pack_name.find(".emz") != string::npos) {
		LibXenoverse::EMZ *emz_pack = new LibXenoverse::EMZ();
		emz_pack->load(pack_name);
		string new_extension = emz_pack->detectNewExtension();
		string new_filename = folder + name + new_extension;
		emz_pack->saveUncompressed(new_filename);
		delete emz_pack;
	}
	else {
		size_t data_size = 0;
		char *data = LibXenoverse::getBytesFrom(pack_name, data_size);

		if (data) {
			LibXenoverse::EMZ *emz_pack = new LibXenoverse::EMZ();
			emz_pack->setDataPtr((unsigned char *)data, data_size);
			string new_filename = folder + name + ".emz";
			emz_pack->save(new_filename);
			delete emz_pack;
		}
	}

	return 0;
}