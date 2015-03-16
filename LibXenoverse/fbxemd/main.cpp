#include "LibXenoverse.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: fbxemd file.fbx\n");
		getchar();
		return 1;
	}

	LibXenoverse::initializeDebuggingLog();

	// Create FBX SDK
	FbxManager *sdk_manager = FbxManager::Create();
	FbxIOSettings *ios = FbxIOSettings::Create(sdk_manager, IOSROOT);
	ios->SetBoolProp(EXP_FBX_EMBEDDED, true);
	sdk_manager->SetIOSettings(ios);

	string fbx_filename = ToString(argv[1]);
	string extension = LibXenoverse::extensionFromFilename(fbx_filename);

	// Import FBX
	FbxImporter* lImporter = FbxImporter::Create(sdk_manager, "");
	// Import
	bool lImportStatus = lImporter->Initialize(fbx_filename.c_str(), -1, sdk_manager->GetIOSettings());
	if (!lImportStatus) {
		// Handle error
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		return NULL;
	}
	FbxScene* lScene = FbxScene::Create(sdk_manager, "FBXEMDImport");
	lImporter->Import(lScene);
	int lFileMajor, lFileMinor, lFileRevision;
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
	lImporter->Destroy();

	string folder = LibXenoverse::folderFromFilename(fbx_filename);

	// Search Scene
	const int lNodeCount = lScene->GetSrcObjectCount<FbxNode>();
	FbxStatus lStatus;
	for (int lIndex = 0; lIndex < lNodeCount; lIndex++) {
		FbxNode *lNode = lScene->GetSrcObject<FbxNode>(lIndex);

		if (lNode) {
			printf("Node found: %s\n", lNode->GetName());

			FbxMesh *lMesh = lNode->GetMesh();
			if (lMesh) {
				printf("Mesh found\n");

				LibXenoverse::EMD *emd_model = new LibXenoverse::EMD();
				if (emd_model) {
					emd_model->importFBX(lNode);
					emd_model->save(folder + lNode->GetName() + ".emd");
					delete emd_model;
				}
			}
		}
	}

	lScene->Destroy();
	return 0;
}