#include "ViewerGrid.h"

ViewerGrid::ViewerGrid() {
}

Ogre::SceneNode *ViewerGrid::createSceneNode(Ogre::SceneManager *mSceneMgr) {
	Ogre::SceneNode *gridRootNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	float line_width = 0.005f;
	float grid_size_f = 0.2f;
	int grid_size_x = 100;
	int grid_size_y = 100;
	float grid_length_x = (grid_size_x * grid_size_f) / 2;
	float grid_length_y = (grid_size_y * grid_size_f) / 2;

	for (int i = 0; i <= grid_size_x; i++)
	{
		Ogre::ManualObject *manual = static_cast<Ogre::ManualObject*>(mSceneMgr->createMovableObject("GridLineZ" + ToString(i + 1), Ogre::ManualObjectFactory::FACTORY_TYPE_NAME));
		manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

		manual->position(-line_width, line_width, -grid_length_x);
		manual->position(-line_width, line_width, grid_length_x);
		manual->position(0.0, line_width, -grid_length_x);
		manual->position(0.0, line_width, grid_length_x);

		manual->end();

		Ogre::SceneNode *gridNode = gridRootNode->createChildSceneNode();
		gridNode->setPosition(-grid_length_x + i*grid_size_f, 0.0, 0.0);
		gridNode->attachObject(manual);
	}

	for (int i = 0; i <= grid_size_y; i++)
	{
		Ogre::ManualObject *manual = static_cast<Ogre::ManualObject*>(mSceneMgr->createMovableObject("GridLineX" + ToString(i + 1), Ogre::ManualObjectFactory::FACTORY_TYPE_NAME));
		manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

		manual->position(-grid_length_y, 0.0, 0.0);
		manual->position(grid_length_y, 0.0, 0.0);
		manual->position(-grid_length_y, 0.0, -line_width);
		manual->position(grid_length_y, 0.0, -line_width);

		manual->end();

		Ogre::SceneNode *gridNode = gridRootNode->createChildSceneNode();
		gridNode->setPosition(0.0, 0.0, -grid_length_y + i*grid_size_f);
		gridNode->attachObject(manual);
	}

	return gridRootNode;
}