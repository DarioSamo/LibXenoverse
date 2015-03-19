#ifndef LIBXENOVERSE_AGD_H_INCLUDED
#define LIBXENOVERSE_AGD_H_INCLUDED

#define LIBXENOVERSE_AGD_SIGNATURE    "#AGD"

namespace LibXenoverse {
	class AGDGrowth {
	public:
		unsigned int level;
		unsigned int xp_to_next_level;
		unsigned int base_xp;
		unsigned int flag;

		AGDGrowth() {
		}

		void read(File *file);
		void readXML(TiXmlElement *root);
		void write(File *file);
		void writeXML(TiXmlElement *root);
	};

	class AGD {
	protected:
		vector<AGDGrowth> growths;
	public:
		AGD() {
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