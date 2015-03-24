namespace LibXenoverse {
	void EANBone::readName(File *file) {
		file->readString(&name);
		LOG_DEBUG("  Bone: %s\n", name.c_str());
	}

	void EANBone::readMatrix(File *file) {
		LOG_DEBUG("  Bone Matrix: \n", name.c_str());
		for (size_t i = 0; i < 12; i++) {
			file->readFloat32E(&matrix[i]);
		}

		LOG_DEBUG("     %f, %f, %f, %f\n", matrix[0], matrix[1], matrix[2], matrix[3]);
		LOG_DEBUG("     %f, %f, %f, %f\n", matrix[4], matrix[5], matrix[6], matrix[7]);
		LOG_DEBUG("     %f, %f, %f, %f\n", matrix[8], matrix[9], matrix[10], matrix[11]);
	}

	void EANBone::readIndices(File *file) {
		file->readInt16E(&parent_index);
		file->readInt16E(&child_index);
		file->readInt16E(&sibling_index);
		file->readInt16E(&index_4);

		LOG_DEBUG("  Bone Indices: %d %d %d %d\n", parent_index, child_index, sibling_index, index_4);
	}

	void EANBone::readFlags(File *file) {
		file->readInt32E(&flag_start);
		file->readInt32E(&flag_end);

		LOG_DEBUG("  Bone Start/End: %d %d\n\n", flag_start, flag_end);
	}
}