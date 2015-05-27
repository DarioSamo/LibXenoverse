
//#include "fbxsdk/core/arch/fbxtypes.h"

namespace LibXenoverse {
	unsigned int EMDSubmesh::getTotalPointCount() {
		return vertices.size();
	}

	unsigned int EMDMesh::getTotalPointCount() {
		unsigned int total_point_count = 0;
		for (size_t i = 0; i < submeshes.size(); i++) {
			total_point_count += submeshes[i]->getTotalPointCount();
		}
		return total_point_count;
	}

	unsigned int EMDModel::getTotalPointCount() {
		unsigned int total_point_count = 0;
		for (size_t i = 0; i < meshes.size(); i++) {
			total_point_count += meshes[i]->getTotalPointCount();
		}
		return total_point_count;
	}

	unsigned int EMD::getTotalPointCount() {
		unsigned int total_point_count = 0;
		for (size_t i = 0; i < models.size(); i++) {
			total_point_count += models[i]->getTotalPointCount();
		}
		return total_point_count;
	}

	void EMDSubmesh::getMaterialNames(vector<string> &material_names) {
		bool found = false;
		for (size_t i = 0; i < material_names.size(); i++) {
			if (material_names[i] == name) {
				found = true;
				fbx_material_index = i;
				break;
			}
		}

		if (!found) {
			fbx_material_index = material_names.size();
			material_names.push_back(name);
		}
	}

	void EMDMesh::getMaterialNames(vector<string> &material_names) {
		for (size_t i = 0; i < submeshes.size(); i++) {
			submeshes[i]->getMaterialNames(material_names);
		}
	}

	void EMDModel::getMaterialNames(vector<string> &material_names) {
		for (size_t i = 0; i < meshes.size(); i++) {
			meshes[i]->getMaterialNames(material_names);
		}
	}

	vector<string> EMD::getMaterialNames() {
		vector<string> material_names;
		for (size_t i = 0; i < models.size(); i++) {
			models[i]->getMaterialNames(material_names);
		}
		return material_names;
	}

#ifdef LIBXENOVERSE_FBX_SUPPORT
	void EMDTriangles::exportFBXSkin(vector<FbxCluster *> &skin_clusters, vector<EMDVertex> &vertex_pool, unsigned int &control_point_base) {
		vector<unsigned int> bone_fbx_index_map;
		bone_fbx_index_map.resize(bone_names.size());

		for (size_t b = 0; b < bone_names.size(); b++) {
			string &bone_name = bone_names[b];
			for (size_t i = 0; i < skin_clusters.size(); i++) {
				if (bone_name == string(skin_clusters[i]->GetName())) {
					bone_fbx_index_map[b] = i;
					break;
				}
			}
		}

		for (size_t i = 0; i < faces.size(); i += 3) {
			for (size_t k = 0; k < 3; k++) {
				unsigned short vertex_index = faces[i + k];
				EMDVertex &v = vertex_pool[vertex_index];

				for (size_t j = 0; j < 4; j++) {
					if (v.bone_weight[j] > 0.0f) {
						unsigned char bone_index = v.bone[3 - j];

						if (bone_index < bone_fbx_index_map.size()) {
							size_t skin_cluster_index = bone_fbx_index_map[bone_index];
							if (skin_cluster_index < skin_clusters.size()) {
								skin_clusters[skin_cluster_index]->AddControlPointIndex(control_point_base + vertex_index, v.bone_weight[j]);
							}
							else {
								printf("Invalid Skin Cluster Index %d\n", skin_cluster_index);
								getchar();
							}
						}
						else {
							printf("Invalid Bone Index %d compared to Bone FBX Index Map size %d\n", bone_index, bone_fbx_index_map.size());
							getchar();
						}
					}
				}
			}
		}
	}

	void EMDSubmesh::exportFBXSkin(vector<FbxCluster *> &skin_clusters, unsigned int &control_point_base) {
		for (size_t i = 0; i < triangles.size(); i++) {
			triangles[i].exportFBXSkin(skin_clusters, vertices, control_point_base);
		}

		control_point_base += vertices.size();
	}

	void EMDMesh::exportFBXSkin(vector<FbxCluster *> &skin_clusters, unsigned int &control_point_base) {
		for (size_t i = 0; i < submeshes.size(); i++) {
			submeshes[i]->exportFBXSkin(skin_clusters, control_point_base);
		}
	}

