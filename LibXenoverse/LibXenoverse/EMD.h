#ifndef LIBXENOVERSE_EMD_H_INCLUDED
#define LIBXENOVERSE_EMD_H_INCLUDED

#define LIBXENOVERSE_EMD_SIGNATURE           "#EMD"
#define LIBXENOVERSE_EMD_SUBMESH_BONE_LIMIT  24

#include "EMM.h"

namespace LibXenoverse {
	class EMDVertex {
		friend class EMD;

		public:
			float x, y, z;
			float bone_weight[4];
			unsigned char bone[4];
			float nx, ny, nz;
			float u, v;
			

			EMDVertex() {
			}

			void zero() {
				x = y = z = nx = ny = nz = u = v = 0.0f;
				bone[0] = bone[1] = bone[2] = bone[3] = 0;
				bone_weight[0] = 1.0f;
				bone_weight[1] = bone_weight[2] = bone_weight[3] = 0.0f;
			}

			bool operator == (const EMDVertex& vertex) {
				if (x != vertex.x) return false;
				if (y != vertex.y) return false;
				if (z != vertex.z) return false;
				if (nx != vertex.nx) return false;
				if (ny != vertex.ny) return false;
				if (nz != vertex.nz) return false;
				if (u != vertex.u) return false;
				if (v != vertex.v) return false;

				for (size_t i = 0; i<4; i++) {
					if (bone[i] != vertex.bone[i]) return false;
					if (bone_weight[i] != vertex.bone_weight[i]) return false;
				}

				return true;
			}

			void read(File *file, unsigned int vertex_type_flag);
			void write(File *file, unsigned int vertex_type_flag);

	};

	class EMDTriangles {
		friend class EMD;

		public:
			vector<unsigned short> faces;
			vector<string> bone_names;

			EMDTriangles() {
			}

			void read(File *file);
			void write(File *file);

			#ifdef LIBXENOVERSE_FBX_SUPPORT
				void exportFBX(FbxMesh *lMesh, unsigned int &control_point_base, int fbx_material_index);
				void exportFBXSkin(vector<FbxCluster *> &skin_clusters, vector<EMDVertex> &vertex_pool, unsigned int &control_point_base);
			#endif
	};

	class EMDSubmeshDefinition {
		friend class EMD;

		public:
			unsigned char flag_1;
			unsigned char tex_index;
			unsigned char flag_2;
			unsigned char flag_3;
			float a;
			float b;

			EMDSubmeshDefinition() {
			}

			void read(File *file);
			void write(File *file);
	};

	class EMDSubmesh {
		friend class EMD;
		friend class EMDMesh;

		protected:
			vector<EMDVertex> vertices;
			vector<EMDTriangles> triangles;
			vector<EMDSubmeshDefinition> definitions;
			float aabb_center_x;
			float aabb_center_y;
			float aabb_center_z;
			float aabb_center_w;
			float aabb_min_x;
			float aabb_min_y;
			float aabb_min_z;
			float aabb_min_w;
			float aabb_max_x;
			float aabb_max_y;
			float aabb_max_z;
			float aabb_max_w;

			unsigned int vertex_type_flag;
			unsigned int vertex_size;

			string name;
			int fbx_material_index;
		public:
			EMDSubmesh() {
				fbx_material_index = -1;
			}

			void read(File *file);
			void write(File *file);

			void setVertexScale(float scale);

			unsigned int getTotalPointCount();
			void getMaterialNames(vector<string> &material_names);

			string getMaterialName() {
				return name;
			}

			vector<EMDVertex> &getVertices() {
				return vertices;
			}
			
			vector<EMDTriangles> &getTriangles() {
				return triangles;
			}

			vector<EMDSubmeshDefinition> &getDefinitions() {
				return definitions;
			}

			/** Get the best triangle list suited for the array of bone names.
			    If there's no good triangle list that can fit all the bone names,
				create a new one and return it.

				A triangle list won't be fit for the bone names if it has reached 
				the bone limit (24), so it's necessary to create new ones depending on
				the bone count of the submesh.				
			*/
			EMDTriangles *getTriangleListFor(vector<string> bone_names);

