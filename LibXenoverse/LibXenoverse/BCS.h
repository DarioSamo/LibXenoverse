#ifndef LIBXENOVERSE_BCS_H_INCLUDED
#define LIBXENOVERSE_BCS_H_INCLUDED

#define LIBXENOVERSE_BCS_SIGNATURE    "#BCS"

namespace LibXenoverse {
	class BCSSection {
		protected:
			unsigned int flag;
		public:
			BCSSection();
			void read(File *file);
			void write(File *file);
	};

	class BCS {
	protected:
		vector<BCSSection *> sections;
	public:
		BCS();
		bool load(string filename);
		void save(string filename, bool big_endian = false);
		void read(File *file);
		void write(File *file);
	};
}

#endif