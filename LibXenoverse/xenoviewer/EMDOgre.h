#include "ESKOgre.h"
#include "EMBOgre.h"

#ifndef EMDOGRE_H_INCLUDED
#define EMDOGRE_H_INCLUDED

class EMDOgre : public EMD {
	protected:
		bool mesh_resources_created;
		EMBOgre *texture_pack;
		EMBOgre *texture_dyt_pack;
		ESKOgre *skeleton;
	public:
		EMDOgre();

		Ogre::SubMesh *createOgreSubmesh(EMDTriangles *triangles, Ogre::MeshPtr mesh);
		void createOgreMesh(EMDSubmesh *submesh, string mesh_name);
		void createOgreEntityMeshes(EMDMesh *mesh);
		Ogre::SceneNode *createOgreSceneNodeModel(EMDModel *model, Ogre::SceneNode *parent, Ogre::SceneManager *scene_manager);
		Ogre::SceneNode *createOgreSceneNode(Ogre::SceneManager *scene_manager, EMBOgre *texture_pack_p = NULL, EMBOgre *texture_dyt_pack_p = NULL);

		void setSkeleton(ESKOgre *v) {
			skeleton = v;
		}
};

#endif