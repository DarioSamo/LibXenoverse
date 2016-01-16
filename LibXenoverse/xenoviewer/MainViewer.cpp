#include "MainViewer.h"
#include "OgreWidget.h"
#include "FileTreeItemWidget.h"
#include "EANOgre.h"
#include "EMBOgre.h"
#include "EMMOgre.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTreeWidgetItem>
#include "OgreCommon.h"
#include "OgreSkeleton.h"
#include <OgreRectangle2D.h>

MainViewer::MainViewer(QWidget *parent)
	: QWidget(parent),
  _textureGraphicsScene(new QGraphicsScene(this))
{
	setupUi(this);

  ddsTextureView->setScene(_textureGraphicsScene);
  ddsTextureView->setSizeAdjustPolicy(QGraphicsView::AdjustToContents);

	FileTree->acceptDrops();
	FileTree->setDragEnabled(true);

	connect(FileTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuFileTree(const QPoint&)));
	connect(FileTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(fileItemDoubleClicked(QTreeWidgetItem *, int)));
	connect(AnimationTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(animationItemDoubleClicked(QTreeWidgetItem *, int)));
	connect(ClearFileTreeButton, SIGNAL(released()), this, SLOT(clearFileTree()));
  connect(ClearAnimationListButton, SIGNAL(released()), this, SLOT(clearAnimTree()));
  connect(ExportOgreButton, SIGNAL(released()), this, SLOT(exportOgre()));
  connect(SaveTextureButton, SIGNAL(released()), this, SLOT(saveTextureToFile()));
  connect(LoadTextureButton, SIGNAL(released()), this, SLOT(loadTextureFromFile()));

	_current_ogre_texture = NULL;

	disableTabs();
}

void MainViewer::fileItemDoubleClicked(QTreeWidgetItem *item, int column) {
  FileTreeItemWidget *item_cast = dynamic_cast<FileTreeItemWidget *>(item);
  Q_ASSERT(item_cast && "FileTreeItemWidget dynamic_cast failed !");

  //Textures
	if (item_cast->getType() == FileTreeItemWidget::ItemTexture) {
    TextureItemWidget *texture_item = dynamic_cast<TextureItemWidget *>(item);
    Q_ASSERT(texture_item && "TextureItemWidget dynamic_cast failed !");
    TexturePackItemWidget *texture_pack_item = dynamic_cast<TexturePackItemWidget *>(texture_item->parent());
    Q_ASSERT(texture_pack_item && "TexturePackItemWidget dynamic_cast failed !");
		if (texture_pack_item) {
			EMBOgre *emb = texture_pack_item->getData();
			EMBFile *emb_file = texture_item->getData();

			if (emb && emb_file) {
        changeCurrentTexture(emb->getOgreTexture(emb_file->getIndex()));
			}
		}
	}

  //Skeleton
  else if (item_cast->getType() == FileTreeItemWidget::ItemSkeleton)
  {
    SkeletonItemWidget* skeleton_item = dynamic_cast<SkeletonItemWidget*>(item);
    Q_ASSERT(skeleton_item && "SkeletonItemWidget cast failed !");
    ESKOgre* esk = skeleton_item->getData();
    changeCurrentSkeleton(esk);
  }

}

void MainViewer::animationItemDoubleClicked(QTreeWidgetItem *item, int column) {
	FileTreeItemWidget *item_cast = static_cast<FileTreeItemWidget *>(item);

	if (item_cast->getType() == FileTreeItemWidget::ItemAnimation) {
		AnimationItemWidget *animation_item_cast = static_cast<AnimationItemWidget *>(item_cast);
		EANAnimation *anim = animation_item_cast->getData();
		EANOgre *anim_pack = static_cast<EANOgre *>(anim->getParent());
		anim_pack->tagForceAnimation(anim);
	}
}

void MainViewer::createFileTreeItems(list<EMDOgre *> &new_emd_list, list<ESKOgre *> &new_esk_list, list<EANOgre *> &new_ean_list) {
	for (list<ESKOgre *>::iterator it = new_esk_list.begin(); it != new_esk_list.end(); it++) {
		SkeletonItemWidget *item_widget = new SkeletonItemWidget(*it, FileTree);
	}

	for (list<EMDOgre *>::iterator it = new_emd_list.begin(); it != new_emd_list.end(); it++) {
		ModelPackItemWidget *item_widget = new ModelPackItemWidget(*it, FileTree);
	}

	for (list<EANOgre *>::iterator it = new_ean_list.begin(); it != new_ean_list.end(); it++) {
		AnimationPackItemWidget *item_widget = new AnimationPackItemWidget(*it, AnimationTree);
	}
}