	void EMDModel::exportFBXSkin(vector<FbxCluster *> &skin_clusters, unsigned int &control_point_base) {
		for (size_t i = 0; i < meshes.size(); i++) {
			meshes[i]->exportFBXSkin(skin_clusters, control_point_base);
		}
	}

	void EMD::exportFBXSkin(FbxScene *scene, FbxMesh *fbx_mesh, vector<FbxNode *> &fbx_bones, FbxAMatrix skin_matrix) {
		FbxSkin* lSkin = FbxSkin::Create(scene, "");
		vector<FbxCluster *> skin_clusters;
		skin_clusters.resize(fbx_bones.size());

		for (size_t b = 0; b < fbx_bones.size(); b++) {
			// Create skin cluster
			FbxCluster *lCluster = FbxCluster::Create(scene, fbx_bones[b]->GetName());
			lCluster->SetLink(fbx_bones[b]);
			lCluster->SetLinkMode(FbxCluster::eTotalOne);
			lCluster->SetTransformMatrix(skin_matrix);
			FbxAMatrix lXMatrix = fbx_bones[b]->EvaluateGlobalTransform();
			lCluster->SetTransformLinkMatrix(lXMatrix);
			lSkin->AddCluster(lCluster);

			skin_clusters[b] = lCluster;
		}

		unsigned int control_point_base = 0;
		for (size_t i = 0; i < models.size(); i++) {
			models[i]->exportFBXSkin(skin_clusters, control_point_base);
		}

		fbx_mesh->AddDeformer(lSkin);
	}

	void EMDTriangles::exportFBX(FbxMesh *lMesh, unsigned int &control_point_base, int fbx_material_index) {
		for (size_t i = 0; i < faces.size(); i += 3) {
			lMesh->BeginPolygon(fbx_material_index);
			lMesh->AddPolygon(control_point_base + faces[i]);
			lMesh->AddPolygon(control_point_base + faces[i + 1]);
			lMesh->AddPolygon(control_point_base + faces[i + 2]);
			lMesh->EndPolygon();
		}
	}

	void EMDSubmesh::exportFBX(FbxMesh *lMesh, unsigned int &control_point_base, FbxGeometryElementNormal *lGeometryElementNormal, FbxGeometryElementUV *lGeometryUVElement) {
		FbxVector4 *lControlPoints = lMesh->GetControlPoints();
		
		for (size_t i = 0; i < vertices.size(); i++) {
			EMDVertex &v = vertices[i];
			lControlPoints[control_point_base+i] = FbxVector4(v.x, v.y, v.z);
			lGeometryElementNormal->GetDirectArray().Add(FbxVector4(v.nx, v.ny, v.nz));
			lGeometryUVElement->GetDirectArray().Add(FbxVector2(v.u, 1.0 - v.v));
		}

		for (size_t i = 0; i < triangles.size(); i++) {
			triangles[i].exportFBX(lMesh, control_point_base, fbx_material_index);
		}

		control_point_base += vertices.size();
	}

	void EMDMesh::exportFBX(FbxMesh *lMesh, unsigned int &control_point_base, FbxGeometryElementNormal *lGeometryElementNormal, FbxGeometryElementUV *lGeometryUVElement) {
		for (size_t i = 0; i < submeshes.size(); i++) {
			submeshes[i]->exportFBX(lMesh, control_point_base, lGeometryElementNormal, lGeometryUVElement);
		}
	}

	void EMDModel::exportFBX(FbxMesh *lMesh, unsigned int &control_point_base, FbxGeometryElementNormal *lGeometryElementNormal, FbxGeometryElementUV *lGeometryUVElement) {
		for (size_t i = 0; i < meshes.size(); i++) {
			meshes[i]->exportFBX(lMesh, control_point_base, lGeometryElementNormal, lGeometryUVElement);
		}
	}


	void CreateTableEntry(FbxBindingTable* pTable, FbxProperty& pProp)
	{
		// create entry
		FbxBindingTableEntry& lEntry = pTable->AddNewEntry();
		// set src to the fbx property, in this sample, fbx properties have the same name with shader parameters
		FbxPropertyEntryView lSrc(&lEntry, true, true);
		// Because CgFX uses compound property, so do not use pProp.GetName()
		lSrc.SetProperty(pProp.GetHierarchicalName());

		// set dst to the shader parameter
		FbxSemanticEntryView lDst(&lEntry, false, true);
		lDst.SetSemantic(pProp.GetName());
	}


