#ifndef ESKOGRE_H_INCLUDED
#define ESKOGRE_H_INCLUDED

class ESKOgre : public ESK {
protected:
	Ogre::Skeleton *skeleton;
	bool resources_created;
	Ogre::Entity *shared_entity;
public:
	ESKOgre();

	void buildBone(unsigned short b, Ogre::Skeleton *ogre_skeleton, Ogre::Bone *parent_bone);
	Ogre::Skeleton *createOgreSkeleton();

	void setSharedEntity(Ogre::Entity *v) {
		shared_entity = v;
	}

	Ogre::Entity *getSharedEntity() {
		return shared_entity;
	}

	Ogre::Skeleton *getOgreSkeleton() {
		return skeleton;
	}
};

#endif