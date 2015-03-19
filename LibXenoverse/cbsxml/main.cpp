#include "LibXenoverse.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: cbsxml file.cbs\n");
		getchar();
		return 1;
	}

	LibXenoverse::initializeDebuggingLog();

	string cbs_filename = ToString(argv[1]);
	string extension = LibXenoverse::extensionFromFilename(cbs_filename);

	if (extension == "cbs.xml") {
		string new_name = cbs_filename;
		new_name.resize(new_name.size() - 4);

		LibXenoverse::CBS *cbs_table = new LibXenoverse::CBS();
		cbs_table->load(cbs_filename);
		cbs_table->save(new_name);
	}
	else if (extension == "cbs") {
		LibXenoverse::CBS *cbs_table = new LibXenoverse::CBS();
		cbs_table->load(cbs_filename);
		cbs_table->saveXML(cbs_filename + ".xml");
	}

	
	return 0;
}