namespace LibXenoverse {
	bool EAN::load(string filename) {
		name = nameFromFilenameNoExtension(filename, true);

		File file(filename, LIBXENOVERSE_FILE_READ_BINARY);
		if (file.valid() && file.readHeader(LIBXENOVERSE_EAN_SIGNATURE)) {
			read(&file);
			file.close();
		}
		else return false;

		return true;
	}

	void EAN::read(File *file) {
		file->goToAddress(8);

		unsigned int unknown_total = 0;
		unsigned int skeleton_offset = 0;
		unsigned int animation_keyframes_offset = 0;
		unsigned int animation_names_offset = 0;
		unsigned short animation_count = 0;

		file->readInt32E(&unknown_total);
		file->moveAddress(6);
		file->readInt16E(&animation_count);
		file->readInt32E(&skeleton_offset);
		file->readInt32E(&animation_keyframes_offset);
		file->readInt32E(&animation_names_offset);

		// Read Skeleton
		file->goToAddress(skeleton_offset);
		skeleton = new EANSkeleton();
		skeleton->read(file);

		// Read Animations
		animations.resize(animation_count);
		for (size_t i = 0; i < animation_count; i++) {
			// Read Name
			file->goToAddress(animation_names_offset + i*4);
			unsigned int address = 0;
			file->readInt32E(&address);
			file->goToAddress(address);
			animations[i].readName(file);

			// Read
			file->goToAddress(animation_keyframes_offset + i*4);
			address = 0;
			file->readInt32E(&address);
			file->goToAddress(address);

			animations[i].setParent(this);
			animations[i].read(file);
		}
	}

	void EAN::save(string filename, bool big_endian) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.writeHeader(LIBXENOVERSE_EAN_SIGNATURE, big_endian);
			write(&file);
			file.close();
		}
	}

	void EAN::write(File *file) {

	}
}