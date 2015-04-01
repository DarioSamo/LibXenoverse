#include "LibXenoverse.h"

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreWindowEventUtilities.h>
#include <OgreTextureManager.h>
#include <OgreMaterialManager.h>
#include <OgreMeshManager.h>
#include <OgreSkeletonManager.h>
#include <OgreMesh.h>
#include <OgreSubmesh.h>
#include <OgreSkeleton.h>
#include <OgreSkeletonInstance.h>
#include <OgreBone.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreMath.h>
#include <OgreMatrix4.h>
#include <OgreKeyFrame.h>
#include <OgreGpuProgramManager.h>
#include <OgreRenderObjectListener.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreManualObject.h>
#include <OgreCommon.h>
#include <OgrePrerequisites.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QtGui>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QList>
#include <QMainWindow>
#include <QSettings>
#include <QWidget>
#include <QTimer>
#include <QTreeWidget>
#include <QtWin>
#include <QColorDialog>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#  include <OIS/OISEvents.h>
#  include <OIS/OISInputManager.h>
#  include <OIS/OISKeyboard.h>
#  include <OIS/OISMouse.h>

#  include <OGRE/SdkTrays.h>
#  include <OGRE/SdkCameraMan.h>
#else
#  include <OISEvents.h>
#  include <OISInputManager.h>
#  include <OISKeyboard.h>
#  include <OISMouse.h>

#  include <SdkTrays.h>
#  include <SdkCameraMan.h>
#endif

#ifdef OGRE_STATIC_LIB
#  define OGRE_STATIC_GL
#  if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#    define OGRE_STATIC_Direct3D9
// D3D10 will only work on vista, so be careful about statically linking
#    if OGRE_USE_D3D10
#      define OGRE_STATIC_Direct3D10
#    endif
#  endif
#  define OGRE_STATIC_BSPSceneManager
#  define OGRE_STATIC_ParticleFX
#  define OGRE_STATIC_CgProgramManager
#  ifdef OGRE_USE_PCZ
#    define OGRE_STATIC_PCZSceneManager
#    define OGRE_STATIC_OctreeZone
#  else
#    define OGRE_STATIC_OctreeSceneManager
#  endif
#  include "OgreStaticPluginLoader.h"
#endif

using namespace LibXenoverse;

#define XENOVIEWER_RESOURCE_GROUP  "General"
#define SHOW_MSG(x)                MessageBox(NULL, x, "Info", MB_OK)
#define SHOW_SMSG(x)               MessageBox(NULL, (x).c_str(), "Info", MB_OK)
#define SHOW_ERROR(x)			   QErrorMessage(x)

void QErrorMessage(QString error_str);