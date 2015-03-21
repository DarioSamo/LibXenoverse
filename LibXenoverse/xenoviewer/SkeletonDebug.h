#ifndef SKELETONDEBUG_H_INCLUDED
#define SKELETONDEBUG_H_INCLUDED

#include <Ogre.h>
#include <OgreTagPoint.h>
#include <vector>
#include "ObjectTextDisplay.h"

class SkeletonDebug
{
public:
	SkeletonDebug(Ogre::Entity *entity, Ogre::SceneManager *man, Ogre::Camera *cam, float boneSize = 0.1f);
	~SkeletonDebug();

	void setAxesScale(Ogre::Real scale){ mScaleAxes = scale; }
	Ogre::Real getAxesScale(){ return mScaleAxes; }

	void showAxes(bool show);
	void showNames(bool show);
	void showBones(bool show);
	bool axesShown(){ return mShowAxes; }
	bool namesShown(){ return mShowNames; }
	bool bonesShown(){ return mShowBones; }

	void update();

private:
	std::vector<Ogre::Entity*> mAxisEntities;
	std::vector<Ogre::Entity*> mBoneEntities;
	std::vector<ObjectTextDisplay*> mTextOverlays;

	float mBoneSize;

	Ogre::Entity *mEntity;
	Ogre::MaterialPtr mAxisMatPtr;
	Ogre::MaterialPtr mBoneMatPtr;
	Ogre::MeshPtr mBoneMeshPtr;
	Ogre::MeshPtr mAxesMeshPtr;
	Ogre::SceneManager *mSceneMan;
	Ogre::Camera *mCamera;

	Ogre::Real mScaleAxes;

	bool mShowAxes;
	bool mShowBones;
	bool mShowNames;

	void createAxesMaterial();
	void createBoneMaterial();
	void createAxesMesh();
	void createBoneMesh();
};

#endif // SKELETONDEBUG_H_INCLUDED