#include "LibXenoverse.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: cbsxml file.emm\n");
		getchar();
		return 1;
	}

	string cbs_filename = ToString(argv[1]);
	string extension = LibXenoverse::extensionFromFilename(cbs_filename);

	if (extension == "cbs.xml") {
		string new_name = cbs_filename;
		new_name.resize(new_name.size() - 4);

		LibXenoverse::CBS *emm_material = new LibXenoverse::CBS(cbs_filename);
		emm_material->save(new_name);
	}
	else if (extension == "cbs") {
		LibXenoverse::CBS *emm_material = new LibXenoverse::CBS(cbs_filename);
		emm_material->saveXML(cbs_filename + ".xml");
	}
	return 0;
}