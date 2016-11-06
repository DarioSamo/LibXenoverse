#include "MainViewer.h"
#include "OgreWidget.h"
#include "SkeletonMergingWidget.h"

#include "FileTreeItemWidget.h"
#include "EANOgre.h"
#include "EMBOgre.h"
#include "EMMOgre.h"
#include "EMDOgre.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include "OgreCommon.h"
#include "OgreSkeleton.h"
#include <OgreRectangle2D.h>
#include <OgreSceneNode.h>



MainViewer::MainViewer(QWidget *parent)
	: QWidget(parent),
  _currentESKOgre(NULL),
  _currentEMDOgre(NULL),
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
  
  if (item->parent())
  {
    fileItemDoubleClicked(item->parent(), column); // FIXME create a proper function with the correct name that use FileTreeItemWidget
  }
  
  FileTreeItemWidget *item_cast = dynamic_cast<FileTreeItemWidget *>(item);
  Q_ASSERT(item_cast && "FileTreeItemWidget dynamic_cast failed !");
  
  //Skeleton
  if (item_cast->getType() == FileTreeItemWidget::ItemSkeleton)
  {
    SkeletonItemWidget* skeleton_item = dynamic_cast<SkeletonItemWidget*>(item);
    Q_ASSERT(skeleton_item && "SkeletonItemWidget cast failed !");
    ESKOgre* esk = skeleton_item->getData();
    changeCurrentSkeleton(esk);
  }
  //Mesh
  else if (item_cast->getType() == FileTreeItemWidget::ItemModelPack)
  {
    ModelPackItemWidget* modelpack_item = dynamic_cast<ModelPackItemWidget*>(item);
    Q_ASSERT(modelpack_item && "ModelPackItemWidget cast failed !");
    changeCurrentMesh(modelpack_item->getData());
    regenerateBoneVertexAssigmentTable();
  }
  //Textures
	else if (item_cast->getType() == FileTreeItemWidget::ItemTexture) {
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
  _currentESKOgre = esk;
  skeletonTreeWidget->setESK(esk);
  if (esk)
  {
    enableSkeletonTab();
  }
  else
  {
    disableSkeletonTab();
  }
}

void MainViewer::changeCurrentMesh(EMDOgre* emd)
{
  _currentEMDOgre = emd;
  if (emd)
  {
    enableMeshTab();
    if (emd && !emd->getSceneNodes().empty())
    {
      std::list<Ogre::SceneNode*> sceneNodes = emd->getSceneNodes();
      Ogre::Vector3 pos = (*sceneNodes.begin())->getPosition(); 
      for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
      {
        Q_ASSERT(pos == (*it)->getPosition()); // Oh oh my assumption was bad 
      }
      posXSpinBox->setValue(pos.x);
      posYSpinBox->setValue(pos.y);
      posZSpinBox->setValue(pos.z);
      //TODO do quaternion and scale 
      transformGroupBox->setEnabled(true);
    }
    else 
    {
      transformGroupBox->setEnabled(false);
    }
  }
  else
  {
    disableMeshTab();
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
    enableTab(2);
}

void MainViewer::disableTextureTab()
{
    tabWidget->setTabEnabled(2, false);
}

void MainViewer::enableMeshTab()
{
  enableTab(1);
}

void MainViewer::disableMeshTab()
{
  tabWidget->setTabEnabled(1, false);
}

void MainViewer::exportOgre(QTreeWidgetItem* item, int& export_success, const QString& dir)
{
  FileTreeItemWidget *item_cast = dynamic_cast<FileTreeItemWidget *>(item);

  if (!item_cast)
  {
    return;
  }

  if (item_cast->getType() == FileTreeItemWidget::ItemTexturePack)
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
            std::string filename = dir.toStdString() + "/" + textures[i]->getName();
            image.save(filename);
            export_success++;
          }
          catch (Ogre::Exception& e)
          {
            QMessageBox::critical(0, "Failed to save texture", QString::fromStdString(e.getFullDescription()));
          }
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
      if (emm)
      {
        Ogre::MaterialSerializer serializer;
        std::vector<Ogre::MaterialPtr> mats = emm->getOgreMaterials();
        for (int i = 0; i < mats.size(); i++)
        {
          std::string filename = dir.toStdString() + "/" + mats[i]->getName();
          try
          {
            serializer.exportMaterial(mats[i], filename,true);
            export_success++;
          }
          catch (Ogre::Exception& e)
          {
            QMessageBox::critical(0, "Failed to export material", QString::fromStdString(e.getFullDescription()));
          }
        }
      }
    }
  }
  else if (item_cast->getType() == FileTreeItemWidget::ItemModelPack)
  {
    ModelPackItemWidget* model_pack = dynamic_cast<ModelPackItemWidget*>(item_cast);
    if (model_pack)
    {
      EMDOgre* emd = model_pack->getData();
      if (emd)
      {
        std::list<Ogre::String> mesh_names = emd->getOgreMeshNames();
        std::list<Ogre::String>::const_iterator it;
        Ogre::MeshSerializer serializer;
        
        for (it = mesh_names.begin(); it != mesh_names.end(); it++)
        {
          Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().getByName(*it);
std::string filename = dir.toStdString() + "/" + mesh->getName();
try
{
  Ogre::Mesh::SubMeshIterator sit = mesh->getSubMeshIterator();

  //qDebug() << "Mesh name:" << mesh->getName().c_str();
  //qDebug() << "Mesh filename: " << filename.c_str();
  while (sit.hasMoreElements())
  {
    Ogre::SubMesh* sub_mesh = sit.getNext();
    //qDebug() << "Sub mesh material name:" << sub_mesh->getMaterialName().c_str();
  }

  serializer.exportMesh(mesh.getPointer(), filename);
  export_success++;
}
catch (Ogre::Exception& e)
{
  QMessageBox::critical(0, "Failed to export mesh", QString::fromStdString(e.getFullDescription()));
}
        }
      }
    }
  }
  else if (item_cast->getType() == FileTreeItemWidget::ItemSkeleton)
  {
    SkeletonItemWidget* skeleton_item = dynamic_cast<SkeletonItemWidget*>(item_cast);
    if (skeleton_item)
    {
      ESKOgre* esk = skeleton_item->getData();
      if (esk)
      {
        Ogre::Skeleton* skeleton = esk->getOgreSkeleton();
        if (skeleton)
        {
          Ogre::SkeletonSerializer serializer;
          std::string filename = dir.toStdString() + "/" + skeleton->getName();
          try
          {
            serializer.exportSkeleton(skeleton, filename);
            export_success++;
          }
          catch (Ogre::Exception& e)
          {
            QMessageBox::critical(0, "Failed to export skeleton", QString::fromStdString(e.getFullDescription()));
          }
        }
      }
    }

  }

  for (int i = 0; i < item->childCount(); i++)
  {
    exportOgre(item->child(i), export_success, dir);
  }
}

