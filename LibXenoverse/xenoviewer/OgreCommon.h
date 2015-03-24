#ifndef OGRE_COMMON_H_INCLUDED
#define OGRE_COMMON_H_INCLUDED

namespace OgreUtil {
	void destroyAllAttachedMovableObjects(Ogre::SceneNode* node);
	void destroySceneNodeChildren(Ogre::SceneNode* node);
}

#endif