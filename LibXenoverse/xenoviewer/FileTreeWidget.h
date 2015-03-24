#ifndef FILE_TREE_WIDGET_H_INCLUDED
#define FILE_TREE_WIDGET_H_INCLUDED

class FileTreeItemWidget;
class ModelPackItemWidget;
class SkeletonItemWidget;

class FileTreeWidget : public QTreeWidget {
	protected:
	public:
		FileTreeWidget(QWidget *parent);

		void dropEvent(QDropEvent *event);
		void dropItemOnItem(FileTreeItemWidget *source_item, FileTreeItemWidget *dest_item);

		void keyPressEvent(QKeyEvent * event);
		void processContextMenuModelPack(ModelPackItemWidget *item, const QPoint& point);
		void processContextMenuSkeleton(SkeletonItemWidget *item, const QPoint& point);
		void processContextMenuEmpty(const QPoint& point);
		void processContextMenu(const QPoint& point);
};

#endif