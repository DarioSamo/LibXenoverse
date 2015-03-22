#ifndef VIEWER_GRID_H_INCLUDED
#define VIEWER_GRID_H_INCLUDED

#define XENOVIEWER_GRID_ENTITY_NAME   "ViewerGrid"

class ViewerGrid {
	protected:
	public:
		ViewerGrid();
		Ogre::SceneNode *createSceneNode(Ogre::SceneManager *mSceneMgr);
};

#endif