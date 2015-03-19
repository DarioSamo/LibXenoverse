#include "LibXenoverse.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: cusxml file.cus\n");
		getchar();
		return 1;
	}

	LibXenoverse::initializeDebuggingLog();

	string cus_filename = ToString(argv[1]);
	string extension = LibXenoverse::extensionFromFilename(cus_filename);

	if (extension == "cus.xml") {
		string new_name = cus_filename;
		new_name.resize(new_name.size() - 4);

		LibXenoverse::CUS *cus_table = new LibXenoverse::CUS();
		cus_table->load(cus_filename);
		cus_table->save(new_name);
	}
	else if (extension == "cus") {
		LibXenoverse::CUS *cus_table = new LibXenoverse::CUS();
		cus_table->load(cus_filename);
		cus_table->saveXML(cus_filename + ".xml");
	}
	return 0;
}