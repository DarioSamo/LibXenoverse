#ifndef ESKOGRE_H_INCLUDED
#define ESKOGRE_H_INCLUDED

class ESKOgre : public ESK {
protected:
	Ogre::Skeleton *skeleton;
	bool resources_created;
	Ogre::SceneNode *skeleton_node;
	Ogre::Entity *skeleton_entity;
	EANAnimation *animation_to_change;
	Ogre::AnimationState *current_animation_state;
	bool to_delete;
public:
	ESKOgre();
	~ESKOgre();

	void buildBone(unsigned short b, Ogre::Skeleton *ogre_skeleton, Ogre::Bone *parent_bone);
	Ogre::Skeleton *createOgreSkeleton(Ogre::SceneManager *mSceneMgr);
	void createFakeEntity(Ogre::SceneManager *mSceneMgr);
	void refreshAnimations();

  Ogre::SceneNode* getOgreSceneNode()
  {
    return skeleton_node;
  }

	Ogre::Skeleton *getOgreSkeleton() {
		return skeleton;
	}

	Ogre::Entity *getEntity() {
		return skeleton_entity;
	}

	void tagAnimationChange(EANAnimation *new_animation) {
		animation_to_change = new_animation;
	}

	void tagForDeletion() {
		to_delete = true;
	}

	bool toDelete() {
		return to_delete;
	}

	bool changedAnimation() {
		if (animation_to_change) {
			return true;
		}

		return false;
	}

	void changeAnimation();

	Ogre::AnimationState *getCurrentAnimationState() {
		return current_animation_state;
	}

	void destroyResources();
};

#endif