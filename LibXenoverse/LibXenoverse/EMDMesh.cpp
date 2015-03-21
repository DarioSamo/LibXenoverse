namespace LibXenoverse {
	void EMDMesh::read(File *file) {
		unsigned int address = 0;
		unsigned int base_mesh_address = file->getCurrentAddress();
		printf("Reading Mesh at %d\n", base_mesh_address);

		file->readFloat32E(&aabb_center_x);
		file->readFloat32E(&aabb_center_y);
		file->readFloat32E(&aabb_center_z);
		file->readFloat32E(&aabb_center_w);
		file->readFloat32E(&aabb_min_x);
		file->readFloat32E(&aabb_min_y);
		file->readFloat32E(&aabb_min_z);
		file->readFloat32E(&aabb_min_w);
		file->readFloat32E(&aabb_max_x);
		file->readFloat32E(&aabb_max_y);
		file->readFloat32E(&aabb_max_z);
		file->readFloat32E(&aabb_max_w);

		unsigned int mesh_name_offset = 0;
		unsigned short unknown_total = 0;
		unsigned short submesh_total = 0;
		unsigned int submesh_table_address = 0;

		file->readInt32E(&mesh_name_offset);
		file->readInt16E(&unknown_total);
		file->readInt16E(&submesh_total);
		file->readInt32E(&submesh_table_address);

		file->goToAddress(base_mesh_address + mesh_name_offset);
		file->readString(&name);

		LOG_DEBUG("Mesh Floats\n");
		LOG_DEBUG("%f %f %f %f\n", aabb_center_x, aabb_center_y, aabb_center_z, aabb_center_w);
		LOG_DEBUG("%f %f %f %f\n", aabb_min_x, aabb_min_y, aabb_min_z, aabb_min_w);
		LOG_DEBUG("%f %f %f %f\n\n", aabb_max_x, aabb_max_y, aabb_max_z, aabb_max_w);

		LOG_DEBUG("Mesh Name: %s\n", name.c_str());
		LOG_DEBUG("\nSubmesh Total: %d\n", submesh_total);
		
		for (size_t k = 0; k < submesh_total; k++) {
			file->goToAddress(base_mesh_address + submesh_table_address + k * 4);
			file->readInt32E(&address);
			file->goToAddress(base_mesh_address + address);

			unsigned int base_submesh_address = base_mesh_address + address;
			file->goToAddress(base_submesh_address);

			EMDSubmesh *emd_submesh = new EMDSubmesh();
			emd_submesh->read(file);
			submeshes.push_back(emd_submesh);
		}
	}

	void EMDMesh::write(File *file) {
		unsigned int base_mesh_address = file->getCurrentAddress();

		file->writeFloat32E(&aabb_center_x);
		file->writeFloat32E(&aabb_center_y);
		file->writeFloat32E(&aabb_center_z);
		file->writeFloat32E(&aabb_center_w);
		file->writeFloat32E(&aabb_min_x);
		file->writeFloat32E(&aabb_min_y);
		file->writeFloat32E(&aabb_min_z);
		file->writeFloat32E(&aabb_min_w);
		file->writeFloat32E(&aabb_max_x);
		file->writeFloat32E(&aabb_max_y);
		file->writeFloat32E(&aabb_max_z);
		file->writeFloat32E(&aabb_max_w);

		unsigned int mesh_name_offset = 0;
		unsigned short submesh_total = submeshes.size();
		unsigned int submesh_table_address = 0;
		file->writeNull(12);

		// Write Name
		mesh_name_offset = file->getCurrentAddress() - base_mesh_address;
		file->writeString(&name);
		file->fixPadding(0x4);

		// Write Submeshes
		submesh_table_address = file->getCurrentAddress() - base_mesh_address;
		file->writeNull(submesh_total * 4);
		
		for (size_t k = 0; k < submesh_total; k++) {
			file->fixPadding(16);
			unsigned int submesh_address = file->getCurrentAddress() - base_mesh_address;
			file->goToAddress(base_mesh_address + submesh_table_address + k * 4);
			file->writeInt32E(&submesh_address);
			file->goToAddress(base_mesh_address + submesh_address);
			submeshes[k]->write(file);
		}

		// Fix Offsets
		file->goToAddress(base_mesh_address + 0x30);
		file->writeInt32E(&mesh_name_offset);
		file->writeNull(2);
		file->writeInt16E(&submesh_total);
		file->writeInt32E(&submesh_table_address);
		file->goToEnd();
	}

	void EMDMesh::setVertexScale(float scale) {
		for (size_t i = 0; i < submeshes.size(); i++) {
			submeshes[i]->setVertexScale(scale);
		}
	}
}