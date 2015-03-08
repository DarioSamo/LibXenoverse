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

	void EMDTriangles::exportFBX(FbxMesh *lMesh, unsigned int &control_point_base) {
		for (size_t i = 0; i < faces.size(); i += 3) {
			lMesh->BeginPolygon();
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
			triangles[i].exportFBX(lMesh, control_point_base);
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

	FbxMesh *EMD::exportFBX(FbxScene *scene) {
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

		for (size_t i = 0; i < models.size(); i++) {
			models[i]->exportFBX(lMesh, control_point_base, lGeometryElementNormal, lGeometryUVElement);
		}

		return lMesh;
	}
#endif
}