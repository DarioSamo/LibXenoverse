#ifndef EMDOGRE_H_INCLUDED
#define EMDOGRE_H_INCLUDED

class EMBOgre;
class ESKOgre;
class EMMOgre;
class EMDRenderObject;

class EMDOgre : public EMD {
	protected:
		bool mesh_resources_created;
		EMMOgre *material_pack;
		ESKOgre *skeleton;
		bool to_rebuild;
		bool to_delete;

		list<Ogre::SceneNode *> scene_nodes;
		list<EMDRenderObject *> created_render_objects;

		list<Ogre::String> created_meshes;
	public:
		EMDOgre();
		~EMDOgre();

		Ogre::SubMesh *createOgreSubmesh(EMDTriangles *triangles, Ogre::MeshPtr mesh);
		void createOgreMesh(EMDSubmesh *submesh, string mesh_name);
		void createOgreEntityMeshes(EMDMesh *mesh);
		Ogre::SceneNode *createOgreSceneNodeModel(EMDModel *model, Ogre::SceneNode *parent, Ogre::SceneManager *scene_manager);
		void createOgreSceneNodeEMD(Ogre::SceneNode *parent, Ogre::SceneManager *scene_manager);
		Ogre::SceneNode *createOgreSceneNode(Ogre::SceneManager *scene_manager);

		void setMaterialPack(EMMOgre *v) {
			material_pack = v;
		}

		EMMOgre *getMaterialPack() {
			return material_pack;
		}

		void setSkeleton(ESKOgre *v) {
			skeleton = v;
		}

		ESKOgre *getSkeleton() {
			return skeleton;
		}

		void tagForRebuild() {
			to_rebuild = true;
		}

		bool toRebuild() {
			return to_rebuild;
		}

		void tagForDeletion() {
			to_delete = true;
		}

		bool toDelete() {
			return to_delete;
		}

		void rebuild();
		void cleanNodes(bool parent=false);
		void destroyResources();
};

#endif