#include "ui_SkeletonMergingWidget.h"

class Ogre::Skeleton;

class SkeletonMergingWidget : public QDialog, private Ui::SkeletonMergingWidget
{
	Q_OBJECT
public:
	SkeletonMergingWidget(QWidget *parent = Q_NULLPTR);

public slots:
  void setTargetSkeleton(Ogre::Skeleton* target);
  void setSourceSkeleton(Ogre::Skeleton* source);

protected slots:
  void on_sourceSkeletonComboBox_currentIndexChanged(int index);
  void on_ogreMergeButton_clicked(bool);
  void on_additiveMergeButton_clicked(bool);

protected:
  void fillSourceComboBox();
  void fillCommonBonesList(Ogre::Skeleton* target, Ogre::Skeleton* source);

  Ogre::Skeleton* targetSkeleton;
  Ogre::Skeleton* sourceSkeleton;


};
