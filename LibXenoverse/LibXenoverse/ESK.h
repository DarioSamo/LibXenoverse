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

			string getName() {
				return name;
			}
	};

	class ESK {
		protected:
			vector<ESKBone *> bones;
			string name;
			unsigned short flag;
		public:
			ESK() {
				name = "";
				flag = 0;
			}

			bool load(string filename);
			void save(string filename, bool big_endian = false);

			void read(File *file);
			void write(File *file);

			string getName() {
				return name;
			}

			vector<ESKBone *> &getBones() {
				return bones;
			}

			unsigned short getFlag() {
				return flag;
			}

#ifdef LIBXENOVERSE_FBX_SUPPORT
			void createFBXBone(FbxScene *scene, FbxNode *root_node, unsigned short parent_index, vector<FbxNode *> &fbx_bones);
			vector<FbxNode *> createFBXSkeleton(FbxScene *scene);
#endif
	};

}

#endif