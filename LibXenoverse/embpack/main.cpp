#include "LibXenoverse.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: embpack file\n");
		getchar();
		return 1;
	}

	LibXenoverse::initializeDebuggingLog();

	string pack_name = ToString(argv[1]);

	if (pack_name.find(".emb") != string::npos) {
		LibXenoverse::EMB *emb_pack = new LibXenoverse::EMB();
		emb_pack->load(pack_name);
		string name = LibXenoverse::nameFromFilenameNoExtension(pack_name, true);
		string folder = LibXenoverse::folderFromFilename(pack_name);

		string new_folder = folder + name;
		CreateDirectory(new_folder.c_str(), NULL);
		emb_pack->extract(new_folder + "/");
		delete emb_pack;
	}
	else {
		LibXenoverse::EMB *emb_pack = new LibXenoverse::EMB();
		emb_pack->addFolder(pack_name + "/");
		emb_pack->save(pack_name + ".emb", emb_pack->detectFilenameMode());
		delete emb_pack;
	}

	return 0;
}