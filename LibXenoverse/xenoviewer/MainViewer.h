#include "ui_MainViewer.h"

class QGraphicsScene;
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

	
  void changeCurrentTexture(Ogre::Texture* texture);
  void changeCurrentSkeleton(ESKOgre* esk);

  inline Ogre::Texture* getCurrentTexture()
  {
    return _current_ogre_texture;
  }

	private slots:
		void fileItemDoubleClicked(QTreeWidgetItem * item, int column);
		void animationItemDoubleClicked(QTreeWidgetItem * item, int column);
		void contextMenuFileTree(const QPoint& point);
		void clearFileTree();
    void clearAnimTree();
		void disableTabs();
		void enableTab(int index);

    void enableTextureTab();
    void enableSkeletonTab();
    void disableTextureTab();
    void disableSkeletonTab();

    void saveTextureToFile();
    void loadTextureFromFile();
    void exportOgre();

  
private:
  static void exportOgre(QTreeWidgetItem* item, int& export_success);


  Ogre::Texture *_current_ogre_texture;
  QGraphicsScene* _textureGraphicsScene;

};