void MainViewer::keyPressEvent(QKeyEvent * event) {
	QWidget::keyPressEvent(event);
	FileTree->keyPressEvent(event);
	//AnimationTree->keyPressEvent(event);
}

void MainViewer::contextMenuFileTree(const QPoint& point) {
	FileTree->processContextMenu(point);
}

void MainViewer::clearFileTree() {
	FileTree->clear();
}

void MainViewer::disableTabs() {
	tabWidget->setTabEnabled(0, false);
	tabWidget->setTabEnabled(1, false);
	tabWidget->setTabEnabled(2, false);
	tabWidget->setTabEnabled(3, false);
	tabWidget->setTabEnabled(4, false);
	tabWidget->setTabEnabled(5, false);
	tabWidget->setTabEnabled(6, false);
}

void MainViewer::enableTab(int index) {
	//disableTabs();
	tabWidget->setTabEnabled(index, true);
	tabWidget->setCurrentIndex(index);
}

void MainViewer::clearAnimTree()
{
  AnimationTree->clear();
}

void MainViewer::changeCurrentSkeleton(ESKOgre* esk)
{
  skeletonTreeWidget->clear();
  if (esk)
  {
    Ogre::Skeleton* skeleton = esk->getOgreSkeleton();
    skeleton->getName();

    QTreeWidgetItem* skeleton_item = new QTreeWidgetItem();
    skeleton_item->setText(0, QString::fromStdString(skeleton->getName()));
    skeletonTreeWidget->addTopLevelItem(skeleton_item);
   
    Ogre::Bone* root_bone = skeleton->getRootBone();

    std::unordered_map<Ogre::Bone*, QTreeWidgetItem*> todo;
    todo.reserve(esk->getBones().size());
    todo[root_bone] = skeleton_item;


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
        item->setText(0, QString::fromStdString(bone->getName()));
        current_parent_item->addChild(item);
        todo[bone] = item;
      }
    }

    skeletonTreeWidget->expandAll();
    enableSkeletonTab();
  }
  else
  {
    disableSkeletonTab();
  }

}


void MainViewer::changeCurrentTexture(Ogre::Texture* texture)
{
  _current_ogre_texture = texture;
  if (_current_ogre_texture)
  {
    enableTextureTab();
    Ogre::Image image;
    _current_ogre_texture->convertToImage(image);
    QImage qimage(
      image.getData(),
      image.getWidth(),
      image.getHeight(), 
      OgreUtil::ToQtPixelFormat(image.getFormat()));
    QPixmap pixmap;
    pixmap.convertFromImage(qimage);
    _textureGraphicsScene->clear();
    ddsTextureView->update();
    QGraphicsPixmapItem* item = _textureGraphicsScene->addPixmap(pixmap);
    _textureGraphicsScene->setSceneRect(item->sceneBoundingRect());
    ddsTextureView->fitInView(item, Qt::KeepAspectRatio);
    
    //ddsTextureView->centerOn((QGraphicsItem*)item);
  }
  else
  {
    disableTextureTab();
  }
}

void MainViewer::enableSkeletonTab()
{
    enableTab(0);
}

void MainViewer::disableSkeletonTab()
{
    tabWidget->setTabEnabled(0, false);
}


void MainViewer::enableTextureTab()
{
    enableTab(6);
}

void MainViewer::disableTextureTab()
{
    tabWidget->setTabEnabled(6, false);
}


