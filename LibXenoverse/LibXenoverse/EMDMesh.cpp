namespace LibXenoverse {
	void EMDMesh::read(File *file) {
		unsigned int address = 0;
		unsigned int base_mesh_address = file->getCurrentAddress();
		printf("Reading Mesh at %d\n", base_mesh_address);

		for (size_t x = 0; x < 12; x++) {
			file->readFloat32E(&float_group[x]);
			printf("%f ", float_group[x]);
			if ((x + 1) % 4 == 0) printf("\n");
		}

		#ifdef LIBXENOVERSE_DEBUGGING_LOG
		fprintf(global_debugging_log, "Mesh Floats\n");
		fprintf(global_debugging_log, "%f %f %f %f\n", float_group[0], float_group[1], float_group[2], float_group[3]);
		fprintf(global_debugging_log, "%f %f %f %f\n", float_group[4], float_group[5], float_group[6], float_group[7]);
		fprintf(global_debugging_log, "%f %f %f %f\n\n", float_group[8], float_group[9], float_group[10], float_group[11]);
		#endif

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
		printf("Mesh Name: %s\n", name.c_str());
		printf("\nSubmesh Total: %d\n", submesh_total);

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

		for (size_t x = 0; x < 12; x++) file->writeFloat32E(&float_group[x]);

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