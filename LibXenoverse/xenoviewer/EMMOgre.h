#ifndef EMMOGRE_H_INCLUDED
#define EMMOGRE_H_INCLUDED

class EMMOgre : public EMM {
protected:
	bool material_resources_created;
public:
	EMMOgre();

	Ogre::Material *createOgreMaterial(EMMMaterial *emm_material);
	void createOgreMaterials();
};

#endif