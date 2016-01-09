#include "ESKOgre.h"
#include "OgreCommon.h"

ESKOgre::ESKOgre() {
	skeleton = NULL;
	resources_created = false;
	skeleton_entity = NULL;
	animation_to_change = NULL;
	current_animation_state = NULL;
	to_delete = false;
	skeleton_node = NULL;
}

void ESKOgre::buildBone(unsigned short b, Ogre::Skeleton *ogre_skeleton, Ogre::Bone *parent_bone) {
	ESKBone *bone = bones[b];
	Ogre::Bone *mBone = ogre_skeleton->createBone(bone->name);
  if (parent_bone)
  {
    parent_bone->addChild(mBone);
  }

	Ogre::Matrix4 parent_matrix = Ogre::Matrix4::IDENTITY;

	if (bone->parent_index < bones.size()) {
		ESKBone *pbone = bones[bone->parent_index];

		parent_matrix = Ogre::Matrix4(pbone->transform_matrix[0], pbone->transform_matrix[4], pbone->transform_matrix[8], pbone->transform_matrix[12],
									  pbone->transform_matrix[1], pbone->transform_matrix[5], pbone->transform_matrix[9], pbone->transform_matrix[13],
									  pbone->transform_matrix[2], pbone->transform_matrix[6], pbone->transform_matrix[10], pbone->transform_matrix[14],
									  pbone->transform_matrix[3], pbone->transform_matrix[7], pbone->transform_matrix[11], pbone->transform_matrix[15]);
	}


	Ogre::Matrix4 transform_matrix(bone->transform_matrix[0], bone->transform_matrix[4], bone->transform_matrix[8], bone->transform_matrix[12],
								   bone->transform_matrix[1], bone->transform_matrix[5], bone->transform_matrix[9], bone->transform_matrix[13],
								   bone->transform_matrix[2], bone->transform_matrix[6], bone->transform_matrix[10], bone->transform_matrix[14],
								   bone->transform_matrix[3], bone->transform_matrix[7], bone->transform_matrix[11], bone->transform_matrix[15]);

	transform_matrix = transform_matrix * parent_matrix.inverse();

	Ogre::Vector3 mPos;
	Ogre::Vector3 mScale;
	Ogre::Quaternion mRot;
	transform_matrix.decomposition(mPos, mScale, mRot);

	LOG_DEBUG("Bone %d Setup for %s: %f %f %f %d\n", b, bone->name.c_str(), (float)mPos.x, (float)mPos.y, (float)mPos.z, bone->index_4);

	mBone->setPosition(mPos * -1);
	mBone->setScale(mScale);
	mBone->setOrientation(mRot.Inverse());
	mBone->setInitialState();

	mBone->setManuallyControlled(false);

	for (size_t i = 0; i < bones.size(); i++) {
		if (bones[i]->parent_index == b) {
			buildBone(i, ogre_skeleton, mBone);
		}
	}
}

void ESKOgre::createFakeEntity(Ogre::SceneManager *mSceneMgr) {
	Ogre::MeshPtr msh = Ogre::MeshManager::getSingleton().createManual(name + "_skeleton", XENOVIEWER_RESOURCE_GROUP);
	msh->setSkeletonName(name);

	Ogre::SubMesh* sub = msh->createSubMesh();
	const size_t nVertices = 3;
	const size_t nVertCount = 3;
	const size_t vbufCount = nVertCount*nVertices;
	float *vertices = (float *)malloc(sizeof(float)*vbufCount);

	for (size_t i = 0; i < nVertices; i++) {
		vertices[i*nVertCount] = 0.0;
		vertices[i*nVertCount + 1] = 0.0;
		vertices[i*nVertCount + 2] = 0.0;
	}

	const size_t ibufCount = 3;
	unsigned short *faces = (unsigned short *)malloc(sizeof(unsigned short) * ibufCount);

	for (size_t i = 0; i < ibufCount; i++) {
		faces[i] = i;
	}

	msh->sharedVertexData = new Ogre::VertexData();
	msh->sharedVertexData->vertexCount = nVertices;

	Ogre::VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
	size_t offset = 0;

	decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(offset, msh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);
	Ogre::VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding;
	bind->setBinding(0, vbuf);
	Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(Ogre::HardwareIndexBuffer::IT_16BIT, ibufCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	ibuf->writeData(0, ibuf->getSizeInBytes(), faces, true);
	sub->useSharedVertices = true;
	sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = ibufCount;
	sub->indexData->indexStart = 0;

	msh->_setBounds(Ogre::AxisAlignedBox(-100, -100, -100, 100, 100, 100));
	msh->_setBoundingSphereRadius(100);
	msh->load();

	free(faces);
	free(vertices);

	skeleton_entity = mSceneMgr->createEntity(name + "_skeleton");
	skeleton_node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	skeleton_node->attachObject(skeleton_entity);
	skeleton_node->setVisible(false);
}

void ESKOgre::refreshAnimations() {
	if (skeleton_entity) {
		skeleton_entity->refreshAvailableAnimationState();
	}
}


Ogre::Skeleton *ESKOgre::createOgreSkeleton(Ogre::SceneManager *mSceneMgr) {
	Ogre::SkeletonPtr mSkel = Ogre::SkeletonManager::getSingleton().create(name, XENOVIEWER_RESOURCE_GROUP, true);
	if (bones.size()) {
		buildBone(0, mSkel.getPointer(), NULL);
	}
	resources_created = true;
	createFakeEntity(mSceneMgr);
	skeleton = mSkel.getPointer();
	return mSkel.getPointer();
}

void ESKOgre::changeAnimation() {
	if (current_animation_state) {
		current_animation_state->setEnabled(false);
		current_animation_state = NULL;
	}

	if (animation_to_change) {
		string animation_name = animation_to_change->getName();

		if (skeleton_entity->hasAnimationState(animation_name)) {
			current_animation_state = skeleton_entity->getAnimationState(animation_name);
			current_animation_state->setLoop(true);
			current_animation_state->setEnabled(true);
		}
		else {
			SHOW_ERROR("The skeleton doesn't have this animation for some reason!");
		}
	}

	animation_to_change = NULL;
}

void ESKOgre::destroyResources() {
	if (resources_created) {
		Ogre::SkeletonManager::getSingleton().remove(name);
		Ogre::MeshManager::getSingleton().remove(name + "_skeleton");
		OgreUtil::destroySceneNodeChildren(skeleton_node);
		skeleton_node->getCreator()->destroySceneNode(skeleton_node);
	}

	resources_created = false;
}

ESKOgre::~ESKOgre() {
	destroyResources();
}