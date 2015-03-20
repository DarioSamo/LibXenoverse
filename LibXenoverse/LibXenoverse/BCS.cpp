namespace LibXenoverse {
	BCS::BCS() {
	}

	bool BCS::load(string filename) {
		File file(filename, LIBXENOVERSE_FILE_READ_BINARY);

		if (file.valid() && file.readHeader(LIBXENOVERSE_BCS_SIGNATURE)) {
			read(&file);
			file.close();
		}
		else return false;
		return true;
	}

	void BCS::read(File *file) {
		file->goToAddress(12);

		unsigned short unknown_total_1 = 0;
		unsigned short unknown_total_2 = 0;
		file->readInt16E(&unknown_total_1);
		file->readInt16E(&unknown_total_2);


		printf("Found %d things.\n", unknown_total_1);
		file->moveAddress(4);

		unsigned int unknown_offset_1 = 0;
		unsigned int unknown_offset_2 = 0;
		file->readInt32E(&unknown_offset_1);
		file->readInt32E(&unknown_offset_2);

		sections.resize(unknown_total_1);
		for (size_t i = 0; i < unknown_total_1; i++) {
			file->goToAddress(unknown_offset_1 + i*4);
			unsigned int address = 0;
			file->readInt32E(&address);

			if (address) {
				file->goToAddress(address);
				sections[i] = new BCSSection();
				sections[i]->read(file);
			}
			else {
				sections[i] = NULL;
			}
		}
	}


	void BCS::save(string filename, bool big_endian) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.writeHeader(LIBXENOVERSE_BCS_SIGNATURE, big_endian);
			write(&file);
			file.close();
		}
	}

	void BCS::write(File *file) {

	}
}