namespace LibXenoverse {
	void ESKBone::readName(File *file) {
		file->readString(&name);

		LOG_DEBUG("Bone - %s\n", name.c_str());
	}

	void ESKBone::readIndices(File *file) {
		file->readInt16E(&parent_index);
		file->readInt16E(&child_index);
		file->readInt16E(&sibling_index);
		file->readInt16E(&index_4);

		printf("Indices - %d %d %d %d\n", parent_index, child_index, sibling_index, index_4);
	}

	void ESKBone::readMatrix(File *file) {
		for (size_t i = 0; i < 16; i++) {
			file->readFloat32E(&transform_matrix[i]);
		}
	}

	void ESKBone::readSkinningMatrix(File *file) {
		printf("Skinning Matrix:\n");

		for (size_t i = 0; i < 12; i++) {
			file->readFloat32E(&skinning_matrix[i]);
			printf("%f ", skinning_matrix[i]);
			if (((i + 1) % 4) == 0) printf("\n");
		}
	}
}