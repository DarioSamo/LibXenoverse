#include <QDoubleSpinBox>
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

	
  void changeCurrentTexture(Ogre::Texture* texture);
  void changeCurrentSkeleton(ESKOgre* esk);
  void changeCurrentMesh(EMDOgre* emd);

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
    void enableMeshTab();
    void disableTextureTab();
    void disableSkeletonTab();
    void disableMeshTab();

    void saveTextureToFile();
    void loadTextureFromFile();
    void exportOgre();

    void on_mergeSkeletonButton_clicked(bool);
    void on_posXSpinBox_valueChanged(double value);
    void on_posYSpinBox_valueChanged(double value);
    void on_posZSpinBox_valueChanged(double value);

    void on_applyTransformButton_clicked(bool);

    void regenerateBoneVertexAssigmentTable();

  
private:
  // TODO: move theses elsewhere
  void transformMeshChanged(const Ogre::Vector3& pos); 
  void applyTransformToMesh(Ogre::VertexData* vertexData, const Ogre::Vector3& pos);

  static void exportOgre(QTreeWidgetItem* item, int& export_success, const QString& dir);


  Ogre::Texture *_current_ogre_texture;
  ESKOgre* _currentESKOgre;
  EMDOgre* _currentEMDOgre;
  QGraphicsScene* _textureGraphicsScene;

};
