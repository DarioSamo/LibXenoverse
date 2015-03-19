#include "EMDOgre.h"


EMDOgre::EMDOgre() {
	mesh_resources_created = false;
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

	/*
	if (skeleton_name.size()) {
	ogre_mesh->setSkeletonName(skeleton_name);
	}
	*/

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
		Ogre::SubMesh *sub=createOgreSubmesh(&submesh_triangles[i], ogre_mesh);
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

	ogre_mesh->_setBounds(Ogre::AxisAlignedBox(mesh_aabb.start_x, mesh_aabb.start_y, mesh_aabb.start_z, mesh_aabb.end_x, mesh_aabb.end_y, mesh_aabb.end_z));
	ogre_mesh->_setBoundingSphereRadius(mesh_aabb.sizeMax() / 2);
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
			entity->setMaterialName(name + "_" + submeshes[i]->getMaterialName());
			model_node->attachObject(entity);
		}
	}
	return model_node;
}

Ogre::SceneNode *EMDOgre::createOgreSceneNode(Ogre::SceneManager *scene_manager) {
	Ogre::SceneNode *parent_node = scene_manager->getRootSceneNode()->createChildSceneNode();
	for (size_t i = 0; i < models.size(); i++) {
		createOgreSceneNodeModel(models[i], parent_node, scene_manager);
	}

	mesh_resources_created = true;
	return parent_node;
}