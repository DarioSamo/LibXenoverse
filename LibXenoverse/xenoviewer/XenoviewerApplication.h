#ifndef XENOVIEWER_APPLICATION_H_INCLUDED
#define XENOVIEWER_APPLICATION_H_INCLUDED

#include "BaseApplication.h"

class XenoviewerApplication : public BaseApplication
{
public:
    XenoviewerApplication(void);
    virtual ~XenoviewerApplication(void);

protected:
    virtual void createScene(void);
};


#endif
