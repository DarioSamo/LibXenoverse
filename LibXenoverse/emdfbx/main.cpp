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
	vector<string> animation_filenames;
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

		if (extension == "ean") {
			animation_filenames.push_back(parameter);
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
	vector<size_t> global_fbx_bones_index;													//TODO find a better way to link skeleton and animations
	global_fbx_bones_index.reserve(300);

	vector<FbxAnimCurveNode *> global_fbx_animation;
	global_fbx_animation.reserve(300);

	FbxScene *scene = FbxScene::Create(sdk_manager, "EMDFBXScene");


	// Load Shaders and convert it to fx file (will be use by fbx).
	vector<string> shader_names;
	shader_names.push_back("adam_shader/shader_age_vs.emb");								//must specified vs folloxed by ps shaders
	shader_names.push_back("adam_shader/shader_age_ps.emb");
	shader_names.push_back("adam_shader/shader_default_vs.emb");
	shader_names.push_back("adam_shader/shader_default_ps.emb");
	

	bool needs_install_shaders = false;
	for (size_t i = 0; i < shader_names.size(); i++) {
		if (!LibXenoverse::fileCheck(shader_names[i])) {
			needs_install_shaders = true;
			break;
		}
	}

	if (needs_install_shaders) {
		printf("Shaders not found. Please use Xenoviewer to prepare shaders in bin folder.");
		return -1;
	}

	for (size_t i = 0; i+1 < shader_names.size(); i+=2) {
		
		LibXenoverse::EMB *shader_pack_vs = new LibXenoverse::EMB();
		LibXenoverse::EMB *shader_pack_ps = new LibXenoverse::EMB();

		if (!shader_pack_vs->load(shader_names[i])) {
			delete shader_pack_vs;
			printf("Couldn't load Shader Pack %s. File is either missing, open by another application, or corrupt.", shader_names[i].c_str());
			continue;
		}
		if (!shader_pack_ps->load(shader_names[i+1])) {
			delete shader_pack_vs;
			delete shader_pack_ps;
			printf("Couldn't load Shader Pack %s. File is either missing, open by another application, or corrupt.", shader_names[i].c_str());
			continue;
		}

		shader_pack_vs->exportShadersToFx(shader_pack_vs, shader_pack_ps);					//convert all shaders in fx file with defaults program parameters (like Ogre's version).
	}



	//build FBX skeleton
	for (size_t i = 0; i < skeleton_filenames.size(); i++) {
		LibXenoverse::ESK *esk_skeleton = new LibXenoverse::ESK();
		esk_skeleton->load(skeleton_filenames[i]);
		vector<FbxNode *> fbx_bones = esk_skeleton->createFBXSkeleton(scene);

		for (size_t j = 0; j < fbx_bones.size(); j++) {
			global_fbx_bones.push_back(fbx_bones[j]);
			global_fbx_bones_index.push_back(j);											//kepp index of bone to link with animations.
		}
	}



	//build FBX animations
	for (size_t i = 0; i < animation_filenames.size(); i++) {
		
		string filename = animation_filenames.at(i);
		string ean_name = LibXenoverse::nameFromFilenameNoExtension(filename, true);

		//vector<FbxAnimCurveNode *> global_fbx_animation;

		LibXenoverse::EAN *animation = new LibXenoverse::EAN();
		if (animation->load(filename)) {

			std::vector<FbxAnimStack *> list_AnimStack;
			size_t nbAnims = animation->getAnimations().size();
			for (size_t j = 0; j < nbAnims; j++) {													//we create only one stack and one layer by animation. each will animate all bones of all skeleton.

				LibXenoverse::EANAnimation *anim_tmp = &(animation->getAnimations().at(j));

				FbxAnimStack* lAnimStack = FbxAnimStack::Create(scene, anim_tmp->getName().c_str());
				FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(scene, (anim_tmp->getName() + "_Layer0").c_str());
				lAnimStack->AddMember(lAnimLayer);

				list_AnimStack.push_back(lAnimStack);
			}

			size_t k = 0;
			for (vector<FbxNode *>::iterator it = global_fbx_bones.begin(); it != global_fbx_bones.end(); it++) {

				vector<FbxAnimCurveNode *> fbx_anim = animation->exportFBXAnimations(scene, list_AnimStack, *it, global_fbx_bones_index.at(k));

				for (size_t j = 0; j < fbx_anim.size(); j++) {
					global_fbx_animation.push_back(fbx_anim[j]);
				}
				k++;
			}
		}
		else {
			delete animation;
			animation = NULL;
		}
	}




	for (size_t i = 0; i < model_filenames.size(); i++) {
		string node_name = LibXenoverse::nameFromFilenameNoExtension(model_filenames[i]);
		string path = model_filenames[i].substr(0, model_filenames[i].size() - LibXenoverse::nameFromFilename(model_filenames[i]).size());

		LibXenoverse::EMD *emd_model = new LibXenoverse::EMD();
		emd_model->load(model_filenames[i]);

		// Fill Scene
		FbxNode *lMeshNode = NULL;

		// Create Node
		lMeshNode = FbxNode::Create(scene, node_name.c_str());
		lMeshNode->LclTranslation.Set(FbxVector4(0, 0, 0));
		lMeshNode->LclScaling.Set(FbxVector4(1, 1, 1));
		lMeshNode->LclRotation.Set(FbxVector4(0, 0, 0));

		// Create and attach Mesh
		FbxMesh *lMesh = emd_model->exportFBX(scene, lMeshNode, path);
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