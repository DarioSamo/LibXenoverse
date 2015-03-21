#ifndef EMD_RENDER_OBJECT_LISTENER_H_INCLUDED
#define EMD_RENDER_OBJECT_LISTENER_H_INCLUDED

class EMDRenderObjectListener : public Ogre::RenderObjectListener {
protected:
public:
	EMDRenderObjectListener() {
	}

	void notifyRenderSingleObject(Ogre::Renderable *rend, const Ogre::Pass *pass, const Ogre::AutoParamDataSource *source, const Ogre::LightList *pLightList, bool suppressRenderStateChanges);
};


class EMDRenderObject {
public:
	Ogre::TexturePtr s1_ptr;
	Ogre::TexturePtr s4_ptr;

	EMDRenderObject(Ogre::TexturePtr s1, Ogre::TexturePtr s4);
};

class EMDRenderObjectAssignVisitor : public Ogre::Renderable::Visitor {
protected:
	EMDRenderObject *parameter;
public:
	EMDRenderObjectAssignVisitor(EMDRenderObject *p) {
		parameter = p;
	}

	void visit(Ogre::Renderable *rend, Ogre::ushort lodIndex, bool isDebug, Ogre::Any *pAny = 0);
};

#endif