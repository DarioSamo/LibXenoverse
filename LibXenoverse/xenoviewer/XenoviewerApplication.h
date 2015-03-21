#ifndef XENOVIEWER_APPLICATION_H_INCLUDED
#define XENOVIEWER_APPLICATION_H_INCLUDED

#include "BaseApplication.h"
#include "EMDRenderObjectListener.h"

class XenoviewerApplication : public BaseApplication
{
public:
    XenoviewerApplication(void);
    virtual ~XenoviewerApplication(void);

protected:
	EMDRenderObjectListener *emd_render_object_listener;

    virtual void createScene(void);
};


#endif
