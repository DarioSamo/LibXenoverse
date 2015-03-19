#ifndef LIBXENOVERSE_CUS_H_INCLUDED
#define LIBXENOVERSE_CUS_H_INCLUDED

#define LIBXENOVERSE_CUS_SIGNATURE    "#CUS"

namespace LibXenoverse {
	class CUS {
		protected:
		public:
			CUS() {
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