			#ifdef LIBXENOVERSE_FBX_SUPPORT
				void importFBX(FbxMesh *lMesh, int material_index, FbxAMatrix transform_matrix, vector<string> &material_names, vector<vector<pair<double, FbxNode *>>> &control_points_skin_bindings);
				void exportFBX(FbxMesh *lMesh, unsigned int &control_point_base, FbxGeometryElementNormal *lGeometryElementNormal, FbxGeometryElementUV *lGeometryUVElement);
				void exportFBXSkin(vector<FbxCluster *> &skin_clusters, unsigned int &control_point_base);
			#endif
	};

	class EMDMesh {
		friend class EMD;

		protected:
			vector<EMDSubmesh *> submeshes;
			float aabb_center_x;
			float aabb_center_y;
			float aabb_center_z;
			float aabb_center_w;
			float aabb_min_x;
			float aabb_min_y;
			float aabb_min_z;
			float aabb_min_w;
			float aabb_max_x;
			float aabb_max_y;
			float aabb_max_z;
			float aabb_max_w;
			string name;
		public:
			EMDMesh() {
			}

			void read(File *file);
			void write(File *file);

			void setVertexScale(float scale);

			string getName() {
				return name;
			}
			
			unsigned int getTotalPointCount();
			void getMaterialNames(vector<string> &material_names);

			vector<EMDSubmesh *> &getSubmeshes() {
				return submeshes;
			}

			#ifdef LIBXENOVERSE_FBX_SUPPORT
				void importFBX(FbxNode *lNode);
				void exportFBX(FbxMesh *lMesh, unsigned int &control_point_base, FbxGeometryElementNormal *lGeometryElementNormal, FbxGeometryElementUV *lGeometryUVElement);
				void exportFBXSkin(vector<FbxCluster *> &skin_clusters, unsigned int &control_point_base);
			#endif
	};

	class EMDModel {
		friend class EMD;

		protected:
			vector<EMDMesh *> meshes;
		public:
			EMDModel() {
			}

			void read(File *file);
			void write(File *file);

			void setVertexScale(float scale);

			vector<string> getMeshNames() {
				vector<string> names;
				for (size_t i = 0; i < meshes.size(); i++) {
					names.push_back(meshes[i]->getName());
				}
				return names;
			}

			vector<EMDMesh *> &getMeshes() {
				return meshes;
			}

			unsigned int getTotalPointCount();
			void getMaterialNames(vector<string> &material_names);

			#ifdef LIBXENOVERSE_FBX_SUPPORT
				void importFBX(FbxNode *lNode);
				void exportFBX(FbxMesh *lMesh, unsigned int &control_point_base, FbxGeometryElementNormal *lGeometryElementNormal, FbxGeometryElementUV *lGeometryUVElement);
				void exportFBXSkin(vector<FbxCluster *> &skin_clusters, unsigned int &control_point_base);
			#endif
	};

	class EMD {
		protected:
			vector<EMDModel *> models;
			vector<string> model_names;
			string name;
		public:
			EMD() {
			}

			bool load(string filename);
			void save(string filename, bool big_endian = false);

			void read(File *file);
			void write(File *file);

			void setVertexScale(float scale);

			#ifdef LIBXENOVERSE_FBX_SUPPORT
				FbxMesh *exportFBX(FbxScene *scene, FbxNode *lMeshNode, string path);
				void exportFBXSkin(FbxScene *scene, FbxMesh *fbx_mesh, vector<FbxNode *> &fbx_bones, FbxAMatrix skin_matrix);
				FbxSurfaceMaterial *exportFBXMaterial(FbxScene *scene, string material_name, EMM *material, EMB *texture_pack, EMB *texture_dyt_pack);
				void importFBX(FbxNode *lNode);
			#endif

			unsigned int getTotalPointCount();
			vector<string> getMaterialNames();

			string getName() {
				return name;
			}
	};

}

#endif