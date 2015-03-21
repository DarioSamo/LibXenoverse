namespace LibXenoverse {
#ifdef LIBXENOVERSE_FBX_SUPPORT
	void EMDSubmesh::importFBX(FbxMesh *lMesh, int material_index, FbxAMatrix transform_matrix, vector<string> &material_names, vector<vector<pair<double, FbxNode *>>> &control_points_skin_bindings) {
		// FBX Mesh UV Sets
		FbxStringList uv_sets;
		lMesh->GetUVSetNames(uv_sets);

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
			AABB vertex_aabb;
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
					EMDTriangles *triangle_list_ptr = NULL;

					// Gather the bone names the 3 vertices use
					vector<string> polygon_bone_names;
					for (int j = 0; j < polygon_size; j++) {
						int control_point_index = lMesh->GetPolygonVertex(lPolygonIndex, j);
						vector<pair<double, FbxNode *>> &skin_bindings = control_points_skin_bindings[control_point_index];
						
						for (size_t s = 0; s < skin_bindings.size(); s++) {
							if (s >= 4) break;

							string bone_name = skin_bindings[s].second->GetName();
							bool found = false;
							for (size_t k = 0; k < polygon_bone_names.size(); k++) {
								if (polygon_bone_names[k] == bone_name) {
									found = true;
									break;
								}
							}
							if (!found) polygon_bone_names.push_back(bone_name);
						}
					}

					// Add a failsafe bone in case no bones were detected for the current polygon
					if (!polygon_bone_names.size()) {
						polygon_bone_names.push_back("b_C_Spine2");
					}

					// Detect appropriate triangle list for these 3 vertices
					triangle_list_ptr = getTriangleListFor(polygon_bone_names);

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

						for (int set = 0; set < uv_sets.GetCount(); set++) {
							if (set > 1) break;
							FbxVector2 uv;
							bool no_uv;
							lMesh->GetPolygonVertexUV(lPolygonIndex, j, uv_sets[set].Buffer(), uv, no_uv);
							v.u = (float) uv[0];
							v.v = 1.0f + (float) uv[1];

							LOG_DEBUG("UV: %d %d %f %f %d\n", lPolygonIndex, j, v.u, v.v, no_uv);
						}

						vector<pair<double, FbxNode *>> &skin_bindings = control_points_skin_bindings[control_point_index];
						if (triangle_list_ptr) {
							for (size_t s = 0; s < skin_bindings.size(); s++) {
								string bone_name = skin_bindings[s].second->GetName();

								for (size_t b = 0; b < triangle_list_ptr->bone_names.size(); b++) {
									if (triangle_list_ptr->bone_names[b] == bone_name) {
										v.bone[3 - s] = b;
										v.bone_weight[s] = (float)skin_bindings[s].first;
										break;
									}
								}
							}
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
							vertex_aabb.addPoint(v.x, v.y, v.z);
						}
					}
				}
				else printf("Unsupported polygon size.\n");
			}

			// Fill Submesh Definitions
			EMDSubmeshDefinition definition;
			definition.flag_1 = 0;
			definition.tex_index = 0;
			definition.flag_2 = 0;
			definition.flag_3 = 0x22;
			definition.a = 1.0f;
			definition.b = 1.0f;

			// Add 2 of these
			definitions.push_back(definition);
			definitions.push_back(definition);

			// Fill Submesh Data
			// FIXME: The Ws need to be figured out to know what they actually are
			aabb_center_x = vertex_aabb.centerX();
			aabb_center_y = vertex_aabb.centerY();
			aabb_center_z = vertex_aabb.centerZ();
			aabb_center_w = vertex_aabb.sizeX();
			aabb_min_x = vertex_aabb.start_x;
			aabb_min_y = vertex_aabb.start_y;
			aabb_min_z = vertex_aabb.start_z;
			aabb_min_w = vertex_aabb.sizeY();
			aabb_max_x = vertex_aabb.end_x;
			aabb_max_y = vertex_aabb.end_y;
			aabb_max_z = vertex_aabb.end_z;
			aabb_max_w = vertex_aabb.sizeZ();

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

		// After Submeshes are imported, calculate new AABB
		aabb_min_x = aabb_min_y = aabb_min_z = LIBXENOVERSE_AABB_MAX_START;
		aabb_max_x = aabb_max_y = aabb_max_z = LIBXENOVERSE_AABB_MIN_END;
		for (size_t i = 0; i < submeshes.size(); i++) {
			if (submeshes[i]->aabb_max_x > aabb_max_x) aabb_max_x = submeshes[i]->aabb_max_x;
			if (submeshes[i]->aabb_max_y > aabb_max_y) aabb_max_y = submeshes[i]->aabb_max_y;
			if (submeshes[i]->aabb_max_z > aabb_max_z) aabb_max_z = submeshes[i]->aabb_max_z;

			if (submeshes[i]->aabb_min_x < aabb_min_x) aabb_min_x = submeshes[i]->aabb_min_x;
			if (submeshes[i]->aabb_min_y < aabb_min_y) aabb_min_y = submeshes[i]->aabb_min_y;
			if (submeshes[i]->aabb_min_z < aabb_min_z) aabb_min_z = submeshes[i]->aabb_min_z;

			if (submeshes[i]->aabb_center_w > aabb_center_w) aabb_center_w = submeshes[i]->aabb_center_w;
			if (submeshes[i]->aabb_min_w    > aabb_min_w)    aabb_min_w    = submeshes[i]->aabb_min_w;
			if (submeshes[i]->aabb_max_w    > aabb_max_w)    aabb_max_w    = submeshes[i]->aabb_max_w;
		}

		aabb_center_x = (aabb_max_x + aabb_min_x) / 2.0f;
		aabb_center_y = (aabb_max_y + aabb_min_y) / 2.0f;
		aabb_center_z = (aabb_max_z + aabb_min_z) / 2.0f;
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