#include "BonesTreeWidget.h"
#include "ESKOgre.h"
#include "OgreSkeletonModifier.h"

#include <QPoint>

BonesTreeWidget::BonesTreeWidget(QWidget *parent) : 
  QTreeWidget(parent), 
  _esk(NULL)
{
  connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(on_itemChanged(QTreeWidgetItem*, int)));
}

void BonesTreeWidget::setESK(ESKOgre* esk)
{
  _esk = esk;
  regenerateTree();
}



static QDataStream & operator<< (QDataStream& stream, QTreeWidgetItem* item)
{
  stream.writeBytes((const char*)&item, sizeof(QTreeWidgetItem*));
  return stream;
}

static QDataStream & operator >> (QDataStream& stream, QTreeWidgetItem*& item)
{
  uint len;
  char* buf = NULL;
  stream.readBytes(buf, len);
  Q_ASSERT(len == sizeof(QTreeWidgetItem*));
  Q_ASSERT(buf);
  item = *((QTreeWidgetItem**)buf);
  delete[] buf;
  return stream;
}

void BonesTreeWidget::dropEvent(QDropEvent * event)
{
  Q_ASSERT(event->dropAction() == Qt::MoveAction);

  QTreeWidgetItem* targetItem = itemAt(event->pos());
  const QMimeData* mimeData = event->mimeData();
  QByteArray array = mimeData->data("application/x-bonetreewidgetitem");
  QDataStream stream(&array, QIODevice::ReadOnly);
  QList<QTreeWidgetItem*> items;
  stream >> items;
  moveItems(targetItem, items);
}

void BonesTreeWidget::moveItems(QTreeWidgetItem * newParent, const QList<QTreeWidgetItem*>& items)
{
  if (newParent)
  {
    QVariant parentData = newParent->data(0, Qt::UserRole);
    Ogre::Bone* newParentBone = (Ogre::Bone*)parentData.value<void*>();
    foreach(QTreeWidgetItem* item, items)
    {
      QVariant itemData = item->data(0, Qt::UserRole);
      Ogre::Bone* bone = (Ogre::Bone*)itemData.value<void*>();
      Ogre::Node* oldParentBone = bone->getParent();
      if (oldParentBone)
      {
        oldParentBone->removeChild(bone);
      }
      if (newParentBone)
      {
        newParentBone->addChild(bone);
      }
      if (item->parent())
      {
        item->parent()->removeChild(item);
      }
      newParent->addChild(item);
      OgreSkeletonModifier::regenerateRootBones(_esk->getOgreSkeleton());
    }
  }
  else
  {
    foreach(QTreeWidgetItem* item, items)
    {
      QVariant itemData = item->data(0, Qt::UserRole);
      Ogre::Bone* bone = (Ogre::Bone*)itemData.value<void*>();
      Ogre::Node* oldParentBone = bone->getParent();
      if (oldParentBone)
      {
        oldParentBone->removeChild(bone);
      }
      if (item->parent())
      {
        item->parent()->removeChild(item);
      }
      addTopLevelItem(item);
      OgreSkeletonModifier::regenerateRootBones(_esk->getOgreSkeleton());
    }
  }
}

QStringList BonesTreeWidget::mimeTypes() const
{
  QStringList list;
  list << "application/x-bonetreewidgetitem";
  return list;
}

void BonesTreeWidget::regenerateTree()
{
  Ogre::Skeleton* skeleton = _esk->getOgreSkeleton();
  skeleton->getName();

  std::unordered_map<Ogre::Bone*, QTreeWidgetItem*> todo;
  todo.reserve(_esk->getBones().size());
  
  Ogre::Skeleton::BoneIterator rootIt = skeleton->getRootBoneIterator();
  while (rootIt.hasMoreElements())
  {
    Ogre::Bone* currentBone = *(rootIt.current());
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | item->flags());
    item->setText(0, QString::fromStdString(currentBone->getName()));
    item->setData(0, Qt::UserRole, qVariantFromValue((void*)currentBone));
    item->setData(0, Qt::ToolTipRole, tr("index = ") + QString::number(currentBone->getHandle()));


    addTopLevelItem(item);
    todo[currentBone] = item;
    rootIt.moveNext();
  }

  while (!todo.empty())
  {
    Ogre::Bone* current_parent_bone = todo.begin()->first;
    QTreeWidgetItem* current_parent_item = todo.begin()->second;
    todo.erase(todo.begin());

    Ogre::Bone::ChildNodeIterator it = current_parent_bone->getChildIterator();
    while (it.hasMoreElements())
    {
      Ogre::Bone* bone = dynamic_cast<Ogre::Bone*>(it.getNext());
      Q_ASSERT(bone && "Ogre::Bone cast Failed !");
      QTreeWidgetItem* item = new QTreeWidgetItem();
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | item->flags());
      item->setText(0, QString::fromStdString(bone->getName()));
      item->setData(0, Qt::UserRole, qVariantFromValue((void*)bone));
      item->setData(0, Qt::ToolTipRole, tr("index = ") + QString::number(bone->getHandle()));
      current_parent_item->addChild(item);
      todo[bone] = item;
    }
  }
}

QMimeData * BonesTreeWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
  if (items.isEmpty())
    return 0;
  
  QMimeData* mimeData = new QMimeData;
  QByteArray array;
  QDataStream stream(&array, QIODevice::WriteOnly);
  stream << items;
  mimeData->setData("application/x-bonetreewidgetitem",array);
  return mimeData;
}


void BonesTreeWidget::on_itemChanged(QTreeWidgetItem* item, int col)
{
  Q_ASSERT(item);
  Q_ASSERT(col == 0);

  QVariant data = item->data(col, Qt::UserRole);
  Ogre::Bone* bone = (Ogre::Bone*)data.value<void*>();
  if (!OgreSkeletonModifier::renameBone(_esk->getOgreSkeleton(), bone, item->text(col).toStdString()))
  {
    item->setText(col, QString::fromStdString(bone->getName()));
  }
}

void BonesTreeWidget::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Delete)
  {
    removeSelectedBones();
  }
  else
  {
    QTreeWidget::keyPressEvent(event);
  }
}

void BonesTreeWidget::removeSelectedBones()
{
  QList<QTreeWidgetItem*> selected_bones = selectedItems();
  
  foreach(QTreeWidgetItem* item, selected_bones)
  {
    QTreeWidgetItem* parent = item->parent();
    while (parent)
    {
      if (selected_bones.indexOf(parent) != -1)
      {
        selected_bones.removeAll(item);
        break;
      }
      parent = parent->parent();
    }
  }

  foreach(QTreeWidgetItem* item, selected_bones)
  {
    QVariant data = item->data(0, Qt::UserRole);
    Ogre::Bone* bone = (Ogre::Bone*)data.value<void*>();
    Q_ASSERT(bone);
    OgreSkeletonModifier::removeBone(_esk->getOgreSkeleton(), bone);
  }
  clear();
  regenerateTree();
}
