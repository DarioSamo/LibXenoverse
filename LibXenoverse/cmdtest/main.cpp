#include "LibXenoverse.h"

int main(int argc, char** argv) {
	LibXenoverse::initializeDebuggingLog();

	LibXenoverse::EMZ *emz_pack = new LibXenoverse::EMZ();
	emz_pack->load("shader_age_ps.emz");
	emz_pack->saveUncompressed("shader_age_ps.emb");
	return 0;
}