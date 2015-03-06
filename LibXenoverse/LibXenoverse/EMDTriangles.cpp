namespace LibXenoverse {
	void EMDTriangles::read(File *file) {
		unsigned int base_face_address = file->getCurrentAddress();
		unsigned int face_count = 0;
		unsigned int face_name_count = 0;
		unsigned int face_table_address = 0;
		unsigned int face_name_table_address = 0;

		file->readInt32E(&face_count);
		file->readInt32E(&face_name_count);
		file->readInt32E(&face_table_address);
		file->readInt32E(&face_name_table_address);

		#ifdef LIBXENOVERSE_DEBUGGING_LOG
		fprintf(global_debugging_log, "Triangle List Face Count: %d\n", face_count);
		fprintf(global_debugging_log, "Triangle List Bone Count: %d\n", face_name_count);
		#endif

		printf("Submesh Face Count: %d\n", face_count);
		printf("Submesh Face Name Count: %d\n", face_name_count);

		// Read Face Indices
		faces.resize(face_count);

		for (size_t n = 0; n < face_count; n++) {
			file->goToAddress(base_face_address + face_table_address + n * 2);
			unsigned short face = 0;
			file->readInt16E(&face);
			faces[n] = face;

			#ifdef LIBXENOVERSE_DEBUGGING_LOG
			fprintf(global_debugging_log, "%d ", face);
			#endif
		}

		#ifdef LIBXENOVERSE_DEBUGGING_LOG
		fprintf(global_debugging_log, "\n");
		#endif

		// Read Face Names
		for (size_t n = 0; n < face_name_count; n++) {
			unsigned int address = 0;
			file->goToAddress(base_face_address + face_name_table_address + n * 4);
			file->readInt32E(&address);
			file->goToAddress(base_face_address + address);

			string face_name = "";
			file->readString(&face_name);
			bone_names.push_back(face_name);

			printf("Face Name %d: %s\n", n, face_name.c_str());
		}
	}


	void EMDTriangles::write(File *file) {
		unsigned int base_face_address = file->getCurrentAddress();
		file->writeNull(16);

		// Write Indices
		unsigned int face_table_address = file->getCurrentAddress() - base_face_address;
		for (size_t i = 0; i < faces.size(); i++) {
			file->writeInt16E(&faces[i]);
		}
		file->fixPadding(0x4);

		// Write Names
		unsigned int face_name_table_address = file->getCurrentAddress() - base_face_address;
		file->writeNull(bone_names.size() * 4);

		for (size_t i = 0; i < bone_names.size(); i++) {
			unsigned int name_address = file->getCurrentAddress() - base_face_address;
			file->goToAddress(base_face_address + face_name_table_address + i * 4);
			file->writeInt32E(&name_address);
			file->goToAddress(base_face_address + name_address);
			file->writeString(&bone_names[i]);
		}
		file->fixPadding(0x4);

		// Write Face Header
		file->goToAddress(base_face_address);
		unsigned int face_count = faces.size();
		unsigned int face_name_count = bone_names.size();
		file->writeInt32E(&face_count);
		file->writeInt32E(&face_name_count);
		file->writeInt32E(&face_table_address);
		file->writeInt32E(&face_name_table_address);

		file->goToEnd();
	}
}