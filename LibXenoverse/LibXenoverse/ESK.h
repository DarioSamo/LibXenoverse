#ifndef LIBXENOVERSE_ESK_H_INCLUDED
#define LIBXENOVERSE_ESK_H_INCLUDED

#define LIBXENOVERSE_ESK_SIGNATURE    "#ESK"

namespace LibXenoverse {
	class ESKBone {
		public:
			float transform_matrix[16];
			float skinning_matrix[12];
			string name;

			unsigned short parent_index;
			unsigned short child_index;
			unsigned short sibling_index;
			unsigned short index_4;

			ESKBone() {
			}

			void readName(File *file);
			void readIndices(File *file);
			void readMatrix(File *file);
			void readSkinningMatrix(File *file);
	};

	class ESK {
		protected:
			vector<ESKBone *> bones;
		public:
			ESK() {
			}

			bool load(string filename);
			void save(string filename, bool big_endian = false);

			void read(File *file);
			void write(File *file);

#ifdef LIBXENOVERSE_FBX_SUPPORT
			void createFBXBone(FbxScene *scene, FbxNode *root_node, unsigned short parent_index, vector<FbxNode *> &fbx_bones);
			vector<FbxNode *> createFBXSkeleton(FbxScene *scene);
#endif
	};

}

#endif