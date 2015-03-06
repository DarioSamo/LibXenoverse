#include "LibXenoverse.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: emmxml file.emm\n");
		getchar();
		return 1;
	}

	string material_filename = ToString(argv[1]);
	string material_name = LibXenoverse::nameFromFilenameNoExtension(material_filename);
	string extension = LibXenoverse::extensionFromFilename(material_filename);

	if (extension == "emm.xml") {
		string new_name = material_filename;
		new_name.resize(new_name.size() - 4);

		LibXenoverse::EMM *emm_material = new LibXenoverse::EMM(material_filename);
		emm_material->save(new_name);
	}
	else if (extension == "emm") {
		LibXenoverse::EMM *emm_material = new LibXenoverse::EMM(material_filename);
		emm_material->saveXML(material_filename + ".xml");
	}
	return 0;
}