void MainViewer::exportOgre(QTreeWidgetItem* item, int& export_success)
{
  FileTreeItemWidget *item_cast = dynamic_cast<FileTreeItemWidget *>(item);

  if (!item_cast)
  {
    return;
  }

  if (item_cast->getType() == FileTreeItemWidget::ItemTexture) 
  {
    /*
    TextureItemWidget* texture_item = dynamic_cast<TextureItemWidget*>(item_cast);
    if (texture_item)
    {
      std::string filename = item->text(0).toStdString() + ".dds";
      filename = texture_item->
      if (!texture_item->getData()->save(filename))
      {
        QMessageBox::critical(0, QString("Error writing file"), QString(filename.c_str()));
      }
      else
      {
        export_success++;
      }
    }*/
  }
  else if (item_cast->getType() == FileTreeItemWidget::ItemTexturePack) 
  {
  
    TexturePackItemWidget* texture_pack = dynamic_cast<TexturePackItemWidget*>(item_cast);
    if (texture_pack)
    {
      EMBOgre* emb_ogre = texture_pack->getData();
      if (emb_ogre)
      {
        Ogre::Image image;
        std::vector<Ogre::TexturePtr> textures = emb_ogre->getOgreTextures();
        for (int i = 0; i < textures.size(); i++)
        {
          try
          {
            textures[i]->convertToImage(image);
            std::string filename = textures[i]->getName() + ".png";
            image.save(filename);
            export_success++;
          }
          catch (Ogre::Exception& e)
          {
            QMessageBox::critical(0, "Failed to save texture", QString::fromStdString(e.getFullDescription()));
          }
        }

        std::vector<Ogre::GpuProgramPtr> shaders = emb_ogre->getOgreShaders();
        for (int i = 0; i < shaders.size(); i++)
        {
          std::ofstream outfile(shaders[i]->getSourceFile(), std::ios::out | std::ios::trunc);
          if (!(outfile << shaders[i]->getSource()))
          {
            export_success++;
          }
          else
          {
            QMessageBox::critical(0, "Failed to save shader", QString("Failed to save: ") + QString::fromStdString(shaders[i]->getSourceFile()));
          }
        }
      }
    }
    else if (item_cast->getType() == FileTreeItemWidget::ItemMaterialPack)
    {
      MaterialPackItemWidget* material_pack = dynamic_cast<MaterialPackItemWidget*>(item_cast);
      if (material_pack)
      {
        EMMOgre* emm = material_pack->getData();

      }
    }

  }
  

  for (int i = 0; i < item->childCount(); i++)
  {
    exportOgre(item->child(i), export_success);
  }
}

void MainViewer::exportOgre()
{
  int export_success = 0;
  for (int i = 0; i < FileTree->topLevelItemCount(); i++)
  {
    exportOgre(FileTree->topLevelItem(i), export_success);
  }
  if (export_success > 0)
  {
    QMessageBox::information(0, "Export results", QString("Exported ") + QString::number(export_success) + QString(" files with success"), QMessageBox::Ok);
  }
  else
  {
    QMessageBox::critical(0, "Export failed", QString("Failed to export anything !"), QMessageBox::Ok);
  }
}

void ExtractPathFilename(const QString i, QString& p, QString& f)
{

}

void MainViewer::loadTextureFromFile()
{
  if (_current_ogre_texture)
  {

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Texture File"),
                                                    QString(_current_ogre_texture->getName().c_str()) + QString(".dds"),
                                                    tr("Images (*.dds *.png *.jpg *.tga)"));
      
    Ogre::Image image;
    try
    {

      QFileInfo infos(fileName);
     
      
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(infos.canonicalPath().toStdString(), "FileSystem" , XENOVIEWER_RESOURCE_GROUP);
      Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(XENOVIEWER_RESOURCE_GROUP);
      image.load(infos.fileName().toStdString(), XENOVIEWER_RESOURCE_GROUP);
      _current_ogre_texture->loadImage(image);
      changeCurrentTexture(_current_ogre_texture);
    }
    catch (Ogre::Exception& e)
    {
      QMessageBox::critical(NULL, QString("Error !"), QString(e.getFullDescription().c_str()));
    }

  }
}

void MainViewer::saveTextureToFile()
{
  
  if (_current_ogre_texture)
  {
    Ogre::Image image;
    _current_ogre_texture->convertToImage(image);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Texture File"),
                                                    QString(_current_ogre_texture->getName().c_str()) + QString(".dds"),
                                                    tr("Images (*.dds *.png *.jpg *.tga)"));
    if (!fileName.isEmpty())
    {
      try {
        image.save(Ogre::String(fileName.toStdString()));
      }
      catch (Ogre::Exception& e)
      {
        QMessageBox::critical(NULL, QString("Error !"), QString(e.getFullDescription().c_str()));
      }
    }
  }

}