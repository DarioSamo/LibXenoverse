namespace LibXenoverse {
	BCSSection::BCSSection() {


	}

	void BCSSection::read(File *file) {
		unsigned int base_section_address = file->getCurrentAddress();
		file->moveAddress(20);
		file->readInt32E(&flag);

		printf("BCS Section Flag: %d\n", flag);
	}

	void BCSSection::write(File *file) {

	}
}