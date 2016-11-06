
#ifndef OGRE_SKELETON_MODIFIER_H_INCLUDED
#define OGRE_SKELETON_MODIFIER_H_INCLUDED

#include <OgreSkeleton.h>

class OgreSkeletonModifier
{
private:

 public:

  static void regenerateRootBones(const Ogre::Skeleton* skeleton);
  static bool renameBone(Ogre::Skeleton* skeleton, Ogre::Bone* bone, const Ogre::String& newName);
  static void removeBone(Ogre::Skeleton* skeleton, Ogre::Bone* bone);
  static bool additiveMergeBone(Ogre::Skeleton& targetSkeleton, const Ogre::Skeleton& sourceSkeleton, const Ogre::String& commonBoneName, Ogre::Vector3& deltaPosition, Ogre::String& failureReason);
};


#endif