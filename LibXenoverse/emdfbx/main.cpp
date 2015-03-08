#include "LibXenoverse.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: emdfbx model.emd skeleton.esk output.fbx\n       Can include multiple emd and esk files into one fbx.");
		getchar();
		return 1;
	}

	LibXenoverse::initializeDebuggingLog();

	vector<string> model_filenames;
	vector<string> skeleton_filenames;
	string export_filename = "";

	for (int i = 1; i < argc; i++)  {
		string parameter = ToString(argv[i]);

		string extension = LibXenoverse::extensionFromFilename(parameter);

		if (extension == "emd") {
			model_filenames.push_back(parameter);
		}

		if (extension == "esk") {
			skeleton_filenames.push_back(parameter);
		}

		if (extension == "fbx") {
			export_filename = parameter;
		}
	}

	if (!export_filename.size()) {
		if (model_filenames.size()) {
			export_filename = model_filenames[0] + ".fbx";
		}
		else if (skeleton_filenames.size()) {
			export_filename = skeleton_filenames[0] + ".fbx";
		}
		else {
			export_filename = "Out.fbx";
		}
	}

	// Create FBX Manager
	FbxManager *sdk_manager = FbxManager::Create();
	FbxIOSettings *ios = FbxIOSettings::Create(sdk_manager, IOSROOT);
	ios->SetBoolProp(EXP_FBX_EMBEDDED, true);
	sdk_manager->SetIOSettings(ios);

	// Create Scene
	vector<FbxNode *> global_fbx_bones;
	global_fbx_bones.reserve(300);

	FbxScene *scene = FbxScene::Create(sdk_manager, "EMDFBXScene");

	for (size_t i = 0; i < skeleton_filenames.size(); i++) {
		LibXenoverse::ESK *esk_skeleton = new LibXenoverse::ESK(skeleton_filenames[i]);
		vector<FbxNode *> fbx_bones = esk_skeleton->createFBXSkeleton(scene);

		for (size_t j = 0; j < fbx_bones.size(); j++) {
			global_fbx_bones.push_back(fbx_bones[j]);
		}
	}

	for (size_t i = 0; i < model_filenames.size(); i++) {
		string node_name = LibXenoverse::nameFromFilenameNoExtension(model_filenames[i]);
		LibXenoverse::EMD *emd_model = new LibXenoverse::EMD(model_filenames[i]);

		// Fill Scene
		FbxNode *lMeshNode = NULL;

		// Create Node
		lMeshNode = FbxNode::Create(scene, node_name.c_str());
		lMeshNode->LclTranslation.Set(FbxVector4(0, 0, 0));
		lMeshNode->LclScaling.Set(FbxVector4(1, 1, 1));
		lMeshNode->LclRotation.Set(FbxVector4(0, 0, 0));

		// Create and attach Mesh
		FbxMesh *lMesh = emd_model->exportFBX(scene, lMeshNode);
		lMeshNode->SetNodeAttribute(lMesh);

		if (global_fbx_bones.size()) {
			emd_model->exportFBXSkin(scene, lMesh, global_fbx_bones, lMeshNode->EvaluateGlobalTransform());
		}

		// Add node to scene
		FbxNode *lRootNode = scene->GetRootNode();
		lRootNode->AddChild(lMeshNode);

	}

	// Export FBX
	int lFileFormat = sdk_manager->GetIOPluginRegistry()->GetNativeWriterFormat();
	FbxExporter* lExporter = FbxExporter::Create(sdk_manager, "");
	bool lExportStatus = lExporter->Initialize(export_filename.c_str(), lFileFormat, sdk_manager->GetIOSettings());

	if (!lExportStatus) {
		printf("Call to FbxExporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
		return 1;
	}

	scene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eMax);
	scene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);
	

	// Export scene
	lExporter->Export(scene);
	lExporter->Destroy();
	return 0;
}