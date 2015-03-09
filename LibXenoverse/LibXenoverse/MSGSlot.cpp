namespace LibXenoverse {
	void MSGSlot::readText(File *file, bool utf) {
		slot_text.read(file, utf);
	}

	void MSGSlot::readEntries(File *file, bool utf) {
		unsigned int entries_total = 0;
		unsigned int entries_address = 0;
		file->readInt32E(&entries_total);
		file->readInt32E(&entries_address);

		text_entries.resize(entries_total);

		for (size_t i = 0; i < entries_total; i++) {
			file->goToAddress(entries_address + i * 16);
			text_entries[i].read(file, utf);
		}
	}

	void MSGSlot::readID(File *file) {
		file->readInt32E(&id);
	}
}