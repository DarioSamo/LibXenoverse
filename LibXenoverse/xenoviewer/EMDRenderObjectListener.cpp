#include "EMDRenderObjectListener.h"

void EMDRenderObjectListener::notifyRenderSingleObject(Ogre::Renderable *rend, const Ogre::Pass *pass, const Ogre::AutoParamDataSource *source, const Ogre::LightList *pLightList, bool suppressRenderStateChanges) {
	Ogre::Any any_ptr = rend->getUserObjectBindings().getUserAny();

	if (!any_ptr.isEmpty()) {
		EMDRenderObject *parameter = Ogre::any_cast<EMDRenderObject *>(any_ptr);
		if (parameter) {
			Ogre::Pass *edit_pass = const_cast<Ogre::Pass *>(pass);
			if (edit_pass) {
				Ogre::TextureUnitState *s1 = edit_pass->getTextureUnitState(1);
				s1->setTexture(parameter->s1_ptr);

				/*
				Ogre::TextureUnitState *s4 = edit_pass->getTextureUnitState(4);
				s4->setTexture(parameter->s4_ptr);
				*/

				Ogre::Root::getSingleton().getRenderSystem()->_setTextureUnitSettings(1, *s1);
				//Ogre::Root::getSingleton().getRenderSystem()->_setTextureUnitSettings(4, *s4);
			}
		}
	}
}


EMDRenderObject::EMDRenderObject(Ogre::TexturePtr s1, Ogre::TexturePtr s4) {
	s1_ptr = s1;
	s4_ptr = s4;
}


void EMDRenderObjectAssignVisitor::visit(Ogre::Renderable *rend, Ogre::ushort lodIndex, bool isDebug, Ogre::Any *pAny) {
	rend->getUserObjectBindings().setUserAny(Ogre::Any(parameter));
}