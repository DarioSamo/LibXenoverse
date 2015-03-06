namespace LibXenoverse {
#ifdef LIBXENOVERSE_FBX_SUPPORT
	/*

	FbxNode *FBX::addHavokBone(FbxNode *parent_node, unsigned int parent_index, vector<FbxNode *> &skeleton_bones, hkaSkeleton *skeleton) {
		FbxNode *root_bone = NULL;
		for (int b = 0; b<skeleton->m_bones.getSize(); b++) {
			hkaBone &bone = skeleton->m_bones[b];
			string bone_name = bone.m_name;
			size_t model_bone_index = 0;
			bool found = false;

			if (skeleton->m_parentIndices[b] != parent_index) {
				continue;
			}

			FbxSkeleton* lSkeletonRootAttribute = FbxSkeleton::Create(scene, bone_name.c_str());
			if (parent_index == -1) lSkeletonRootAttribute->SetSkeletonType(FbxSkeleton::eRoot);
			else lSkeletonRootAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);

			FbxNode* bone_node = FbxNode::Create(scene, bone_name.c_str());
			bone_node->SetNodeAttribute(lSkeletonRootAttribute);
			root_bone = bone_node;

			hkQsTransform ref = skeleton->m_referencePose[b];
			bone_node->LclTranslation.Set(FbxVector4(ref.getTranslation()(0), ref.getTranslation()(1), ref.getTranslation()(2)));
			//bone_node->LclScaling.Set(FbxVector4(ref.getScale()(0), ref.getScale()(1), ref.getScale()(2)));
			FbxQuaternion lcl_quat(ref.getRotation()(0), ref.getRotation()(1), ref.getRotation()(2), ref.getRotation()(3));
			FbxVector4 lcl_rotation;
			lcl_rotation.SetXYZ(lcl_quat);
			bone_node->LclRotation.Set(lcl_rotation);

			skeleton_bones[b] = bone_node;
			parent_node->AddChild(bone_node);

			addHavokBone(bone_node, b, skeleton_bones, skeleton);
		}
		return root_bone;
	}


	// Create a skeleton with 2 segments.
	FbxNode *FBX::addHavokSkeleton(vector<FbxNode *> &skeleton_bones, hkaSkeleton *skeleton) {
		FbxNode *lRootNode = scene->GetRootNode();
		FbxNode *skeleton_root_bone = addHavokBone(lRootNode, -1, skeleton_bones, skeleton);
		return skeleton_root_bone;
	}*/

	void ESK::createFBXBone(FbxScene *scene, FbxNode *root_node, unsigned short parent_index, vector<FbxNode *> &fbx_bones) {
		for (size_t b = 0; b < bones.size(); b++) {
			ESKBone *bone = bones[b];

			if (bone->parent_index != parent_index) {
				continue;
			}

			FbxMatrix parent_matrix;
			parent_matrix.SetIdentity();

			if (bone->parent_index < bones.size()) {
				ESKBone *pbone = bones[bone->parent_index];

				parent_matrix = FbxMatrix(pbone->transform_matrix[0], pbone->transform_matrix[1], pbone->transform_matrix[2], pbone->transform_matrix[3],
										  pbone->transform_matrix[4], pbone->transform_matrix[5], pbone->transform_matrix[6], pbone->transform_matrix[7],
										  pbone->transform_matrix[8], pbone->transform_matrix[9], pbone->transform_matrix[10], pbone->transform_matrix[11],
										  pbone->transform_matrix[12], pbone->transform_matrix[13], pbone->transform_matrix[14], pbone->transform_matrix[15]);
			}

			FbxSkeleton* lSkeletonRootAttribute = FbxSkeleton::Create(scene, bone->name.c_str());
			if (parent_index == 0xFFFF) lSkeletonRootAttribute->SetSkeletonType(FbxSkeleton::eRoot);
			else lSkeletonRootAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);

			FbxNode* bone_node = FbxNode::Create(scene, bone->name.c_str());
			bone_node->SetNodeAttribute(lSkeletonRootAttribute);

			FbxMatrix fbx_matrix(bone->transform_matrix[0],  bone->transform_matrix[1],  bone->transform_matrix[2],  bone->transform_matrix[3],
								 bone->transform_matrix[4],  bone->transform_matrix[5],  bone->transform_matrix[6],  bone->transform_matrix[7],
								 bone->transform_matrix[8],  bone->transform_matrix[9],  bone->transform_matrix[10], bone->transform_matrix[11],
								 bone->transform_matrix[12], bone->transform_matrix[13], bone->transform_matrix[14], bone->transform_matrix[15]);

			fbx_matrix = fbx_matrix * parent_matrix.Inverse();

			FbxVector4 pTranslation;
			FbxQuaternion pRotation;
			FbxVector4 pShearing;
			FbxVector4 pScaling;
			double pSign;

			fbx_matrix.GetElements(pTranslation, pRotation, pShearing, pScaling, pSign);

			bone_node->LclTranslation.Set(pTranslation*-1);
			bone_node->LclScaling.Set(pScaling);
			FbxQuaternion lcl_quat(pRotation);
			FbxVector4 lcl_rotation;
			lcl_rotation.SetXYZ(lcl_quat);
			bone_node->LclRotation.Set(lcl_rotation);

			fbx_bones[b] = bone_node;
			root_node->AddChild(bone_node);

			createFBXBone(scene, bone_node, b, fbx_bones);
		}
	}

	vector<FbxNode *> ESK::createFBXSkeleton(FbxScene *scene) {
		vector<FbxNode *> fbx_bones;
		fbx_bones.resize(bones.size());

		FbxNode *lRootNode = scene->GetRootNode();
		createFBXBone(scene, lRootNode, 0xFFFF, fbx_bones);
		return fbx_bones;
	}
#endif
}