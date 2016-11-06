#ifndef BONES_TREE_WIDGET_H_INCLUDED
#define BONES_TREE_WIDGET_H_INCLUDED

#include <QTreeWidget>

class ESKOgre;

class BonesTreeWidget : public QTreeWidget 
{

  Q_OBJECT

	public:
		BonesTreeWidget(QWidget *parent = 0);

  public slots:
    void setESK(ESKOgre* esk);
    void regenerateTree();
    void removeSelectedBones();

 
  protected slots:
    void on_itemChanged(QTreeWidgetItem* item, int col);

  protected:

    virtual void keyPressEvent(QKeyEvent* event);

    virtual void dropEvent(QDropEvent *event);
    void moveItems(QTreeWidgetItem* newParent, const QList<QTreeWidgetItem*>& items);

    virtual QStringList mimeTypes() const;
    virtual QMimeData* mimeData(const QList<QTreeWidgetItem *> items) const;

    ESKOgre* _esk;
    
};

#endif