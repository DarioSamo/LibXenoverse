#ifndef QTOGRE_OGREWIDGET_H_
#define QTOGRE_OGREWIDGET_H_

#include <OgreCommon.h>
#include <OgrePrerequisites.h>

#include <QSettings>
#include <QWidget>
#include <QTimer>

#include "SkeletonDebug.h"
#include "EANOgre.h"
#include "EMDRenderObjectListener.h"

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
	protected:
		void paintEvent(QPaintEvent* evt);
		void resizeEvent(QResizeEvent* evt);

		void mousePressEvent(QMouseEvent * event);
		void mouseMoveEvent(QMouseEvent * event);
		void mouseReleaseEvent(QMouseEvent * event);

		void keyPressEvent(QKeyEvent * event);
		void wheelEvent(QWheelEvent * event);
	private:
		void initialiseOgre();
		void chooseSceneManager();
		void moveAndResize();
		void createCamera();
		void createViewports();
		void loadResources();
		virtual void createScene();
		virtual void destroyScene();
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		void switchCurrentAnimation();
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
	};
}

#endif