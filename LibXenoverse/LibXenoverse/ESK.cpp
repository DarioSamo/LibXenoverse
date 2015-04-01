namespace LibXenoverse {
	bool ESK::load(string filename) {
		name = nameFromFilenameNoExtension(filename, true);

		File file(filename, LIBXENOVERSE_FILE_READ_BINARY);
		if (file.valid() && file.readHeader(LIBXENOVERSE_ESK_SIGNATURE)) {
			file.goToAddress(0x10);
			unsigned int address = 0;
			file.readInt32E(&address);
			file.goToAddress(address);
			read(&file);
			file.close();
		}
		else return false;

		return true;
	}

	void ESK::read(File *file) {
		unsigned int base_skeleton_address = file->getCurrentAddress();
		unsigned short bone_count = 0;
		file->readInt16E(&bone_count);
		file->readInt16E(&flag);

		bones.resize(bone_count);
		for (size_t i = 0; i < bone_count; i++) {
			bones[i] = new ESKBone();
		}

		unsigned int bone_indices_offset = 0;
		unsigned int bone_names_offset = 0;
		unsigned int skinning_matrix_offset = 0;
		unsigned int transform_matrix_offset = 0;
		unsigned int unknown_offset_3 = 0;
		unsigned int unknown_offset_4 = 0;

		file->readInt32E(&bone_indices_offset);
		file->readInt32E(&bone_names_offset);
		file->readInt32E(&skinning_matrix_offset);
		file->readInt32E(&transform_matrix_offset);
		file->readInt32E(&unknown_offset_3);
		file->readInt32E(&unknown_offset_4);

		// Read Bone Indices
		for (size_t i = 0; i < bone_count; i++) {
			unsigned int address = 0;
			file->goToAddress(base_skeleton_address + bone_indices_offset + i * 8);

			bones[i]->readIndices(file);
		}

		// Read Bone Names
		for (size_t i = 0; i < bone_count; i++) {
			unsigned int address = 0;
			file->goToAddress(base_skeleton_address + bone_names_offset + i*4);
			file->readInt32E(&address);
			file->goToAddress(base_skeleton_address + address);

			bones[i]->readName(file);
		}

		// Read Skinning Matrices
		for (size_t i = 0; i < bone_count; i++) {
			unsigned int address = 0;
			file->goToAddress(base_skeleton_address + skinning_matrix_offset + i * 48);

			bones[i]->readSkinningMatrix(file);
		}

		// Read Bone Matrices
		if (transform_matrix_offset) {
			for (size_t i = 0; i < bone_count; i++) {
				unsigned int address = 0;
				file->goToAddress(base_skeleton_address + transform_matrix_offset + i * 64);

				bones[i]->readMatrix(file);
			}
		}
	}
}