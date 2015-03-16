#include "LibXenoverse.h"

int main(int argc, char** argv) {
	LibXenoverse::initializeDebuggingLog();

	LibXenoverse::EMD *emd_model = new LibXenoverse::EMD("FR4_000_Bust.emd");
	emd_model->save("FR4_000_Bust.emd.new");
	return 0;
}