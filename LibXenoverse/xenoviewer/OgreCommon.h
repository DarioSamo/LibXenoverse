

#include <QImage>
#include <Ogre.h>

#ifndef OGRE_COMMON_H_INCLUDED
#define OGRE_COMMON_H_INCLUDED


namespace OgreUtil {
	void destroyAllAttachedMovableObjects(Ogre::SceneNode* node);
	void destroySceneNodeChildren(Ogre::SceneNode* node);

   QImage::Format ToQtPixelFormat(Ogre::PixelFormat format);
}

#endif