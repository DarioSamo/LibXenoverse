#ifndef __MovableTextOverlay_H__
#define __MovableTextOverlay_H__

#include <Ogre.h>
#include <OgreFont.h>
#include <OgreFontManager.h>

class ObjectTextDisplay {

public:
	ObjectTextDisplay(Ogre::String name, Ogre::Bone* p, Ogre::Camera* c, Ogre::Entity *ent) {
		m_p = p;
		m_ent = ent;
		m_c = c;
		m_enabled = false;
		m_text = "";

		// create an overlay that we can use for later
		m_pOverlay = Ogre::OverlayManager::getSingleton().create(name);
		m_pContainer = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement(
			"Panel", name + "container"));

		m_pOverlay->add2D(m_pContainer);

		m_pText = Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", name + "shapeNameText");
		m_pText->setDimensions(1.0, 1.0);
		m_pText->setMetricsMode(Ogre::GMM_PIXELS);
		m_pText->setPosition(0, 0);

		m_pText->setParameter("font_name", "SdkTrays/Value");
		m_pText->setParameter("char_height", "16");
		m_pText->setParameter("horz_align", "center");
		m_pText->setColour(Ogre::ColourValue(1.0, 1.0, 1.0));

		m_pContainer->addChild(m_pText);
		m_pOverlay->show();
	}

	virtual ~ObjectTextDisplay() {

		// overlay cleanup -- Ogre would clean this up at app exit but if your app
		// tends to create and delete these objects often it's a good idea to do it here.

		m_pOverlay->hide();
		Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
		m_pContainer->removeChild("shapeNameText");
		m_pOverlay->remove2D(m_pContainer);
		overlayManager->destroyOverlayElement(m_pText);
		overlayManager->destroyOverlayElement(m_pContainer);
		overlayManager->destroy(m_pOverlay);
	}

	void enable(bool enable) {
		m_enabled = enable;
		if (enable)
			m_pOverlay->show();
		else
			m_pOverlay->hide();
	}

	void setText(const Ogre::String& text) {
		m_text = text;
		m_pText->setCaption(m_text);
	}

	void update();

	//protected:
	Ogre::Bone* m_p;
	Ogre::Camera* m_c;
	Ogre::Entity *m_ent;
	bool m_enabled;
	Ogre::Overlay* m_pOverlay;
	Ogre::OverlayElement* m_pText;
	Ogre::OverlayContainer* m_pContainer;
	Ogre::String m_text;
};


#endif /* __MovableTextOverlay_H__ */