void MainViewer::exportOgre()
{
  int export_success = 0;
  QString dir = QFileDialog::getExistingDirectory();
  if (dir.isEmpty())
  {
    return;
  }

  for (int i = 0; i < FileTree->topLevelItemCount(); i++)
  {
    exportOgre(FileTree->topLevelItem(i), export_success, dir);
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

void MainViewer::on_mergeSkeletonButton_clicked(bool)
{
  if (_currentESKOgre && _currentESKOgre->getOgreSkeleton())
  {
    SkeletonMergingWidget mergingWidget(this);
    mergingWidget.setTargetSkeleton(_currentESKOgre->getOgreSkeleton());
    mergingWidget.exec();
    skeletonTreeWidget->clear();
    skeletonTreeWidget->regenerateTree();
  }
}

void MainViewer::on_posXSpinBox_valueChanged(double value)
{
  transformMeshChanged(Ogre::Vector3(posXSpinBox->value(), posYSpinBox->value(), posZSpinBox->value()));
}

void MainViewer::on_posYSpinBox_valueChanged(double value)
{
  transformMeshChanged(Ogre::Vector3(posXSpinBox->value(), posYSpinBox->value(), posZSpinBox->value()));
}

void MainViewer::on_posZSpinBox_valueChanged(double value)
{
  transformMeshChanged(Ogre::Vector3(posXSpinBox->value(), posYSpinBox->value(), posZSpinBox->value()));
}


void MainViewer::transformMeshChanged(const Ogre::Vector3& pos)
{
  if (_currentEMDOgre && !_currentEMDOgre->getSceneNodes().empty())
  {
    std::list<Ogre::SceneNode*> meshNodes = _currentEMDOgre->getSceneNodes();
    for (auto it = meshNodes.begin(); it != meshNodes.end(); ++it)
    {
      (*it)->setPosition(pos);
    }
  }
}


void MainViewer::on_applyTransformButton_clicked(bool)
{
  if (_currentEMDOgre && !_currentEMDOgre->getSceneNodes().empty())
  {
    const Ogre::Vector3 pos(posXSpinBox->value(), posYSpinBox->value(), posZSpinBox->value());
    std::list<std::string> meshNames = _currentEMDOgre->getOgreMeshNames();
    for (auto it = meshNames.begin(); it != meshNames.end(); ++it)
    {
      Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().getByName(*it, XENOVIEWER_RESOURCE_GROUP);
      if (mesh->sharedVertexData)
      {
        applyTransformToMesh(mesh->sharedVertexData, pos);
      }
      
      auto subMeshIt = mesh->getSubMeshIterator();
      while (subMeshIt.hasMoreElements())
      {
        if ((*subMeshIt.current())->vertexData)
        {
          applyTransformToMesh((*subMeshIt.current())->vertexData, pos);
        }
        subMeshIt.moveNext();
      }
    }
    _currentEMDOgre->applyDeltaPosition(pos);
    posXSpinBox->setValue(0);
    posYSpinBox->setValue(0);
    posZSpinBox->setValue(0);
    //transformMeshChanged(Ogre::Vector3(0, 0, 0));
  }
}

void MainViewer::applyTransformToMesh(Ogre::VertexData* vertexData, const Ogre::Vector3& pos)
{
  Q_ASSERT(vertexData);
  Q_ASSERT(vertexData->vertexBufferBinding);
  for (size_t i = 0; i < vertexData->vertexBufferBinding->getBufferCount(); ++i)
  {
    Ogre::HardwareVertexBufferSharedPtr vbuf = vertexData->vertexBufferBinding->getBuffer(i);
    unsigned char* pVert = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));
    Ogre::Real* pReal;
    for (size_t v = 0; v < vertexData->vertexCount; ++v)
    {
      // Get elements
      Ogre::VertexDeclaration::VertexElementList elems = vertexData->vertexDeclaration->findElementsBySource(i);
      Ogre::VertexDeclaration::VertexElementList::iterator i, iend;
      for (i = elems.begin(); i != elems.end(); ++i)
      {
        Ogre::VertexElement& elem = *i;
        if (elem.getSemantic() == Ogre::VES_POSITION)
        {
          elem.baseVertexPointerToElement(pVert, &pReal);
          pReal[0] += pos.x;
          pReal[1] += pos.y;
          pReal[2] += pos.z;
        }
      }
      pVert += vbuf->getVertexSize();
    }
    vbuf->unlock();
  }
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

void MainViewer::regenerateBoneVertexAssigmentTable()
{
  Q_ASSERT(_currentEMDOgre);
  std::list<Ogre::String> meshNames = _currentEMDOgre->getOgreMeshNames();
  std::list<Ogre::String>::iterator it;
  
  std::map<unsigned short, size_t> verticesAssigned;
  for (it = meshNames.begin(); it != meshNames.end(); ++it)
  {
    Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().getByName(*it);
    const Ogre::Mesh::VertexBoneAssignmentList& assigments = mesh->getBoneAssignments();
    Ogre::Mesh::VertexBoneAssignmentList::const_iterator bit;
    for (bit = assigments.cbegin(); bit != assigments.cend(); ++bit)
    {
      if (verticesAssigned.find(bit->second.boneIndex) == verticesAssigned.end())
      {
        verticesAssigned[bit->second.boneIndex] = 1;
      }
      else
      {
        verticesAssigned[bit->second.boneIndex] += 1;
      }
    }

    auto submesh_it = mesh->getSubMeshIterator();
    while (submesh_it.hasMoreElements())
    {
      Ogre::SubMesh* submesh = *submesh_it.current();
      const Ogre::SubMesh::VertexBoneAssignmentList& assigments = submesh->getBoneAssignments();
      Ogre::SubMesh::VertexBoneAssignmentList::const_iterator bit;
      for (bit = assigments.cbegin(); bit != assigments.cend(); ++bit)
      {
        if (verticesAssigned.find(bit->second.boneIndex) == verticesAssigned.end())
        {
          verticesAssigned[bit->second.boneIndex] = 1;
        }
        else
        {
          verticesAssigned[bit->second.boneIndex] += 1;
        }
      }

      submesh_it.moveNext();
    }

    if (_currentESKOgre)
    {
      Q_ASSERT(mesh->getSkeleton().get() == _currentESKOgre->getOgreSkeleton());
    }
    else
    {
      Q_ASSERT(!mesh->getSkeleton().get());
    }
  }


  vertexBoneAssignmentMeshTable->clearContents();
  vertexBoneAssignmentMeshTable->setRowCount(verticesAssigned.size());

  Ogre::Skeleton* skeleton = 0;
  if (_currentESKOgre)
  {
    skeleton = _currentESKOgre->getOgreSkeleton();
  }

  int current_row = 0;
  std::map<unsigned short, size_t>::iterator vit;
  for (vit = verticesAssigned.begin(); vit != verticesAssigned.end(); ++vit)
  {
    QTableWidgetItem* boneIndex = new QTableWidgetItem(QString::number(vit->first));
    QTableWidgetItem* verticesCount = new QTableWidgetItem(QString::number(vit->second));
    vertexBoneAssignmentMeshTable->setItem(current_row, 1, boneIndex);
    vertexBoneAssignmentMeshTable->setItem(current_row, 2, verticesCount);

    if (skeleton)
    {
      Ogre::Bone* bone = skeleton->getBone(vit->first);
      QTableWidgetItem* boneName = new QTableWidgetItem(QString::fromStdString(bone->getName()));
      vertexBoneAssignmentMeshTable->setItem(current_row, 0, boneName);
    }
    ++current_row;
  }

}
