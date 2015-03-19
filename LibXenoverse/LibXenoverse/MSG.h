#ifndef LIBXENOVERSE_MSG_H_INCLUDED
#define LIBXENOVERSE_MSG_H_INCLUDED

#define LIBXENOVERSE_MSG_SIGNATURE    "#MSG"

#define LIBXENOVERSE_MSG_UTF8         0
#define LIBXENOVERSE_MSG_UTF16        1

namespace LibXenoverse {
	class MSGEntry {
	public:
		bool utf_mode;

		string utf8_string;
		u16string utf16_string;

		MSGEntry() {
			utf_mode = false;
			utf8_string.clear();
			utf16_string.clear();
		}

		void read(File *file, bool utf);
		void write(File *file);
	};

	class MSGSlot {
	public:
		MSGEntry slot_text;
		vector<MSGEntry> text_entries;
		unsigned int id;

		MSGSlot() {
		}

		void readText(File *file, bool utf);
		void readEntries(File *file, bool utf);
		void readID(File *file);

		void write(File *file);
	};


	class MSG {
	protected:
		vector<MSGSlot> slots;

		unsigned char utf_mode_slots;
		unsigned char utf_mode_text;
	public:
		MSG() {
		}

		bool load(string filename);
		void save(string filename, bool big_endian = false);
		void read(File *file);
		void write(File *file);
	};
}


#endif