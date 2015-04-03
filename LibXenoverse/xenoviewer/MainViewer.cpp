#include "MainViewer.h"
#include "OgreWidget.h"
#include "FileTreeItemWidget.h"
#include "EANOgre.h"
#include "EMBOgre.h"

MainViewer::MainViewer(QWidget *parent)
	: QWidget(parent) {
	setupUi(this);

	FileTree->acceptDrops();
	FileTree->setDragEnabled(true);

	connect(FileTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuFileTree(const QPoint&)));
	connect(FileTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(fileItemDoubleClicked(QTreeWidgetItem *, int)));
	connect(AnimationTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(animationItemDoubleClicked(QTreeWidgetItem *, int)));
	connect(ClearFileTreeButton, SIGNAL(released()), this, SLOT(clearFileTree()));
	connect(GenerateClusterButton, SIGNAL(released()), this, SLOT(paintCluster()));

	current_ogre_texture = NULL;

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
				current_ogre_texture = emb->getOgreTexture(emb_file->getIndex());
				enableTab(6);
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

void MainViewer::paintCluster() {
	if (!current_ogre_texture) return;

	Ogre::Image image;
	size_t width = 128;
	size_t height = 128;
	size_t image_byte_size = width * height * 3;
	uchar *image_data = new uchar[image_byte_size];
	memset(image_data, 0x00, image_byte_size);

	Cluster1_1->paintOgreTexture(image_data, width, 0);
	Cluster1_2->paintOgreTexture(image_data, width, 4);
	Cluster1_3->paintOgreTexture(image_data, width, 8);
	Cluster1_4->paintOgreTexture(image_data, width, 12);
	Cluster2_1->paintOgreTexture(image_data, width, 16);
	Cluster2_2->paintOgreTexture(image_data, width, 20);
	Cluster2_3->paintOgreTexture(image_data, width, 24);
	Cluster2_4->paintOgreTexture(image_data, width, 28);
	Cluster3_1->paintOgreTexture(image_data, width, 32);
	Cluster3_2->paintOgreTexture(image_data, width, 36);
	Cluster3_3->paintOgreTexture(image_data, width, 40);
	Cluster3_4->paintOgreTexture(image_data, width, 44);
	Cluster4_1->paintOgreTexture(image_data, width, 48);
	Cluster4_2->paintOgreTexture(image_data, width, 52);
	Cluster4_3->paintOgreTexture(image_data, width, 56);
	Cluster4_4->paintOgreTexture(image_data, width, 60);
	Cluster5_1->paintOgreTexture(image_data, width, 64);
	Cluster5_2->paintOgreTexture(image_data, width, 68);
	Cluster5_3->paintOgreTexture(image_data, width, 72);
	Cluster5_4->paintOgreTexture(image_data, width, 76);
	Cluster6_1->paintOgreTexture(image_data, width, 80);
	Cluster6_2->paintOgreTexture(image_data, width, 84);
	Cluster6_3->paintOgreTexture(image_data, width, 88);
	Cluster6_4->paintOgreTexture(image_data, width, 92);
	Cluster7_1->paintOgreTexture(image_data, width, 96);
	Cluster7_2->paintOgreTexture(image_data, width, 100);
	Cluster7_3->paintOgreTexture(image_data, width, 104);
	Cluster7_4->paintOgreTexture(image_data, width, 108);
	Cluster8_1->paintOgreTexture(image_data, width, 112);
	Cluster8_2->paintOgreTexture(image_data, width, 116);
	Cluster8_3->paintOgreTexture(image_data, width, 120);
	Cluster8_4->paintOgreTexture(image_data, width, 124);

	image.loadDynamicImage(image_data, width, height, 1, Ogre::PF_BYTE_BGR);
	current_ogre_texture->unload();
	current_ogre_texture->loadImage(image);

	delete [] image_data;
}