	void CustomParameterProp(FbxSurfaceMaterial* lMaterial, FbxBindingTable* lTable, string paramName, FbxDouble4 val)
	{
		FbxProperty lProp = FbxProperty::Create(lMaterial, FbxDouble4DT, paramName.c_str(), paramName.c_str());
		lProp.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
		lProp.Set(val);
		CreateTableEntry(lTable, lProp);
	}



	void CustomTextureProp(FbxScene *scene, FbxSurfaceMaterial* lMaterial, FbxBindingTable* lTable, string paramName, string filename)
	{
		FbxProperty lProp = FbxProperty::Create(lMaterial, FbxDouble3DT, paramName.c_str(), paramName.c_str());
		lProp.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
		FbxDouble3 lMapVal(0, 1, 0);
		lProp.Set(lMapVal);
		CreateTableEntry(lTable, lProp);

		FbxFileTexture* lTexture = FbxFileTexture::Create(scene, paramName.c_str());
		if (filename.size() != 0)
		{
			if (LibXenoverse::fileCheck(filename))
				lTexture->SetFileName(filename.c_str());
			else
				printf("Image %s not found", filename.c_str());
		}
		lTexture->SetTextureUse(FbxTexture::eStandard);
		lTexture->SetMappingType(FbxTexture::eUV);
		lTexture->ConnectDstProperty(lProp);
	}


	FbxSurfaceMaterial *EMD::exportFBXMaterial(FbxScene *scene, string material_name, EMM *material, EMB *texture_pack, EMB *texture_dyt_pack) {
		
		FbxSurfaceMaterial* lMaterial = NULL;

		FbxString lMaterialName = material_name.c_str();
		FbxString lShadingName = "Phong";
		FbxDouble3 lBlack(0.0, 0.0, 0.0);
		FbxDouble3 lDiffuseColor(1.0, 1.0, 1.0);

		
		

		size_t isFound = (size_t)-1;
		if (material != NULL)
		{
			vector<EMMMaterial *> listMaterial = material->getMaterials();
			for (size_t i = 0; i < listMaterial.size(); i++) {							//serach for material definition
				if (listMaterial.at(i)->getName() == material_name)
				{
					isFound = i;
					break;
				}
			}
		}


		if ((material == NULL) || (isFound==(size_t)-1))					//default case if there is a problem.
		{
			printf("DBX Material %s not found. Put a default material in fbx.", material_name.c_str());
			
			FbxSurfacePhong* lMaterial = FbxSurfacePhong::Create(scene, lMaterialName.Buffer());

			// Generate primary and secondary colors.
			lMaterial->Emissive.Set(lBlack);
			lMaterial->Ambient.Set(lBlack);
			lMaterial->AmbientFactor.Set(1.0);
			// Add texture for diffuse channel
			lMaterial->Diffuse.Set(lDiffuseColor);
			lMaterial->DiffuseFactor.Set(1.0);
			lMaterial->ShadingModel.Set(lShadingName);
			lMaterial->Shininess.Set(0.0);
			lMaterial->Specular.Set(lBlack);
			lMaterial->SpecularFactor.Set(0.0);

			return (FbxSurfaceMaterial*)lMaterial;
		}


		lMaterial = FbxSurfaceMaterial::Create(scene, lMaterialName.Buffer());
		EMMMaterial *emm_mat = material->getMaterials().at(isFound);
		string &shaderName = emm_mat->getShaderName();


		



		//**** inspired by FBX_2015_1\samples\ExportShader\main.cxx
		FbxImplementation* lImpl = FbxImplementation::Create(scene, FbxString((material_name +"_Implementation").c_str()));

		lMaterial->AddImplementation(lImpl);
		lMaterial->SetDefaultImplementation(lImpl);
		lImpl->RenderAPI = FBXSDK_RENDERING_API_DIRECTX;
		lImpl->RenderAPIVersion = "9.0";
		lImpl->Language = FBXSDK_SHADING_LANGUAGE_HLSL;
		lImpl->LanguageVersion = "1.0";

		FbxBindingTable* lTable = lTable = lImpl->AddNewTable("root", "shader");
		lImpl->RootBindingName = "root";

		lTable->DescAbsoluteURL = (shaderName +".fx").c_str();			// shader file
		lTable->DescTAG = "dx9";										// technique name


		//////////////////////// custom shader parameters for this material.		
		CustomTextureProp(scene, lMaterial, lTable, "g_ImageSampler1", ((texture_pack != NULL) ? (texture_pack->getName() + "\\DATA000.dds") : ""));	// Property g_ImageSampler1, the property type is sample, so connect a texture to it
		CustomTextureProp(scene, lMaterial, lTable, "g_SamplerToon", ((texture_dyt_pack != NULL) ? (texture_dyt_pack->getName() + "\\DATA000.dds") : ""));
		CustomTextureProp(scene, lMaterial, lTable, "g_ImageSamplerTemp14", ((texture_dyt_pack != NULL) ? (texture_dyt_pack->getName() + "\\DATA001.dds") : ""));

		FbxDouble4 MatCol0(0, 0, 0, 0);
		FbxDouble4 MatCol1(0, 0, 0, 0);
		FbxDouble4 MatCol2(0, 0, 0, 0);
		FbxDouble4 MatCol3(0, 0, 0, 0);
		FbxDouble4 MatScale0(1, 1, 1, 1);
		FbxDouble4 MatScale1(1, 1, 1, 1);

		vector<EMMParameter *> &parameters = emm_mat->getParameters();
		for (size_t i = 0; i < parameters.size(); i++) {
			string parameter_name = parameters[i]->name;
			if (parameter_name == "MatCol0R") MatCol0[0] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol0G") MatCol0[1] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol0B") MatCol0[2] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol0A") MatCol0[3] = parameters[i]->valueFloat();

			if (parameter_name == "MatCol1R") MatCol1[0] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol1G") MatCol1[1] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol1B") MatCol1[2] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol1A") MatCol1[3] = parameters[i]->valueFloat();

