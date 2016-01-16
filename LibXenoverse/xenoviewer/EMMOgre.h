#ifndef EMMOGRE_H_INCLUDED
#define EMMOGRE_H_INCLUDED

#include <vector>
#include <map>

class EMBOgre;

class EMMOgre : public EMM {
protected:
	bool material_resources_created;
	EMBOgre *texture_pack;
	EMBOgre *texture_dyt_pack;

	std::map<Ogre::String,Ogre::MaterialPtr> _materials;
public:
	EMMOgre();
	~EMMOgre();

	Ogre::MaterialPtr createOgreMaterial(EMMMaterial *emm_material);
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
		return texture_dyt_pack;
	}

  std::vector<Ogre::MaterialPtr> getOgreMaterials() const
  {
    std::vector<Ogre::MaterialPtr> mats;
    mats.reserve(_materials.size());
    std::map<Ogre::String, Ogre::MaterialPtr>::const_iterator it;
    for (it = _materials.begin(); it != _materials.end(); it++)
    {
      mats.push_back(it->second);
    }
    return mats;
  }

	void destroyResources();
};

#endif