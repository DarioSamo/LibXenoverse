#include "ESKOgre.h"

#ifndef EANOGRE_H_INCLUDED
#define EANOGRE_H_INCLUDED

class EANOgre : public EAN {
protected:
	bool animation_resources_created;
	ESKOgre *esk_skeleton;
	float fps;
public:
	EANOgre();

	Ogre::NodeAnimationTrack *createOgreAnimationTrack(Ogre::Animation *mAnim, Ogre::Bone *mBone, EANAnimation *animation, EANAnimationNode *node);
	Ogre::Animation *createOgreAnimation(EANAnimation *animation);
	void createOgreAnimations(ESKOgre *v);
};

#endif