namespace LibXenoverse {
	bool MSG::load(string filename) {
		File file(filename, LIBXENOVERSE_FILE_READ_BINARY);

		if (file.valid() && file.readHeader(LIBXENOVERSE_MSG_SIGNATURE, LIBXENOVERSE_LITTLE_ENDIAN)) {
			read(&file);
			file.close();
		}
		else return false;

		return true;
	}

	void MSG::read(File *file) {
		file->goToAddress(4);
		file->moveAddress(1);
		file->readUChar(&utf_mode_slots);
		file->readUChar(&utf_mode_text);
		file->moveAddress(1);

		unsigned int message_slot_entries_total = 0;
		unsigned int message_slot_text_address_table = 0;
		unsigned int message_slot_id_address_table = 0;
		unsigned int message_slot_entries_address_table = 0;
		//unsigned int message_text_entries_total = 0;
		//unsigned int message_text_address_table = 0;
		file->readInt32E(&message_slot_entries_total);
		file->readInt32E(&message_slot_text_address_table);
		file->readInt32E(&message_slot_id_address_table);
		file->readInt32E(&message_slot_entries_address_table);
		//file->readInt32E(&message_text_entries_total);
		//file->readInt32E(&message_text_address_table);

		printf("\nFound %d Message Slots:\n\n", message_slot_entries_total);
		slots.resize(message_slot_entries_total);

		// Read Slot Text Entries
		for (size_t i = 0; i < message_slot_entries_total; i++) {
			file->goToAddress(message_slot_text_address_table + i * 16);
			slots[i].readText(file, (utf_mode_slots == 1));

			file->goToAddress(message_slot_id_address_table + i * 4);
			slots[i].readID(file);

			file->goToAddress(message_slot_entries_address_table + i * 8);
			slots[i].readEntries(file, (utf_mode_text == 1));
		}
	}


	void MSG::save(string filename, bool big_endian) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.writeHeader(LIBXENOVERSE_MSG_SIGNATURE, big_endian);
			write(&file);
			file.close();
		}
	}

	void MSG::write(File *file) {

	}
}