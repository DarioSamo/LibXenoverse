

namespace LibXenoverse {
#ifdef LIBXENOVERSE_FBX_SUPPORT

	void EMDSubmesh::getTotalData(list<EMDVertex *> *vertex_list, list<unsigned int> *face_list,
		list<unsigned int> *submesh_vertex_counts, list<vector<string>> *submesh_bone_groups) {
		unsigned int face_base = vertex_list->size();
		for (size_t i = 0; i < vertices.size(); i++) {
			vertex_list->push_back(&vertices[i]);
		}

		for (size_t i = 0; i < triangles.size(); i++) {
			for (size_t j = 0; j < triangles[i].faces.size(); j++) {
				face_list->push_back(face_base + triangles[i].faces[j]);
			}
		}

		submesh_vertex_counts->push_back(vertices.size());
		//submesh_bone_groups->push_back(bone_names);
	}

	void EMDMesh::getTotalData(list<EMDVertex *> *vertex_list, list<unsigned int> *face_list,
		list<unsigned int> *submesh_vertex_counts, list<vector<string>> *submesh_bone_groups) {
		for (size_t i = 0; i < submeshes.size(); i++) {
			submeshes[i]->getTotalData(vertex_list, face_list, submesh_vertex_counts, submesh_bone_groups);
		}
	}

	void EMDModel::getTotalData(list<EMDVertex *> *vertex_list, list<unsigned int> *face_list,
		list<unsigned int> *submesh_vertex_counts, list<vector<string>> *submesh_bone_groups) {
		for (size_t i = 0; i < meshes.size(); i++) {
			meshes[i]->getTotalData(vertex_list, face_list, submesh_vertex_counts, submesh_bone_groups);
		}
	}

	void EMD::getTotalData(list<EMDVertex *> *vertex_list, list<unsigned int> *face_list,
		list<unsigned int> *submesh_vertex_counts, list<vector<string>> *submesh_bone_groups) {
		for (size_t i = 0; i < models.size(); i++) {
			models[i]->getTotalData(vertex_list, face_list, submesh_vertex_counts, submesh_bone_groups);
		}
	}

	vector<int> EMD::createFBXBoneMap(vector<string> bone_table, vector<FbxNode *> &fbx_bones) {
		vector<int> current_bone_indices;
		current_bone_indices.clear();

		for (size_t k = 0; k < bone_table.size(); k++) {
			string bone_name = bone_table[k];
			int found_index = -1;

			for (size_t l = 0; l < fbx_bones.size(); l++) {
				if (ToString(fbx_bones[l]->GetName()) == bone_name) {
					found_index = l;
					break;
				}
			}

			current_bone_indices.push_back(found_index);
		}

		return current_bone_indices;
	}

	void EMD::createFBXSkin(FbxScene *scene, FbxMesh *fbx_mesh, vector<FbxNode *> &fbx_bones, FbxAMatrix skin_matrix) {
		list<EMDVertex *> vertex_list;
		list<unsigned int> face_list;
		list<unsigned int> submesh_vertex_counts;
		list<vector<string>> submesh_bone_groups;
		getTotalData(&vertex_list, &face_list, &submesh_vertex_counts, &submesh_bone_groups);

		if (!vertex_list.size()) {
			return;
		}

		FbxSkin* lSkin = FbxSkin::Create(scene, "");

		for (size_t b = 0; b < fbx_bones.size(); b++) {
			// Create skin cluster
			FbxCluster *lCluster = FbxCluster::Create(scene, "");
			lCluster->SetLink(fbx_bones[b]);
			lCluster->SetLinkMode(FbxCluster::eTotalOne);

			list<unsigned int>::iterator vertex_count_iterator = submesh_vertex_counts.begin();
			list<vector<string>>::iterator bone_group_iterator = submesh_bone_groups.begin();

			vector<string> current_bone_table = *bone_group_iterator;
			vector<int> current_bone_indices = createFBXBoneMap(current_bone_table, fbx_bones);
			int current_vertex_count = *vertex_count_iterator;
			bool skip_group = false;

			unsigned int vertex_index = 0;
			for (list<EMDVertex *>::iterator it = vertex_list.begin(); it != vertex_list.end(); it++) {
				if (!skip_group) {
					// Transform Vertex Weights into Skin Cluster
					for (size_t j = 0; j < 4; j++) {
						unsigned char bone_index = (*it)->bone[j];
						float bone_weight_f = (*it)->bone_weight[j];

						if ((bone_index < current_bone_indices.size()) && (bone_weight_f > 0.0f)) {
							int bone_fbx_index = current_bone_indices[bone_index];
							if ((bone_fbx_index >= 0) && (bone_fbx_index == b)) {
								lCluster->AddControlPointIndex(vertex_index, bone_weight_f);
							}
						}
					}
				}
				
				// Skip to next Bone Table if vertex count is reached
				current_vertex_count -= 1;
				if (current_vertex_count <= 0) {
					vertex_count_iterator++;
					bone_group_iterator++;

					if (vertex_count_iterator != submesh_vertex_counts.end()) current_vertex_count = *vertex_count_iterator;

					if (bone_group_iterator != submesh_bone_groups.end()) {
						current_bone_table = *bone_group_iterator;
						current_bone_indices = createFBXBoneMap(current_bone_table, fbx_bones);

						skip_group = true;
						for (size_t i = 0; i < current_bone_indices.size(); i++) {
							if (current_bone_indices[i] == b) {
								skip_group = false;
								break;
							}
						}
					}
				}

				vertex_index++;
			}


			lCluster->SetTransformMatrix(skin_matrix);
			FbxAMatrix lXMatrix = fbx_bones[b]->EvaluateGlobalTransform();
			lCluster->SetTransformLinkMatrix(lXMatrix);
			lSkin->AddCluster(lCluster);
		}

		fbx_mesh->AddDeformer(lSkin);
	}


