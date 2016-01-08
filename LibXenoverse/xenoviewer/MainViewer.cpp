#include "MainViewer.h"
#include "OgreWidget.h"
#include "FileTreeItemWidget.h"
#include "EANOgre.h"
#include "EMBOgre.h"

#include <QgraphicsScene>
#include "OgreCommon.h"
#include <OgreRectangle2D.h>

MainViewer::MainViewer(QWidget *parent)
	: QWidget(parent),
  _textureGraphicsScene(new QGraphicsScene(this))
{
	setupUi(this);

  ddsTextureView->setScene(_textureGraphicsScene);

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
	FileTreeItemWidget *item_cast = static_cast<FileTreeItemWidget *>(item);

	if (item_cast->getType() == FileTreeItemWidget::ItemTexture) {
		TextureItemWidget *texture_item = static_cast<TextureItemWidget *>(item);
		TexturePackItemWidget *texture_pack_item = static_cast<TexturePackItemWidget *>(texture_item->parent());
		if (texture_pack_item) {
			EMBOgre *emb = texture_pack_item->getData();
			EMBFile *emb_file = texture_item->getData();

			if (emb && emb_file) {
        changeCurrentTexture(emb->getOgreTexture(emb_file->getIndex()));
			}
		}
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
	disableTabs();
	tabWidget->setTabEnabled(index, true);
	tabWidget->setCurrentIndex(index);
}

void MainViewer::clearAnimTree()
{
  AnimationTree->clear();
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
    QGraphicsPixmapItem* item = _textureGraphicsScene->addPixmap(pixmap);
    ddsTextureView->ensureVisible((QGraphicsItem*)item,0,0);

  }
  else
  {
    disableTextureTab();
  }
}

void MainViewer::enableTextureTab()
{
    enableTab(6);
}

void MainViewer::disableTextureTab()
{
    tabWidget->setTabEnabled(6, false);
}

void MainViewer::exportOgre()
{


  for each (QTreeWidgetItem* item in FileTree->selectedItems())
  {
    FileTreeItemWidget *item_cast = static_cast<FileTreeItemWidget *>(item);
    
    if (item_cast->getType() == FileTreeItemWidget::ItemTexture) {

      std::string filename = item->text(0).toStdString() + ".dds";
      TextureItemWidget* texture_item = static_cast<TextureItemWidget*>(item_cast);
      if (!texture_item->getData()->save(filename))
      {
        QMessageBox::critical(0, QString("Error writing file"), QString(filename.c_str()));
      }
    }

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