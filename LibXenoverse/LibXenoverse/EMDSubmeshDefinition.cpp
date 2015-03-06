namespace LibXenoverse {
	void EMDSubmeshDefinition::read(File *file) {
		file->readInt32E(&flag);
		file->readFloat32E(&a);
		file->readFloat32E(&b);
	}

	void EMDSubmeshDefinition::write(File *file) {
		file->writeInt32E(&flag);
		file->writeFloat32E(&a);
		file->writeFloat32E(&b);
	}
}