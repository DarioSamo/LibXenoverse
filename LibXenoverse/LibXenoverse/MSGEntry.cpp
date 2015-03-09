namespace LibXenoverse {
	void MSGEntry::read(File *file, bool utf) {
		utf_mode = utf;

		unsigned int slot_name_address = 0;
		unsigned int slot_name_character_length = 0;
		unsigned int slot_name_character_real_length = 0;

		file->readInt32E(&slot_name_address);
		file->readInt32E(&slot_name_character_length);
		file->readInt32E(&slot_name_character_real_length);

		file->goToAddress(slot_name_address);
		if (utf_mode) {
			file->readString16(&utf16_string);
			//wprintf(L"%s\n", utf16_string.c_str());
		}
		else {
			file->readString(&utf8_string);
			//printf("%s\n", utf8_string.c_str());
		}
	}

	void MSGEntry::write(File *file) {

	}
}