#include <OgreCommon.h>

namespace OgreUtil {
	void destroyAllAttachedMovableObjects(Ogre::SceneNode* node) {
		if (!node) return;

		// Destroy all the attached objects
		Ogre::SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator();

		while (itObject.hasMoreElements())
			node->getCreator()->destroyMovableObject(itObject.getNext());

		// Recurse to child SceneNodes
		Ogre::SceneNode::ChildNodeIterator itChild = node->getChildIterator();

		while (itChild.hasMoreElements())
		{
			Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
			destroyAllAttachedMovableObjects(pChildNode);
		}
	}

	void destroySceneNodeChildren(Ogre::SceneNode* node) {
		if (!node) return;
		destroyAllAttachedMovableObjects(node);
		node->removeAndDestroyAllChildren();
	}


  QImage::Format ToQtPixelFormat(Ogre::PixelFormat format)
  {
    switch (format)
    {
    case Ogre::PF_BYTE_RGBA:
    case Ogre::PF_R8G8B8A8:
    case Ogre::PF_R8G8B8A8_UINT:
    case Ogre::PF_R8G8B8A8_SINT:
    case Ogre::PF_R8G8B8A8_SNORM:
      return QImage::Format_RGBA8888;

    case Ogre::PF_BYTE_RGB:
    case Ogre::PF_R8G8B8:
    case Ogre::PF_R8G8B8_UINT:
    case Ogre::PF_R8G8B8_SINT:
    case Ogre::PF_R8G8B8_SNORM:
      return QImage::Format_RGB888;

    case Ogre::PF_A8R8G8B8:
      return QImage::Format_ARGB32;

    case Ogre::PF_X8R8G8B8:
      return QImage::Format_RGB32;
    
    case Ogre::PF_R5G6B5:
      return QImage::Format_RGB16;

    default:
      //Q_ASSERT(0 && "Error Ogre::Texture must be converted in proper qt format");
      return QImage::Format_Invalid;
    }
  }
}