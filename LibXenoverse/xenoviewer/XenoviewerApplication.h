#ifndef XENOVIEWER_APPLICATION_H_INCLUDED
#define XENOVIEWER_APPLICATION_H_INCLUDED

#include "BaseApplication.h"
#include "EMDRenderObjectListener.h"
#include "SkeletonDebug.h"

class XenoviewerApplication : public BaseApplication
{
public:
    XenoviewerApplication(void);
    virtual ~XenoviewerApplication(void);

	Ogre::AnimationState *current_animation_state;
	SkeletonDebug *skeleton_debug;
protected:
	EMDRenderObjectListener *emd_render_object_listener;

    virtual void createScene(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};


#endif
