#ifndef XENOVIEWER_APPLICATION_H_INCLUDED
#define XENOVIEWER_APPLICATION_H_INCLUDED

#include "BaseApplication.h"
#include "EMDRenderObjectListener.h"
#include "SkeletonDebug.h"
#include "EANOgre.h"

class XenoviewerApplication : public BaseApplication
{
public:
    XenoviewerApplication(void);
    virtual ~XenoviewerApplication(void);

	Ogre::AnimationState *current_animation_state;
	SkeletonDebug *skeleton_debug;
	int current_animation_index;
	EANOgre *animation;
	Ogre::Entity *entity;
protected:
	EMDRenderObjectListener *emd_render_object_listener;

    virtual void createScene(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	virtual bool keyPressed(const OIS::KeyEvent &arg);

	void switchCurrentAnimation();
};


#endif
