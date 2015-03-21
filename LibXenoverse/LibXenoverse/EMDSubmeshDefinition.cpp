namespace LibXenoverse {
	void EMDSubmeshDefinition::read(File *file) {
		file->readUChar(&flag_1);
		file->readUChar(&tex_index);
		file->readUChar(&flag_2);
		file->readUChar(&flag_3);
		file->readFloat32E(&a);
		file->readFloat32E(&b);

		LOG_DEBUG("Submesh Definition: %d %d %d %d %f %f\n", flag_1, tex_index, flag_2, flag_3, a, b);
	}

	void EMDSubmeshDefinition::write(File *file) {
		file->writeUChar(&flag_1);
		file->writeUChar(&tex_index);
		file->writeUChar(&flag_2);
		file->writeUChar(&flag_3);
		file->writeFloat32E(&a);
		file->writeFloat32E(&b);
	}
}