	FbxMesh *EMD::createFBXMesh(FbxScene *scene) {
		FbxMesh *lMesh = FbxMesh::Create(scene, "EMDFBXMeshNode");
		if (!lMesh) {
			return NULL;
		}


		FbxGeometryElementNormal* lGeometryElementNormal = lMesh->CreateElementNormal();
		lGeometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
		lGeometryElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);

		FbxGeometryElementUV* lGeometryUVElement = lMesh->CreateElementUV("DiffuseUV");
		lGeometryUVElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
		lGeometryUVElement->SetReferenceMode(FbxGeometryElement::eDirect);


		list<EMDVertex *> vertex_list;
		list<unsigned int> face_list;
		list<unsigned int> submesh_vertex_counts;
		list<vector<string>> submesh_bone_groups;
		getTotalData(&vertex_list, &face_list, &submesh_vertex_counts, &submesh_bone_groups);

		lMesh->InitControlPoints(vertex_list.size());
		FbxVector4* lControlPoints = lMesh->GetControlPoints();

		list<EMDVertex *>::iterator vertex_iterator = vertex_list.begin();
		list<unsigned int>::iterator face_iterator = face_list.begin();

		size_t index = 0;
		for (index = 0; index < vertex_list.size(); index++) {
			EMDVertex *v = *vertex_iterator;
			lControlPoints[index] = FbxVector4(v->x, v->y, v->z);
			lGeometryElementNormal->GetDirectArray().Add(FbxVector4(v->nx, v->ny, v->nz));
			lGeometryUVElement->GetDirectArray().Add(FbxVector2(v->u, 1.0 - v->v));
			vertex_iterator++;
		}

		for (index = 0; index < face_list.size() / 3; index++) {
			lMesh->BeginPolygon();
			lMesh->AddPolygon(*face_iterator);
			face_iterator++;
			lMesh->AddPolygon(*face_iterator);
			face_iterator++;
			lMesh->AddPolygon(*face_iterator);
			face_iterator++;
			lMesh->EndPolygon();
		}

		return lMesh;
	}

	void EMDSubmesh::importFBX(FbxMesh *lMesh, int material_index, vector<string> &material_names, vector<vector<pair<double, FbxNode *>>> &control_points_skin_bindings) {
		// Scan FBX Mesh for vertices on the current material index
		int lPolygonCount = lMesh->GetPolygonCount();
		FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;

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

						FbxVector4 control_point = /*transform_matrix.MultT(*/control_points[control_point_index]/*)*/;
						FbxVector4 normal;
						lMesh->GetPolygonVertexNormal(lPolygonIndex, j, normal);
						//normal = rotation_matrix.MultT(normal);

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
										v.bone[s] = b;
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
				float_group[i] = 1.0f;
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
			float_group[i] = 1.0f;
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

		// Go through materials
		for (int m = 0; m < material_count; m++) {
			EMDSubmesh *submesh = new EMDSubmesh();
			submesh->importFBX(lMesh, m, material_names, control_points_skin_bindings);
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