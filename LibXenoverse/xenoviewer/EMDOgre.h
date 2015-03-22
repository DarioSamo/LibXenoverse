#ifndef EMDOGRE_H_INCLUDED
#define EMDOGRE_H_INCLUDED

class EMBOgre;
class ESKOgre;
class EMMOgre;

class EMDOgre : public EMD {
	protected:
		bool mesh_resources_created;
		EMMOgre *material_pack;
		ESKOgre *skeleton;
	public:
		EMDOgre();

		Ogre::SubMesh *createOgreSubmesh(EMDTriangles *triangles, Ogre::MeshPtr mesh);
		void createOgreMesh(EMDSubmesh *submesh, string mesh_name);
		void createOgreEntityMeshes(EMDMesh *mesh);
		Ogre::SceneNode *createOgreSceneNodeModel(EMDModel *model, Ogre::SceneNode *parent, Ogre::SceneManager *scene_manager);
		Ogre::SceneNode *createOgreSceneNode(Ogre::SceneManager *scene_manager);

		void setMaterialPack(EMMOgre *v) {
			material_pack = v;
		}

		void setSkeleton(ESKOgre *v) {
			skeleton = v;
		}
};

#endif