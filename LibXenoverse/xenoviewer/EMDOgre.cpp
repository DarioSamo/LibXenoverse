#include "EMDOgre.h"
#include "EMDRenderObjectListener.h"


EMDOgre::EMDOgre() {
	mesh_resources_created = false;
	texture_pack = NULL;
	skeleton = NULL;
}

Ogre::SubMesh *EMDOgre::createOgreSubmesh(EMDTriangles *triangles, Ogre::MeshPtr mesh) {
	Ogre::SubMesh *sub = mesh->createSubMesh();
	const size_t ibufCount = triangles->faces.size();
	unsigned short *faces = (unsigned short *)malloc(sizeof(unsigned short) * ibufCount);
	for (size_t i = 0; i<ibufCount; i++) {
		faces[i] = triangles->faces[i];
	}

	Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(Ogre::HardwareIndexBuffer::IT_16BIT, ibufCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	ibuf->writeData(0, ibuf->getSizeInBytes(), faces, true);
	sub->useSharedVertices = true;
	sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = ibufCount;
	sub->indexData->indexStart = 0;
	free(faces);
	return sub;
}

void EMDOgre::createOgreMesh(EMDSubmesh *submesh, string mesh_name) {
	string ogre_mesh_name = mesh_name + "_" + submesh->getMaterialName();
	Ogre::MeshPtr ogre_mesh = Ogre::MeshManager::getSingleton().createManual(ogre_mesh_name, XENOVIEWER_RESOURCE_GROUP);

	LibXenoverse::AABB mesh_aabb;
	mesh_aabb.reset();

	if (skeleton) {
		ogre_mesh->setSkeletonName(skeleton->getName());
	}

	// Create Vertex Pool
	vector<EMDVertex> submesh_vertices = submesh->getVertices();
	const size_t nVertices = submesh_vertices.size();
	const size_t nVertCount = 8;
	const size_t vbufCount = nVertCount*nVertices;
	float *vertices = (float *)malloc(sizeof(float)*vbufCount);
	for (size_t i = 0; i < nVertices; i++) {
		vertices[i*nVertCount] = submesh_vertices[i].x;
		vertices[i*nVertCount + 1] = submesh_vertices[i].y;
		vertices[i*nVertCount + 2] = submesh_vertices[i].z;
		vertices[i*nVertCount + 3] = submesh_vertices[i].nx;
		vertices[i*nVertCount + 4] = submesh_vertices[i].ny;
		vertices[i*nVertCount + 5] = submesh_vertices[i].nz;
		vertices[i*nVertCount + 6] = submesh_vertices[i].u;
		vertices[i*nVertCount + 7] = submesh_vertices[i].v;
		mesh_aabb.addPoint(submesh_vertices[i].x, submesh_vertices[i].y, submesh_vertices[i].z);
	}

	// Create Submeshes for each Triangle List
	vector<EMDTriangles> submesh_triangles = submesh->getTriangles();
	for (size_t i = 0; i < submesh_triangles.size(); i++) {
		Ogre::SubMesh *sub = createOgreSubmesh(&submesh_triangles[i], ogre_mesh);
	}

	// Create Shared Vertex Data for all submeshes
	Ogre::VertexData *vertex_data = new Ogre::VertexData();
	ogre_mesh->sharedVertexData = vertex_data;
	vertex_data->vertexCount = nVertices;

	Ogre::VertexDeclaration* decl = vertex_data->vertexDeclaration;
	size_t offset = 0;

	decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);

	Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(offset, nVertices, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);
	Ogre::VertexBufferBinding* bind = vertex_data->vertexBufferBinding;
	bind->setBinding(0, vbuf);


	// Create Bone Assignments if Skeleton name exists
	if (skeleton) {
		Ogre::Skeleton *ogre_skeleton = skeleton->getOgreSkeleton();
		if (ogre_skeleton) {
			for (size_t i = 0; i < submesh_triangles.size(); i++) {
				EMDTriangles *triangles = &submesh_triangles[i];
				vector<unsigned int> vertex_indices;
				size_t face_count = triangles->faces.size();

				// Make a list of all vertex indices being used in the submesh
				vertex_indices.reserve(face_count);
				for (size_t j = 0; j < face_count; j++) {
					bool found = false;

					for (size_t k = 0; k < vertex_indices.size(); k++) {
						if (vertex_indices[k] == triangles->faces[j]) {
							found = true;
							break;
						}
					}

					if (!found) vertex_indices.push_back(triangles->faces[j]);
				}

				// Build Bone Mapping Table
				vector<unsigned short> bone_table;
				bone_table.resize(triangles->bone_names.size());

				for (size_t j = 0; j < bone_table.size(); j++) {
					string bone_name = triangles->bone_names[j];

					LOG_DEBUG("Bone Skin Table %d: %s\n", j, bone_name.c_str());
					if (ogre_skeleton->hasBone(bone_name)) {
						Ogre::Bone *mBone = ogre_skeleton->getBone(bone_name);
						bone_table[j] = mBone->getHandle();
					}
					else {
						LOG_DEBUG("Couldn't find %s in ogre skeleton!\n", bone_name.c_str());
					}
				}

				// Add bone assignments to all the vertices that were found
				for (size_t j = 0; j < vertex_indices.size(); j++) {
					Ogre::VertexBoneAssignment vba;
					vba.vertexIndex = vertex_indices[j];
					EMDVertex *vertex = &submesh_vertices[vba.vertexIndex];

					for (size_t k = 0; k < 4; k++) {
						unsigned char bone_index = vertex->bone[3 - k];
						float bone_weight = vertex->bone_weight[k];

						if (bone_weight > 0.0f) {
							vba.boneIndex = bone_table[bone_index];
							vba.weight = bone_weight;
							ogre_mesh->addBoneAssignment(vba);
						}
					}
				}
			}

			// Apply changes, build the buffer
			ogre_mesh->_compileBoneAssignments();
			ogre_mesh->sharedVertexData->reorganiseBuffers(decl->getAutoOrganisedDeclaration(true, false, false));
		}
	}

	//ogre_mesh->_setBounds(Ogre::AxisAlignedBox(mesh_aabb.start_x, mesh_aabb.start_y, mesh_aabb.start_z, mesh_aabb.end_x, mesh_aabb.end_y, mesh_aabb.end_z));
	ogre_mesh->_setBounds(Ogre::AxisAlignedBox(-1000, -1000, -1000, 1000, 1000, 1000));
	ogre_mesh->_setBoundingSphereRadius(mesh_aabb.sizeMax());
	ogre_mesh->load();

	free(vertices);
}


