
#include "SkeletonMergingWidget.h"
#include "OgreSkeletonModifier.h"

#include <OgreSkeleton.h>
#include <OgreSkeletonManager.h>



SkeletonMergingWidget::SkeletonMergingWidget(QWidget *parent):
  QDialog(parent,Qt::Dialog)
{
  setupUi(this);
}

void SkeletonMergingWidget::setTargetSkeleton(Ogre::Skeleton* target)
{
  targetSkeleton = target;
  targetSkeletonName->setText(QString::fromStdString(target->getName()));
  fillSourceComboBox();
}

void SkeletonMergingWidget::setSourceSkeleton(Ogre::Skeleton* source)
{
  sourceSkeleton = source;
  fillCommonBonesList(targetSkeleton, sourceSkeleton);
}

void SkeletonMergingWidget::on_ogreMergeButton_clicked(bool)
{
  if (!sourceSkeleton || !targetSkeleton)
  {
    QMessageBox::critical(this, tr("Merge error"), tr("there aren't 2 skeleton to merge"), QMessageBox::Cancel);
    reject();
  }
  else
  {
    Ogre::Skeleton::BoneHandleMap boneMap;
    try
    {
      targetSkeleton->_buildMapBoneByName(sourceSkeleton, boneMap);
      targetSkeleton->_mergeSkeletonAnimations(sourceSkeleton, boneMap);
      accept();
    }
    catch (const Ogre::Exception& e)
    {
      QMessageBox::critical(this, tr("Merge error"), QString::fromStdString(e.getFullDescription()), QMessageBox::Cancel);
      reject();
    }
  }
}

void SkeletonMergingWidget::on_additiveMergeButton_clicked(bool)
{
  if (!sourceSkeleton || !targetSkeleton)
  {
    QMessageBox::critical(this, tr("Merge error"), tr("there aren't 2 skeleton to merge"), QMessageBox::Cancel);
    reject();
  }
  else if (!this->commonBonesList->currentItem())
  {
    QMessageBox::critical(this, tr("Merge error"), tr("Please select the root bone using the common bone list"), QMessageBox::Cancel);
  }
  else
  {
    Ogre::String failureReason;
    const int numBonesBefore = targetSkeleton->getNumBones();
    Ogre::Vector3 deltaPosition;
    if (!OgreSkeletonModifier::additiveMergeBone(*targetSkeleton, *sourceSkeleton, commonBonesList->currentItem()->text().toStdString(), deltaPosition, failureReason))
    {
      QMessageBox::critical(this, tr("Merge error"), QString::fromStdString(failureReason), QMessageBox::Cancel);
    }
    else
    {
      const int numBonesAfter = targetSkeleton->getNumBones();
      QMessageBox::information(this, tr("Merge success"), tr("Added ") + QString::number(numBonesAfter - numBonesBefore) + 
                                     tr(" bones from ") + QString::fromStdString(sourceSkeleton->getName()) + tr(" to ") + 
                                     QString::fromStdString(targetSkeleton->getName()) + tr("\nPlease move apply the following transformation on the source mesh: ") + 
                                     tr("\nDelta Position: ") + QString::number(deltaPosition.x) + ", " + QString::number(deltaPosition.y) + ", " + QString::number(deltaPosition.z), QMessageBox::Ok);
      accept();
    }
  }
}

void SkeletonMergingWidget::fillSourceComboBox()
{
  sourceSkeletonComboBox->clear();
  if (targetSkeleton)
  {
    Ogre::SkeletonManager* manager = Ogre::SkeletonManager::getSingletonPtr();
    Ogre::SkeletonManager::ResourceMapIterator it = manager->getResourceIterator();
    QStringList skeletons;
    while (it.hasMoreElements())
    {
      if (it.current()->first != targetSkeleton->getHandle())
      {
        Ogre::String name = it.current()->second.get()->getName();
        skeletons += QString::fromStdString(name);
      }
      it.moveNext();
    }
    sourceSkeletonComboBox->addItems(skeletons);
  }
}

void SkeletonMergingWidget::fillCommonBonesList(Ogre::Skeleton * target, Ogre::Skeleton * source)
{
  commonBonesList->clear();
  if (target && source)
  {
    QSet<QString> boneNames[2];
    Ogre::Skeleton* skeletons[2] = {target, source};
    for (int i = 0; i < 2; ++i)
    {
      Ogre::Skeleton::BoneIterator it = skeletons[i]->getBoneIterator();
      while (it.hasMoreElements())
      {
        Ogre::Skeleton::BoneIterator::const_iterator bit = it.current();
        boneNames[i].insert(QString::fromStdString((*bit)->getName()));
        it.moveNext();
      }
    }
    QStringList commonBones = QStringList::fromSet(boneNames[0] & boneNames[1]);
    commonBonesList->addItems(commonBones);
  }
}



void SkeletonMergingWidget::on_sourceSkeletonComboBox_currentIndexChanged(int index)
{
  if (index == -1)
  {
    setSourceSkeleton(NULL);
  }
  else
  {
    Ogre::SkeletonManager* manager = Ogre::SkeletonManager::getSingletonPtr();
    setSourceSkeleton(manager->getByName(sourceSkeletonComboBox->itemText(index).toStdString()).getPointer());
  }
}

