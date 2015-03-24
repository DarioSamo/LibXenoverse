#ifndef EMMOGRE_H_INCLUDED
#define EMMOGRE_H_INCLUDED

class EMBOgre;

class EMMOgre : public EMM {
protected:
	bool material_resources_created;
	EMBOgre *texture_pack;
	EMBOgre *texture_dyt_pack;

	list<Ogre::String> created_materials;
public:
	EMMOgre();
	~EMMOgre();

	Ogre::Material *createOgreMaterial(EMMMaterial *emm_material);
	void createOgreMaterials();

	void setTexturePack(EMBOgre *v) {
		texture_pack = v;
	}

	void setDYTTexturePack(EMBOgre *v) {
		texture_dyt_pack = v;
	}

	EMBOgre *getTexturePack() {
		return texture_pack;
	}

	EMBOgre *getDYTTexturePack() {
		return texture_pack;
	}

	void destroyResources();
};

#endif