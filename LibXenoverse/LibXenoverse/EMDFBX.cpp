

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

	void EMDSubmesh::importFBX(FbxMesh *lMesh, int material_index, bool single_material) {
		vector<unsigned short> new_indices;

		// Scan FBX Mesh for vertices on the current material index
		int lPolygonCount = lMesh->GetPolygonCount();
		FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;

		bool add = false;
		if (!single_material) {
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
						v.x = control_point[0];
						v.y = control_point[1];
						v.z = control_point[2];
						v.nx = normal[0];
						v.ny = normal[1];
						v.nz = normal[2];

						FbxStringList uv_sets;
						lMesh->GetUVSetNames(uv_sets);

						for (int set = 0; set < uv_sets.GetCount(); set++) {
							if (set > 1) break;
							FbxVector2 uv;
							bool no_uv;
							lMesh->GetPolygonVertexUV(lPolygonIndex, j, uv_sets[set].Buffer(), uv, no_uv);
							v.u = uv[0];
							v.v = 1.0 - uv[1];
						}

						/*
						for (size_t c = 0; c<vertex_color_count; c++) {
						FbxGeometryElementVertexColor* lVertexColor = lMesh->GetElementVertexColor(c);

						if ((lVertexColor->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) && (lVertexColor->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)) {
						FbxColor lColor = lVertexColor->GetDirectArray().GetAt(control_point_index);

						if (c == 0) {
						v->rgba[0] = (lColor.mRed * 255.0);
						v->rgba[1] = (lColor.mGreen * 255.0);
						v->rgba[2] = (lColor.mBlue * 255.0);
						v->rgba[3] = (lColor.mAlpha * 255.0);
						}

						if (c == 1) {
						v->rgba_2[0] = (lColor.mRed * 255.0);
						v->rgba_2[1] = (lColor.mGreen * 255.0);
						v->rgba_2[2] = (lColor.mBlue * 255.0);
						v->rgba_2[3] = (lColor.mAlpha * 255.0);
						}
						}
						else {
						printf("Vertex color mapping type unsupported for this FBX! Report!\n");
						getchar();
						}
						}
						*/

						// Detect for clones on current vertex list
						bool clone = false;
						for (unsigned int y = 0; y < vertices.size(); y++) {
							if (vertices[y] == v) {
								clone = true;
								new_indices.push_back(y);
								break;
							}
						}

						if (!clone) {
							vertices.push_back(v);
							new_indices.push_back(vertices.size() - 1);
						}
					}
				}
				else printf("Unsupported polygon size.\n");
			}
		}

		// Fill Submesh Triangles
		EMDTriangles triangle_list;
		triangle_list.faces = new_indices;
		triangle_list.bone_names.push_back("b_C_Head");
		triangles.push_back(triangle_list);

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

		name = "face";
	}

	void EMDMesh::importFBX(FbxNode *lNode) {
		if (!lNode) return;

		FbxMesh *lMesh = lNode->GetMesh();
		if (!lMesh) return;

		name = ToString(lNode->GetName());

		for (size_t i = 0; i < 12; i++) {
			float_group[i] = 1.0f;
		}

		int material_count = lNode->GetMaterialCount();
		for (int m = 0; m < material_count; m++) {
			FbxSurfaceMaterial *lMaterial = lNode->GetMaterial(m);
			//addFBXMaterial(lMaterial);
		}

		for (int m = 0; m < material_count; m++) {
			EMDSubmesh *submesh = new EMDSubmesh();
			submesh->importFBX(lMesh, m, material_count == 1 ? true : false);
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