namespace LibXenoverse {
	void EMDModel::read(File *file) {
		unsigned int address = 0;
		unsigned int base_model_address = file->getCurrentAddress();
		unsigned short unknown_total = 0;
		unsigned short mesh_total = 0;
		unsigned int mesh_table_address = 0;
		file->readInt16E(&unknown_total);
		file->readInt16E(&mesh_total);
		file->readInt32E(&mesh_table_address);

		LOG_DEBUG("\n\nModel - Mesh Count: %d\n", mesh_total);

		for (size_t j = 0; j < mesh_total; j++) {
			file->goToAddress(base_model_address + mesh_table_address + j * 4);
			file->readInt32E(&address);

			unsigned int base_mesh_address = base_model_address + address;
			file->goToAddress(base_mesh_address);

			EMDMesh *emd_mesh = new EMDMesh();
			emd_mesh->read(file);
			meshes.push_back(emd_mesh);
		}
	}


	void EMDModel::write(File *file) {
		unsigned int base_model_address = file->getCurrentAddress();
		unsigned short mesh_total = meshes.size();
		unsigned int mesh_table_address = 8;
		file->writeNull(2);
		file->writeInt16E(&mesh_total);
		file->writeInt32E(&mesh_table_address);

		file->writeNull(mesh_total * 4);
		
		for (size_t j = 0; j < mesh_total; j++) {
			file->fixPadding(16);
			unsigned int mesh_address = file->getCurrentAddress() - base_model_address;
			file->goToAddress(base_model_address + mesh_table_address + j * 4);
			file->writeInt32E(&mesh_address);
			file->goToAddress(base_model_address + mesh_address);
			meshes[j]->write(file);
		}
	}

	void EMDModel::setVertexScale(float scale) {
		for (size_t i = 0; i < meshes.size(); i++) {
			meshes[i]->setVertexScale(scale);
		}
	}
}