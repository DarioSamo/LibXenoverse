namespace LibXenoverse {
#ifdef LIBXENOVERSE_FBX_SUPPORT
	void EMDSubmesh::importFBX(FbxMesh *lMesh, int material_index, FbxAMatrix transform_matrix, vector<string> &material_names, vector<vector<pair<double, FbxNode *>>> &control_points_skin_bindings) {
		// Scan FBX Mesh for vertices on the current material index
		int lPolygonCount = lMesh->GetPolygonCount();
		FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;

		FbxAMatrix rotation_matrix = transform_matrix;
		rotation_matrix.SetT(FbxVector4(0.0, 0.0, 0.0, 0.0));
		rotation_matrix.SetS(FbxVector4(1.0, 1.0, 1.0, 1.0));

		bool add = false;
		if (material_names.size() != 1) {
			FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
			if (lMesh->GetElementMaterial()) {
				lMaterialIndice = &lMesh->GetElementMaterial()->GetIndexArray();
				lMaterialMappingMode = lMesh->GetElementMaterial()->GetMappingMode();


				if (!lMaterialIndice) printf("Mesh has no material index element.\n");

				if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon) {
					if (lMaterialIndice->GetCount() == lPolygonCount) {
						add = true;
					}
				}
			}
		}
		else add = true;

		if (add) {
			int control_points_count = lMesh->GetControlPointsCount();
			FbxVector4 *control_points = lMesh->GetControlPoints();
			int vertex_color_count = lMesh->GetElementVertexColorCount();


			for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex) {
				if (lMaterialIndice) {
					const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
					if (lMaterialIndex != material_index) continue;
				}

				int polygon_size = lMesh->GetPolygonSize(lPolygonIndex);
				if (polygon_size == 3) {
					for (int j = 0; j < polygon_size; j++) {
						int control_point_index = lMesh->GetPolygonVertex(lPolygonIndex, j);

						FbxVector4 control_point = transform_matrix.MultT(control_points[control_point_index]);
						FbxVector4 normal;
						lMesh->GetPolygonVertexNormal(lPolygonIndex, j, normal);
						normal = rotation_matrix.MultT(normal);

						// Create Vertex
						EMDVertex v;
						v.zero();
						v.x = (float)control_point[0];
						v.y = (float)control_point[1];
						v.z = (float)control_point[2];
						v.nx = (float)normal[0];
						v.ny = (float)normal[1];
						v.nz = (float)normal[2];

						FbxStringList uv_sets;
						lMesh->GetUVSetNames(uv_sets);

						for (int set = 0; set < uv_sets.GetCount(); set++) {
							if (set > 1) break;
							FbxVector2 uv;
							bool no_uv;
							lMesh->GetPolygonVertexUV(lPolygonIndex, j, uv_sets[set].Buffer(), uv, no_uv);
							v.u = (float) uv[0];
							v.v = 1.0f - (float) uv[1];
						}

						vector<pair<double, FbxNode *>> &skin_bindings = control_points_skin_bindings[control_point_index];
						vector<string> bone_names;

						EMDTriangles *triangle_list_ptr = NULL;
						if (skin_bindings.size()) {
							// Gather bone names for the skin bindings of the current control point
							// and try to get a matching triangle list
							for (size_t s = 0; s < skin_bindings.size(); s++) {
								if (s >= 4) break;

								string bone_name = skin_bindings[s].second->GetName();
								bone_names.push_back(bone_name);
							}
							triangle_list_ptr = getTriangleListFor(bone_names);

							for (size_t s = 0; s < bone_names.size(); s++) {
								for (size_t b = 0; b < triangle_list_ptr->bone_names.size(); b++) {
									if (triangle_list_ptr->bone_names[b] == bone_names[s]) {
										v.bone[3-s] = b;
										v.bone_weight[s] = (float)skin_bindings[s].first;
										break;
									}
								}
							}
						}
						else {
							// Add a failsafe bone in case no bones were detected for the current submesh
							bone_names.push_back("b_C_Spine2");
							triangle_list_ptr = getTriangleListFor(bone_names);
						}

						// Detect for clones on current vertex list
						bool clone = false;
						for (unsigned int y = 0; y < vertices.size(); y++) {
							if (vertices[y] == v) {
								clone = true;
								if (triangle_list_ptr) triangle_list_ptr->faces.push_back(y);
								break;
							}
						}

						if (!clone) {
							if (triangle_list_ptr) triangle_list_ptr->faces.push_back(vertices.size());
							vertices.push_back(v);
						}
					}
				}
				else printf("Unsupported polygon size.\n");
			}

			// Fill Submesh Definitions
			EMDSubmeshDefinition definition;
			definition.flag = 0x22;
			definition.a = 1.0f;
			definition.b = 1.0f;

			// Add 2 of these
			definitions.push_back(definition);
			definitions.push_back(definition);

			// Fill Submesh Data
			for (size_t i = 0; i < 12; i++) {
				float_group[i] = 0.0f;
			}

			vertex_type_flag = 0x8207;
			vertex_size = 36;

			name = material_names[material_index];
		}
	}

	EMDTriangles *EMDSubmesh::getTriangleListFor(vector<string> bone_names) {
		for (size_t i = 0; i < triangles.size(); i++) {
			vector<string> bones_to_add;
			vector<string> &triangle_bone_names = triangles[i].bone_names;

			// Detect what bones of the list exist on the triangle's bone names
			for (size_t b = 0; b < bone_names.size(); b++) {
				bool found = false;

				for (size_t j = 0; j < triangle_bone_names.size(); j++) {
					if (bone_names[b] == triangle_bone_names[j]) {
						found = true;
						break;
					}
				}

				if (!found) {
					bones_to_add.push_back(bone_names[b]);
				}
			}

			// If some bones were undetected, check if it can be added to the triangle list's bones.
			if (bones_to_add.size()) {
				size_t new_size = triangle_bone_names.size() + bones_to_add.size();
				if (new_size > LIBXENOVERSE_EMD_SUBMESH_BONE_LIMIT) {
					continue;
				}
				else {
					triangle_bone_names.reserve(new_size);
					triangle_bone_names.insert(triangle_bone_names.end(), bones_to_add.begin(), bones_to_add.end());
					return &triangles[i];
				}
			}
			else {
				return &triangles[i];
			}
		}

		// If no good triangle list was found, create a new one
		size_t triangles_index = triangles.size();
		triangles.resize(triangles_index+1);
		triangles[triangles_index].bone_names = bone_names;
		return &triangles[triangles_index];
	}

	void EMDMesh::importFBX(FbxNode *lNode) {
		if (!lNode) return;

		FbxMesh *lMesh = lNode->GetMesh();
		if (!lMesh) return;

		name = ToString(lNode->GetName());

		for (size_t i = 0; i < 12; i++) {
			float_group[i] = 0.0f;
		}

		vector<string> material_names;
		int material_count = lNode->GetMaterialCount();
		for (int m = 0; m<material_count; m++) {
			FbxSurfaceMaterial *lMaterial = lNode->GetMaterial(m);
			string material_name = lMaterial->GetName();
			material_names.push_back(material_name);
		}

		// Skin Cluster
		int control_points_count = lMesh->GetControlPointsCount();
		vector<vector<pair<double, FbxNode *>>> control_points_skin_bindings;
		control_points_skin_bindings.resize(control_points_count);

		// Iterate through the Skin deformers
		int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int i = 0; i < lSkinCount; i++) {
			FbxSkin* lSkin = (FbxSkin *)lMesh->GetDeformer(i, FbxDeformer::eSkin);

			// Iterate through the clusters of the skin deformer
			int lClusterCount = lSkin->GetClusterCount();
			for (int j = 0; j != lClusterCount; j++) {
				FbxCluster *lCluster = lSkin->GetCluster(j);

				int lIndexCount = lCluster->GetControlPointIndicesCount();
				int *lIndices = lCluster->GetControlPointIndices();
				double *lWeights = lCluster->GetControlPointWeights();

				for (int k = 0; k < lIndexCount; k++) {
					int lControlIndex = lIndices[k];
					if ((lControlIndex < control_points_count) && lCluster->GetLink()) {
						pair<double, FbxNode *> lSkinLink;
						lSkinLink.first = lWeights[k];
						lSkinLink.second = lCluster->GetLink();

						bool added = false;
						vector<pair<double, FbxNode *>> &skin_bindings = control_points_skin_bindings[lControlIndex];
						for (size_t w = 0; w < skin_bindings.size(); w++) {
							if (skin_bindings[w].first < lSkinLink.first) {
								skin_bindings.insert(skin_bindings.begin()+w, lSkinLink);
								added = true;
								break;
							}
						}

						if (!added) skin_bindings.push_back(lSkinLink);
					}
				}
			}
		}

		FbxAMatrix transform_matrix = lNode->EvaluateGlobalTransform();
		// Go through materials
		for (int m = 0; m < material_count; m++) {
			EMDSubmesh *submesh = new EMDSubmesh();
			submesh->importFBX(lMesh, m, transform_matrix, material_names, control_points_skin_bindings);
			submeshes.push_back(submesh);
		}
	}

	void EMDModel::importFBX(FbxNode *lNode) {
		EMDMesh *mesh = new EMDMesh();
		mesh->importFBX(lNode);
		meshes.push_back(mesh);
	}

	void EMD::importFBX(FbxNode *lNode) {
		if (!lNode) return;

		EMDModel *model = new EMDModel();
		model->importFBX(lNode);
		models.push_back(model);

		for (size_t i = 0; i < models.size(); i++) {
			vector<string> mesh_names = models[i]->getMeshNames();
			for (size_t j = 0; j < mesh_names.size(); j++) {
				model_names.push_back(mesh_names[j]);
			}
		}
	}
#endif

}