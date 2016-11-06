#include "OgreSkeletonModifier.h"

void OgreSkeletonModifier::regenerateRootBones(const Ogre::Skeleton * skeleton)
{
  // ugly isn't it 
  skeleton->deriveRootBone();
}

bool OgreSkeletonModifier::renameBone(Ogre::Skeleton * skeleton, Ogre::Bone * bone, const Ogre::String & newName)
{
  if (skeleton->hasBone(newName) || !skeleton->hasBone(bone->getName()))
  {
    return false;
  }

  skeleton->mBoneListByName.erase(bone->getName());
  Ogre::Node* parentBone = bone->getParent();
  if (parentBone)
    parentBone->removeChild(bone->getName());

  bone->mName = newName;

  if (parentBone)
    parentBone->addChild(bone);
  skeleton->mBoneListByName[newName] = bone;

  return true;
}


void OgreSkeletonModifier::removeBone(Ogre::Skeleton* skeleton, Ogre::Bone* bone)
{
  Q_ASSERT(skeleton->hasBone(bone->getName()));

  while (bone->mChildren.size())
  {
    Ogre::Bone* child = dynamic_cast<Ogre::Bone*>((bone->mChildren.begin())->second);
    Q_ASSERT(child);
    removeBone(skeleton, child);
  }

  skeleton->mBoneListByName.erase(bone->getName());
  
  std::vector<Ogre::Bone*>::iterator it;
  std::vector<Ogre::Bone*>::iterator to_erase;
  int decrement = 0;
  for (it = skeleton->mBoneList.begin(); it != skeleton->mBoneList.end(); ++it)
  {
    if (*it == bone)
    {
      decrement++;
      to_erase = it;
    }
    else
    {
      (*it)->mHandle -= decrement;
    }
  }
  delete *to_erase;
  skeleton->mBoneList.erase(to_erase);

  regenerateRootBones(skeleton);
}

bool OgreSkeletonModifier::additiveMergeBone(Ogre::Skeleton& targetSkeleton, const Ogre::Skeleton& sourceSkeleton, const Ogre::String& commonBoneName, Ogre::Vector3& deltaPosition, Ogre::String& failureReason)
{
  if (!targetSkeleton.hasBone(commonBoneName))
  {
    failureReason = "Target skeleton " + targetSkeleton.getName() + " doesn't have a bone called " + commonBoneName;
    return false;
  }
  else if (!sourceSkeleton.hasBone(commonBoneName))
  {
    failureReason = "Source skeleton " + sourceSkeleton.getName() + " doesn't have a bone called " + commonBoneName;
    return false;
  }

  if (targetSkeleton.getNumAnimations() != 0 || sourceSkeleton.getNumAnimations() != 0)
  {
    failureReason = "Additive merge of 2 skeleton with already loaded animations are not supported yet please load animations after merging";
    return false;
  }

  Ogre::Vector3 dSourcePos = sourceSkeleton.getBone(commonBoneName)->_getDerivedPosition();
  Ogre::Quaternion dSourceRot = sourceSkeleton.getBone(commonBoneName)->_getDerivedOrientation();
  Ogre::Vector3 dSourceScale = sourceSkeleton.getBone(commonBoneName)->_getDerivedScale();
  Ogre::Vector3 dTargetPos = targetSkeleton.getBone(commonBoneName)->_getDerivedPosition();
  Ogre::Quaternion dTargetRot = targetSkeleton.getBone(commonBoneName)->_getDerivedOrientation();
  Ogre::Vector3 dTargetScale = targetSkeleton.getBone(commonBoneName)->_getDerivedScale();

  qDebug() << "Source derived pos" << dSourcePos.x << dSourcePos.y << dSourcePos.z;
  qDebug() << "Source derived rot" << dSourceRot.x << dSourceRot.y << dSourceRot.z << dSourceRot.w;
  qDebug() << "Source derived scale" << dSourceScale.x << dSourceScale.y << dSourceScale.z;
  qDebug() << "Target derived pos" << dTargetPos.x << dTargetPos.y << dTargetPos.z;
  qDebug() << "Target derived rot" << dTargetRot.x << dTargetRot.y << dTargetRot.z << dTargetRot.w;
  qDebug() << "Target derived scale" << dTargetScale.x << dTargetScale.y << dTargetScale.z;

  deltaPosition = dTargetPos - dSourcePos;

  std::map<Ogre::String, Ogre::String> todoBones;
  Ogre::String parentBoneName = commonBoneName;
  Ogre::Bone* sourceParentBone = sourceSkeleton.getBone(parentBoneName);
  {
    auto childIt = sourceParentBone->getChildIterator();
    while (childIt.hasMoreElements())
    {
      todoBones[childIt.current()->first] = parentBoneName;
      childIt.moveNext();
    }
  }

  while (!todoBones.empty())
  {
    Ogre::String currentBoneName = todoBones.begin()->first;
    parentBoneName = todoBones.begin()->second;
    todoBones.erase(todoBones.begin());

    Ogre::Bone* parentTargetBone = targetSkeleton.getBone(parentBoneName);
    Ogre::Bone* parentSourceBone = sourceSkeleton.getBone(parentBoneName);
    Ogre::Bone* currentSourceBone = sourceSkeleton.getBone(currentBoneName);
    if (targetSkeleton.hasBone(currentBoneName))
    {
      failureReason = "Additive merge failed bone " + currentBoneName + " already exists in branch of " + targetSkeleton.getName() + " !";
      return false;
    }
    Ogre::Bone* currentTargetBone = targetSkeleton.createBone(currentBoneName);
    parentTargetBone->addChild(currentTargetBone);
    //Q_ASSERT(!currentSourceBone->mInheritOrientation);
    //Q_ASSERT(!currentSourceBone->mInheritScale);
    //currentTargetBone->mInheritOrientation = currentSourceBone->mInheritOrientation;
    //currentTargetBone->mInheritScale = currentSourceBone->mInheritScale;
    //currentTargetBone->mOrientation = currentSourceBone->mOrientation;
    currentTargetBone->mPosition = currentSourceBone->mPosition;
    //currentTargetBone->mScale = currentSourceBone->mScale;
    Q_ASSERT(currentSourceBone->mInitialPosition == currentSourceBone->mPosition);
    Q_ASSERT(currentSourceBone->mInitialOrientation == currentSourceBone->mOrientation);
    Q_ASSERT(currentSourceBone->mInitialScale == currentSourceBone->mScale);
    //currentTargetBone->mInitialOrientation = currentSourceBone->mInitialOrientation;
    currentTargetBone->mInitialPosition = currentSourceBone->mInitialPosition;
    //currentTargetBone->mInitialScale = currentSourceBone->mInitialScale;
    currentTargetBone->needUpdate(true);

    auto childIt = currentSourceBone->getChildIterator();
    while (childIt.hasMoreElements())
    {
      todoBones[childIt.current()->first] = currentBoneName;
      childIt.moveNext();
    }
  }
  return true;
}
