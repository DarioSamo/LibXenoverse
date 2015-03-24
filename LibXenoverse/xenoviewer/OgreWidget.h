#ifndef QTOGRE_OGREWIDGET_H_
#define QTOGRE_OGREWIDGET_H_

class EMDOgre;
class ESKOgre;
class EANOgre;
class SkeletonDebug;
class EMDRenderObjectListener;

namespace QtOgre
{
	class InputEventHandler;

	class OgreWidget : public QWidget, public Ogre::FrameListener
	{
		Q_OBJECT

	public:
		OgreWidget(QWidget* pParentWidget=0, Qt::WindowFlags f=0);
		~OgreWidget();

		Ogre::RenderWindow* getOgreRenderWindow() const;
	public:
		Ogre::RenderWindow* renderWindow()
		{
			return mWindow;
		}
		Ogre::RenderWindow* mWindow;

		void resetCamera();

		void addFile(string filename, list<EMDOgre *> &target_emd_list, list<ESKOgre *> &target_esk_list, list<EANOgre *> &target_ean_list);
		void addFileEAN(string filename, list<EANOgre *> &target_ean_list);
		void addFileESK(string filename, list<ESKOgre *> &target_esk_list);
		void addFileEMD(string filename, list<EMDOgre *> &target_emd_list);
		void addFiles(const QStringList& pathList, list<EMDOgre *> &target_emd_list, list<ESKOgre *> &target_esk_list, list<EANOgre *> &target_ean_list);

		void getItemLists(list<EMDOgre *> &target_emd_list, list<ESKOgre *> &target_esk_list, list<EANOgre *> &target_ean_list);

		void checkRebuild();
		bool installShaders();
	protected:
		void paintEvent(QPaintEvent* evt);
		void resizeEvent(QResizeEvent* evt);

		void mousePressEvent(QMouseEvent * event);
		void mouseMoveEvent(QMouseEvent * event);
		void mouseReleaseEvent(QMouseEvent * event);
		void wheelEvent(QWheelEvent * event);
	private:
		void initialiseOgre();
		void chooseSceneManager();
		void moveAndResize();
		void createCamera();
		void createViewports();
		void loadResources();
		virtual void createScene();
		void loadDebugModels();
		virtual void destroyScene();
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		void repositionCamera();
		QPaintEngine *paintEngine() const;
		void spinCamera(float delta_x, float delta_y);
		void panCamera(float delta_x, float delta_y);
		void zoomCamera(float delta);

		QWidget* mpParentWidget;
		Ogre::RenderSystem* mActiveRenderSystem;
		Ogre::Root* mRoot;
		Ogre::Camera* mCamera;
		Ogre::SceneManager* mSceneMgr;
		QTimer* mUpdateTimer;

		bool enable_spinning;
		bool enable_panning;
		float spin_x;
		float spin_y;
		int last_mouse_x;
		int last_mouse_y;
		float zoom;

		Ogre::Vector3 viewer_center;
		float viewer_angle_x;
		float viewer_angle_y;

		int current_animation_index;
		SkeletonDebug *skeleton_debug;
		Ogre::AnimationState *current_animation_state;
		EANOgre *animation;
		Ogre::Entity *entity;
		EMDRenderObjectListener *emd_render_object_listener;

		list<EMDOgre *> emd_list;
		list<ESKOgre *> esk_list;
		list<EANOgre *> ean_list;
	};
}

#endif