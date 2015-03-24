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
}