void EMDOgre::createOgreEntityMeshes(EMDMesh *mesh) {
	vector<EMDSubmesh *> &submeshes = mesh->getSubmeshes();
	for (size_t i = 0; i < submeshes.size(); i++) {
		createOgreMesh(submeshes[i], mesh->getName());
	}
}

Ogre::SceneNode *EMDOgre::createOgreSceneNodeModel(EMDModel *model, Ogre::SceneNode *parent, Ogre::SceneManager *scene_manager) {
	Ogre::SceneNode *model_node = parent->createChildSceneNode();
	vector<EMDMesh *> &meshes = model->getMeshes();
	for (size_t i = 0; i < meshes.size(); i++) {
		vector<EMDSubmesh *> &submeshes = meshes[i]->getSubmeshes();
		if (!mesh_resources_created) {
			createOgreEntityMeshes(meshes[i]);
		}

		for (size_t j = 0; j < submeshes.size(); j++) {
			Ogre::Entity *entity = scene_manager->createEntity(meshes[i]->getName() + "_" + submeshes[j]->getMaterialName());
			entity->setMaterialName(name + "_" + submeshes[j]->getMaterialName());

			// Create Render Object Listeners depending on submesh definitions
			vector<EMDSubmeshDefinition> definitions = submeshes[j]->getDefinitions();
			if (texture_pack && texture_dyt_pack) {
				vector<Ogre::TexturePtr> textures = texture_pack->getOgreTextures();
				vector<Ogre::TexturePtr> textures_dyt = texture_dyt_pack->getOgreTextures();

				for (size_t k = 0; k < definitions.size(); k++) {
					unsigned short texture_index = definitions[k].tex_index;
					unsigned short dyt_texture_index = definitions[k].tex_index;

					if (texture_index >= textures.size()) texture_index = textures.size()-1;
					if (dyt_texture_index >= textures_dyt.size()) dyt_texture_index = textures_dyt.size() - 1;

					EMDRenderObject *emd_render_object = new EMDRenderObject(textures[texture_index], textures_dyt[dyt_texture_index]);
					EMDRenderObjectAssignVisitor visitor(emd_render_object);
					entity->visitRenderables(&visitor);

					break; // FIXME: Figure out why there's multiple definitions
				}
			}

			// Share Skeleton Instances
			if (skeleton) {
				Ogre::Entity *shared_entity = skeleton->getSharedEntity();
				if (!shared_entity) {
					skeleton->setSharedEntity(entity);
				}
				else if (entity->hasSkeleton()) {
					entity->shareSkeletonInstanceWith(shared_entity);
				}
			}

			model_node->attachObject(entity);
		}
	}
	return model_node;
}

Ogre::SceneNode *EMDOgre::createOgreSceneNode(Ogre::SceneManager *scene_manager, EMBOgre *texture_pack_p, EMBOgre *texture_dyt_pack_p) {
	texture_pack = texture_pack_p;
	texture_dyt_pack = texture_dyt_pack_p;

	Ogre::SceneNode *parent_node = scene_manager->getRootSceneNode()->createChildSceneNode();
	for (size_t i = 0; i < models.size(); i++) {
		createOgreSceneNodeModel(models[i], parent_node, scene_manager);
	}

	mesh_resources_created = true;
	return parent_node;
}