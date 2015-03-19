#ifndef LIBXENOVERSE_CBS_H_INCLUDED
#define LIBXENOVERSE_CBS_H_INCLUDED

#define LIBXENOVERSE_CBS_SIGNATURE    "#CBS"

namespace LibXenoverse {
	class CBSScale {
	public:
		unsigned short id_1;
		unsigned short id_2;
		float x;
		float y;
		float z;

		CBSScale() {
		}

		void read(File *file);
		void readXML(TiXmlElement *root);
		void write(File *file);
		void writeXML(TiXmlElement *root);
	};

	class CBS {
	protected:
		vector<CBSScale> scales;
	public:
		CBS() {
		}

		bool load(string filename);
		void save(string filename, bool big_endian = false);
		void saveXML(string filename);

		void read(File *file);
		void readXML(TiXmlElement *root);
		void write(File *file);
	};
}

#endif