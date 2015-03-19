#include "LibXenoverse.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: agdxml file.emm\n");
		getchar();
		return 1;
	}

	LibXenoverse::initializeDebuggingLog();

	string agd_filename = ToString(argv[1]);
	string extension = LibXenoverse::extensionFromFilename(agd_filename);

	if (extension == "agd.xml") {
		string new_name = agd_filename;
		new_name.resize(new_name.size() - 4);

		LibXenoverse::AGD *agd_table = new LibXenoverse::AGD();
		agd_table->load(agd_filename);
		agd_table->save(new_name);
	}
	else if (extension == "agd") {
		LibXenoverse::AGD *agd_table = new LibXenoverse::AGD();
		agd_table->load(agd_filename);
		agd_table->saveXML(agd_filename + ".xml");
	}
	return 0;
}