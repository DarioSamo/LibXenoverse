#ifdef LIBXENOVERSE_ZLIB_SUPPORT

#ifndef LIBXENOVERSE_EMZ_H_INCLUDED
#define LIBXENOVERSE_EMZ_H_INCLUDED

#define LIBXENOVERSE_EMZ_SIGNATURE           "#EMZ"
#define LIBXENOVERSE_EMZ_CHUNK               16384

namespace LibXenoverse {
	class EMZ {
	protected:
		unsigned char *data;
		unsigned int data_size;
	public:
		EMZ() {
			data = NULL;
			data_size = 0;
		}

		~EMZ() {
			delete data;
		}

		EMZ(string filename);
		void saveUncompressed(string filename);

		string detectNewExtension();
	};
}

#endif

#endif