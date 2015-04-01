#ifndef LIBXENOVERSE_EMM_H_INCLUDED
#define LIBXENOVERSE_EMM_H_INCLUDED

#define LIBXENOVERSE_EMM_SIGNATURE  "#EMM"

namespace LibXenoverse {
	class EMMParameter {
		public:
			char name[32];
			unsigned int type;
			unsigned int value;

			EMMParameter() {
			}

			void read(File *file);
			void readXML(TiXmlElement *root);
			void write(File *file);
			void writeXML(TiXmlElement *root);

			float valueFloat() {
				return *(float *)(&value);
			}
	};

	class EMMMaterial {
		protected:
			char name[32];
			char shader_name[32];
			vector<EMMParameter *> parameters;
		public:
			EMMMaterial() {
			}

			string getName() {
				return string(name);
			}

			string getShaderName() {
				return string(shader_name);
			}

			vector<EMMParameter *> &getParameters() {
				return parameters;
			}

			void read(File *file);
			void readXML(TiXmlElement *root);
			void write(File *file);
			void writeXML(TiXmlElement *root);
	};

	class EMM {
		protected:
			vector<EMMMaterial *> materials;
			string name;
		public:
			EMM() {
			}

			vector<EMMMaterial *> &getMaterials() {
				return materials;
			}

			bool load(string filename);
			void save(string filename, bool big_endian=false);
			void saveXML(string filename);

			void read(File *file);
			void readXML(TiXmlElement *root);
			void write(File *file);

			string getName() {
				return name;
			}
	};
}

#endif