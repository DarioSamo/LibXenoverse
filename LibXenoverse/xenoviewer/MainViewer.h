#include "ui_MainViewer.h"

class EMDOgre;
class ESKOgre;
class EANOgre;

class MainViewer : public QWidget, private Ui::MainViewer
{
	Q_OBJECT
public:
	MainViewer(QWidget *parent = 0);

	void createFileTreeItems(list<EMDOgre *> &new_emd_list, list<ESKOgre *> &new_esk_list, list<EANOgre *> &new_ean_list);
	void keyPressEvent(QKeyEvent * event);
	
	private slots:
		void animationItemDoubleClicked(QTreeWidgetItem * item, int column);
		void contextMenuFileTree(const QPoint& point);
		void clearFileTree();
};
