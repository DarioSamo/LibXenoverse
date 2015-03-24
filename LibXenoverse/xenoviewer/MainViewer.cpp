#include "MainViewer.h"
#include "OgreWidget.h"
#include "FileTreeItemWidget.h"
#include "EANOgre.h"

MainViewer::MainViewer(QWidget *parent)
	: QWidget(parent) {
	setupUi(this);

	FileTree->acceptDrops();
	FileTree->setDragEnabled(true);

	connect(FileTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuFileTree(const QPoint&)));
	connect(AnimationTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(animationItemDoubleClicked(QTreeWidgetItem *, int)));
	connect(ClearFileTreeButton, SIGNAL(released()), this, SLOT(clearFileTree()));
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