			if (parameter_name == "MatCol2R") MatCol2[0] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol2G") MatCol2[1] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol2B") MatCol2[2] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol2A") MatCol2[3] = parameters[i]->valueFloat();

			if (parameter_name == "MatCol3R") MatCol3[0] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol3G") MatCol3[1] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol3B") MatCol3[2] = parameters[i]->valueFloat();
			if (parameter_name == "MatCol3A") MatCol3[3] = parameters[i]->valueFloat();

			if (parameter_name == "MatScale0X") MatScale0[0] = parameters[i]->valueFloat();
			if (parameter_name == "MatScale0Y") MatScale0[1] = parameters[i]->valueFloat();
			if (parameter_name == "MatScale0Z") MatScale0[2] = parameters[i]->valueFloat();
			if (parameter_name == "MatScale0W") MatScale0[3] = parameters[i]->valueFloat();

			if (parameter_name == "MatScale1X") MatScale1[0] = parameters[i]->valueFloat();
			if (parameter_name == "MatScale1Y") MatScale1[1] = parameters[i]->valueFloat();
			if (parameter_name == "MatScale1Z") MatScale1[2] = parameters[i]->valueFloat();
			if (parameter_name == "MatScale1W") MatScale1[3] = parameters[i]->valueFloat();
		}

		// Override Battle Damage
		MatCol3[0] = 0.0; // Scratch Mark Multiplier
		MatCol3[1] = 0.0; // Blood Mark Multiplier

		/*
		FbxDouble4 val(0, 0, 0, 0);
		CustomParameterProp(lMaterial, lTable, "g_vFadeMulti_PS", val);
		CustomParameterProp(lMaterial, lTable, "g_vFadeRim_PS", val);
		CustomParameterProp(lMaterial, lTable, "g_vFadeAdd_PS", val);
		*/

		CustomParameterProp(lMaterial, lTable, "MatCol0", MatCol0);
		CustomParameterProp(lMaterial, lTable, "MatCol1", MatCol1);
		CustomParameterProp(lMaterial, lTable, "MatCol2", MatCol2);
		CustomParameterProp(lMaterial, lTable, "MatCol3", MatCol3);
		CustomParameterProp(lMaterial, lTable, "MatScale0", MatScale0);
		CustomParameterProp(lMaterial, lTable, "MatScale1", MatScale1);

		/*
		CustomParameterProp(lMaterial, lTable, "g_vColor0_PS", val);
		CustomParameterProp(lMaterial, lTable, "g_vColor1_PS", val);
		CustomParameterProp(lMaterial, lTable, "g_vColor2_PS", val);
		CustomParameterProp(lMaterial, lTable, "g_vColor12_PS", val);
		val = FbxDouble4(1,1,1,1);
		CustomParameterProp(lMaterial, lTable, "g_Color_Multiplier", val);
		val = FbxDouble4(0,0,0,0);
		CustomParameterProp(lMaterial, lTable, "g_vParam4_PS", val);
		CustomParameterProp(lMaterial, lTable, "g_vParam5_PS", val);
		val = FbxDouble4(0, 23.2558, 0.04587, 0);
		CustomParameterProp(lMaterial, lTable, "g_Toon_Detail", val);
		*/

		return lMaterial;
	}


	FbxMesh *EMD::exportFBX(FbxScene *scene, FbxNode *lMeshNode, string path) {
		FbxMesh *lMesh = FbxMesh::Create(scene, "EMDFBXMeshNode");
		if (!lMesh) {
			return NULL;
		}

		unsigned int control_point_base = 0;
		unsigned int total_control_points = getTotalPointCount();
		lMesh->InitControlPoints(total_control_points);

		FbxGeometryElementNormal *lGeometryElementNormal = lMesh->CreateElementNormal();
		lGeometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
		lGeometryElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);

		FbxGeometryElementUV *lGeometryUVElement = lMesh->CreateElementUV("DiffuseUV");
		lGeometryUVElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
		lGeometryUVElement->SetReferenceMode(FbxGeometryElement::eDirect);

		FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
		lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
		lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);


		//------ read material/texture files
		string baseFileName = path + lMeshNode->GetName();
		string emb_filename = baseFileName + ".emb";
		string emb_dyt_filename = baseFileName + ".dyt.emb";
		string emm_filename = baseFileName + ".emm";


		EMM *material = NULL;
		EMB *texture_pack = NULL;
		EMB *texture_dyt_pack = NULL;

		if (!LibXenoverse::fileCheck(emm_filename))
		{
			printf("No EMM Pack with the name %s found. Make sure it's on the same folder as the EMD file you're adding and it's not open by any other application!", emm_filename.c_str());
		}
		else {
			material = new EMM();
			if (!material->load(emm_filename)) {
				printf("Invalid EMM Material Pack. Is  %s valid ? ", emm_filename.c_str());
				delete material;
				material = NULL;
			}
		}

		if (!LibXenoverse::fileCheck(emb_filename))
		{
			printf("No EMB Pack with the name %s found. Make sure it's on the same folder as the EMD file you're adding and it's not open by any other application!", emb_filename.c_str());
		} else {
			texture_pack = new EMB();
			if (!texture_pack->load(emb_filename)){
				printf("Invalid EMB Texture Pack. Is  %s valid ? ", emb_filename.c_str());
				delete texture_pack;
				texture_pack = NULL;
			}else{
				CreateDirectory(texture_pack->getName().c_str(), NULL);
				texture_pack->extract(texture_pack->getName() +"/");					//extract textures (to create fbx textures after)
			}
		}

		if (!LibXenoverse::fileCheck(emb_dyt_filename))
		{
			printf("No EMB DYT Pack with the name %s found. Make sure it's on the same folder as the EMD file you're adding and it's not open by any other application!", emb_dyt_filename.c_str());
		} else {

			texture_dyt_pack = new EMB();
			if (!texture_dyt_pack->load(emb_dyt_filename)) {
				printf("Invalid EMB DYT Texture Pack. Is  %s valid ? ", emb_dyt_filename.c_str());
				delete texture_dyt_pack;
				texture_dyt_pack = NULL;
			}else{
				CreateDirectory(texture_dyt_pack->getName().c_str(), NULL);
				texture_dyt_pack->extract(texture_dyt_pack->getName() + "/");			//extract textures (to create fbx textures after)
			}
		}
		
		



		vector<string> material_names = getMaterialNames();
		vector<FbxSurfaceMaterial *> materials;
		materials.resize(material_names.size());

		for (size_t i = 0; i < material_names.size(); i++) {
			materials[i] = exportFBXMaterial(scene, material_names[i], material, texture_pack, texture_dyt_pack);
			lMeshNode->AddMaterial(materials[i]);
		}

		for (size_t i = 0; i < models.size(); i++) {
			models[i]->exportFBX(lMesh, control_point_base, lGeometryElementNormal, lGeometryUVElement);
		}

		
		if (material)
			delete material;
		if (texture_dyt_pack)
			delete texture_dyt_pack;
		if (texture_pack)
			delete texture_pack;
		
		return lMesh;
	}
#endif
}