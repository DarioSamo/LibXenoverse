namespace LibXenoverse {
#ifdef LIBXENOVERSE_FBX_SUPPORT
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

			bone_node->LclTranslation.Set(pTranslation * -1);
			bone_node->LclScaling.Set(pScaling);
			FbxQuaternion lcl_quat(pRotation);
			FbxVector4 lcl_rotation;
			lcl_rotation.SetXYZ(lcl_quat);
			bone_node->LclRotation.Set(lcl_rotation * -1);

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