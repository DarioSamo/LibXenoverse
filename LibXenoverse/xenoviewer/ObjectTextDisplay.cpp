#include "ObjectTextDisplay.h"

Ogre::Vector3 GetBoneWorldPosition(Ogre::Entity* ent, Ogre::Bone* bone)
{
	Ogre::Vector3 world_position = bone->_getDerivedPosition();

	//multiply with the parent derived transformation
	Ogre::Node *pParentNode = ent->getParentNode();
	Ogre::SceneNode *pSceneNode = ent->getParentSceneNode();
	while (pParentNode != NULL)
	{
		//process the current i_Node
		if (pParentNode != pSceneNode)
		{
			//this is a tag point (a connection point between 2 entities). which means it has a parent i_Node to be processed
			world_position = pParentNode->_getFullTransform() * world_position;
			pParentNode = pParentNode->getParent();
		}
		else
		{
			//this is the scene i_Node meaning this is the last i_Node to process
			world_position = pParentNode->_getFullTransform() * world_position;
			break;
		}
	}
	return world_position;
}

void ObjectTextDisplay::update()
{
	if (!m_enabled)
		return;

	Ogre::Vector3 v = GetBoneWorldPosition(m_ent, m_p);
	Ogre::Vector3 ScreenPos = m_c->getProjectionMatrix() * m_c->getViewMatrix() * v;
	ScreenPos.x = (ScreenPos.x) / 2;
	ScreenPos.y = (ScreenPos.y) / 2;

	float max_x = ScreenPos.x;
	float min_y = -ScreenPos.y;

	m_pContainer->setPosition(max_x, min_y + 0.5);
}