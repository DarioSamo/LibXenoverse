namespace LibXenoverse {
	void EANSkeleton::read(File *file) {
		unsigned int base_tree_address = file->getCurrentAddress();

		unsigned short node_count = 0;
		unsigned short unknown_count = 0;
		file->readInt16E(&node_count);
		file->readInt16E(&unknown_count);

		LOG_DEBUG("Skeleton Bone Count: %d %d\n", node_count, unknown_count);

		unsigned int index_table_offset = 0;
		unsigned int node_name_table_offset = 0;
		unsigned int matrix_table_offset = 0;
		unsigned int unknown_offset = 0;
		unsigned int unknown_offset_2 = 0;
		unsigned int flag_table_offset = 0;
		file->readInt32E(&index_table_offset);
		file->readInt32E(&node_name_table_offset);
		file->readInt32E(&matrix_table_offset);
		file->readInt32E(&unknown_offset);
		file->readInt32E(&unknown_offset_2);
		file->readInt32E(&flag_table_offset);

		bones.resize(node_count);

		for (size_t i = 0; i < node_count; i++) {
			file->goToAddress(base_tree_address + node_name_table_offset + i * 4);
			unsigned int address = 0;
			file->readInt32E(&address);
			file->goToAddress(base_tree_address + address);
			bones[i].readName(file);

			file->goToAddress(base_tree_address + matrix_table_offset + i * 48);
			bones[i].readMatrix(file);

			file->goToAddress(base_tree_address + index_table_offset + i * 8);
			bones[i].readIndices(file);

			file->goToAddress(base_tree_address + flag_table_offset + i * 8);
			bones[i].readFlags(file);
		}
	}


	void EANSkeleton::write(File *file) {

	}
}