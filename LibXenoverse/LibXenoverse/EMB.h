#ifndef LIBXENOVERSE_EMB_H_INCLUDED
#define LIBXENOVERSE_EMB_H_INCLUDED

#define LIBXENOVERSE_EMB_SIGNATURE    "#EMB"

namespace LibXenoverse {
	class EMBFile {
		protected:
			char *data;
			size_t data_size;
			string name;
		public:
			EMBFile(string filename);

			EMBFile(char *data_p, size_t data_size_p) {
				data = data_p;
				data_size = data_size_p;
				name = "";
			}

			void setName(string name_p) {
				name = name_p;
			}

			string getName() {
				return name;
			}

			void write(File *file);

			void save(string filename);

			size_t getSize() {
				return data_size;
			}
	};


	class EMB {
		protected:
			vector<EMBFile *> files;
		public:
			EMB() {
			}

			EMB(string filename);
			void extract(string folder);
			void save(string filename, bool enable_filenames = true, bool big_endian = false);

			void addFile(string filename);
			void addFolder(string folder);

			bool detectFilenameMode();
	};
}

#endif