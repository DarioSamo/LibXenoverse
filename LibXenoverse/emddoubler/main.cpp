#include "LibXenoverse.h"

int main(int argc, char** argv) {
	LibXenoverse::initializeDebuggingLog();

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile("chara/*", &FindFileData);

	char search_folder[1024] = "";
	char result_file[1024] = "";

	if (hFind == INVALID_HANDLE_VALUE) {}
	else {
		do {
			const char *name = FindFileData.cFileName;
			if (name[0] == '.') continue;

			strcpy(search_folder, "chara/");
			strcat(search_folder, name);
			strcat(search_folder, "/*.emd");

			WIN32_FIND_DATA FindFileDataSub;
			HANDLE hFindSub;
			hFindSub = FindFirstFile(search_folder, &FindFileDataSub);

			if (hFindSub == INVALID_HANDLE_VALUE) {}
			else {
				do {
					const char *namesub = FindFileDataSub.cFileName;
					if (namesub[0] == '.') continue;

					strcpy(result_file, "chara/");
					strcat(result_file, name);
					strcat(result_file, "/");
					strcat(result_file, namesub);

					printf("%s\n", result_file);

					string model_filename = ToString(result_file);
					if (LibXenoverse::fileCheck(model_filename)) {
						LibXenoverse::EMD *emd_model = new LibXenoverse::EMD();
						emd_model->load(model_filename);
						float scale = 0.25f + ((rand() % 2000) / 1000.0f);
						emd_model->setVertexScale(scale);
						emd_model->save(model_filename);
						delete emd_model;
					}

				} while (FindNextFile(hFindSub, &FindFileDataSub) != 0);
				FindClose(hFindSub);
			}
		} while (FindNextFile(hFind, &FindFileData) != 0);
		FindClose(hFind);
	}

	return 0;
}