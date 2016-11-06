#include "FileTreeWidget.h"
#include "FileTreeItemWidget.h"

#include "EMDOgre.h"
#include "ESKOgre.h"

FileTreeWidget::FileTreeWidget(QWidget *parent) : QTreeWidget(parent) {
	setFocusPolicy(Qt::StrongFocus);
	setContextMenuPolicy(Qt::CustomContextMenu);
}

void FileTreeWidget::dropEvent(QDropEvent *event) {
	//MimeData: application/x-qabstractitemmodeldatalist"
	const QMimeData* qMimeData = event->mimeData();

	QByteArray encoded = qMimeData->data("application/x-qabstractitemmodeldatalist");
	QDataStream stream(&encoded, QIODevice::ReadOnly);

	while (!stream.atEnd())
	{
		int row, col;
		QMap<int, QVariant> roleDataMap;
		stream >> row >> col >> roleDataMap;
		QString dropped = roleDataMap[0].toString();

		// FIXME: There has to be a better way to get the dropped widget than just a name match
		QList<QTreeWidgetItem *> match_items = findItems(dropped, Qt::MatchExactly | Qt::MatchRecursive);
		if (match_items.count()) {
			QTreeWidgetItem *first_match = match_items[0];
			FileTreeItemWidget *source_item = static_cast<FileTreeItemWidget *>(first_match);
			FileTreeItemWidget *dest_item = static_cast<FileTreeItemWidget *>(this->itemAt(event->pos()));
			dropItemOnItem(source_item, dest_item);
		}
	}

	QTreeWidget::dropEvent(event);
}


void FileTreeWidget::dropItemOnItem(FileTreeItemWidget *source_item, FileTreeItemWidget *dest_item) {
	ModelPackItemWidget *src_model_pack_item = NULL;
	SkeletonItemWidget *src_skeleton_item = NULL;
	ModelPackItemWidget *dest_model_pack_item = NULL;
	SkeletonItemWidget *dest_skeleton_item = NULL;

	if (source_item) {
		src_model_pack_item = static_cast<ModelPackItemWidget *>(source_item->getMatchType(FileTreeItemWidget::ItemModelPack));
		src_skeleton_item = static_cast<SkeletonItemWidget *>(source_item->getMatchType(FileTreeItemWidget::ItemSkeleton));
	}

	if (dest_item) {
		dest_model_pack_item = static_cast<ModelPackItemWidget *>(dest_item->getMatchType(FileTreeItemWidget::ItemModelPack));
		dest_skeleton_item = static_cast<SkeletonItemWidget *>(dest_item->getMatchType(FileTreeItemWidget::ItemSkeleton));
	}

	// Dragging a model pack into a skeleton
	if (src_model_pack_item && dest_skeleton_item) {
		EMDOgre *model_pack = src_model_pack_item->getData();
		ESKOgre *skeleton = dest_skeleton_item->getData();
		model_pack->setSkeleton(skeleton);
		model_pack->tagForRebuild();
		src_model_pack_item->updateText();
		dest_skeleton_item->setExpanded(true);
	}

	// Dragging a model pack into something that isn't a skeleton
	if (src_model_pack_item && !dest_skeleton_item) {
		// Dragging into nothing
		if (!dest_item) {
			EMDOgre *model_pack = src_model_pack_item->getData();

			if (model_pack->getSkeleton()) {
				model_pack->setSkeleton(NULL);
				model_pack->tagForRebuild();
				src_model_pack_item->updateText();
			}
		}
		// Dragging into a model, verify that its parent is not the same skeleton
		else if (dest_model_pack_item) {
			EMDOgre *model_pack = src_model_pack_item->getData();
			EMDOgre *dest_model_pack = dest_model_pack_item->getData();

			if (model_pack->getSkeleton() != dest_model_pack->getSkeleton()) {
				model_pack->setSkeleton(dest_model_pack->getSkeleton());
				model_pack->tagForRebuild();
				src_model_pack_item->updateText();
			}
		}
		else {
			SHOW_ERROR("FIXME: Unhandled Drag and Drop for Model Pack");
		}
	}
}

void FileTreeWidget::keyPressEvent(QKeyEvent * event) {
	QTreeWidget::keyPressEvent(event);

	/*if (event->key() == Qt::Key_Delete) {
		SHOW_ERROR("Key Detected!");
	}*/
}

void FileTreeWidget::processContextMenuModelPack(ModelPackItemWidget *item, const QPoint& point) {
	QMenu myMenu;
	myMenu.addAction("Delete");

	QAction* selectedItem = myMenu.exec(point);
	if (selectedItem) {
		delete item;
	}
	else {
	}
}

void FileTreeWidget::processContextMenuSkeleton(SkeletonItemWidget *item, const QPoint& point) {
	QMenu myMenu;
	myMenu.addAction("Delete");

	QAction* selectedItem = myMenu.exec(point);
	if (selectedItem) {
		delete item;
	}
	else {
	}
}

void FileTreeWidget::processContextMenuEmpty(const QPoint& point) {
	
}

void FileTreeWidget::processContextMenu(const QPoint& point) {
	FileTreeItemWidget *item = static_cast<FileTreeItemWidget *>(itemAt(point));
	QPoint globalPos = mapToGlobal(point);

	if (item) {
		ModelPackItemWidget *model_pack_item = static_cast<ModelPackItemWidget *>(item->getMatchType(FileTreeItemWidget::ItemModelPack));
		SkeletonItemWidget *skeleton_item = static_cast<SkeletonItemWidget *>(item->getMatchType(FileTreeItemWidget::ItemSkeleton));

		if (model_pack_item) processContextMenuModelPack(model_pack_item, globalPos);
		else if (skeleton_item) processContextMenuSkeleton(skeleton_item, globalPos);
	}
	else processContextMenuEmpty(globalPos);
}