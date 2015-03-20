#include "LibXenoverse.h"

int main(int argc, char** argv) {
	LibXenoverse::initializeDebuggingLog();

	LibXenoverse::BCS *bcs = new LibXenoverse::BCS();
	bcs->load("GOK.bcs");
	getchar();
	return 0;
}