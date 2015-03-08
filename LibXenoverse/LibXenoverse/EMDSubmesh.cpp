namespace LibXenoverse {
	void EMDSubmesh::read(File *file) {
		unsigned int address = 0;
		unsigned int base_submesh_address = file->getCurrentAddress();

		// Read Submesh
		printf("\nReading Submesh at %d\n", base_submesh_address);

		for (size_t x = 0; x < 12; x++) {
			file->readFloat32E(&float_group[x]);
			printf("%f ", float_group[x]);
			if ((x + 1) % 4 == 0) printf("\n");
		}

		#ifdef LIBXENOVERSE_DEBUGGING_LOG
		fprintf(global_debugging_log, "Submesh Floats\n");
		fprintf(global_debugging_log, "%f %f %f %f\n", float_group[0], float_group[1], float_group[2], float_group[3]);
		fprintf(global_debugging_log, "%f %f %f %f\n", float_group[4], float_group[5], float_group[6], float_group[7]);
		fprintf(global_debugging_log, "%f %f %f %f\n\n", float_group[8], float_group[9], float_group[10], float_group[11]);
		#endif

		unsigned int vertex_count = 0;
		unsigned int vertex_address = 0;
		unsigned int submesh_name_address = 0;
		unsigned int submesh_offset_1 = 0;
		unsigned int submesh_offset_2 = 0;
		unsigned char submesh_definition_count = 0;
		unsigned short submesh_triangles_count = 0;

		file->readInt32E(&vertex_type_flag);
		file->readInt32E(&vertex_size);
		file->readInt32E(&vertex_count);
		file->readInt32E(&vertex_address);
		file->readInt32E(&submesh_name_address);
		file->moveAddress(1);
		file->readUChar(&submesh_definition_count);
		file->readInt16E(&submesh_triangles_count);
		file->readInt32E(&submesh_offset_1);
		file->readInt32E(&submesh_offset_2);

		if (vertex_type_flag == 0x8207) {
		}
		else {
			printf("Unknown Vertex Type Flag %x\n", vertex_type_flag);
			getchar();
		}

		if (vertex_size == 36) {
		}
		else {
			printf("Unknown Vertex Size %d\n", vertex_size);
			getchar();
		}

		#ifdef LIBXENOVERSE_DEBUGGING_LOG
		fprintf(global_debugging_log, "Submesh Triangles Count: %d\n", submesh_triangles_count);
		#endif

		file->goToAddress(base_submesh_address + submesh_name_address);
		file->readString(&name);

		printf("Submesh Name: %s\n", name.c_str());
		printf("Submesh Unknown 1: %d\n", vertex_type_flag);
		printf("Vertex Size: %d\n", vertex_size);
		printf("Vertex Count: %d\n", vertex_count);
		printf("Vertex Address: %d\n", vertex_address);
		printf("Submesh Offset 1: %d\n", submesh_offset_1);
		printf("Submesh Offset 2: %d\n", submesh_offset_2);

		definitions.resize(submesh_definition_count);
		for (size_t i = 0; i < submesh_definition_count; i++) {
			file->goToAddress(base_submesh_address + submesh_offset_1 + i * 12);
			definitions[i].read(file);
		}

		// Read Triangle Lists
		triangles.resize(submesh_triangles_count);
		for (size_t i = 0; i < submesh_triangles_count; i++) {
			file->goToAddress(base_submesh_address + submesh_offset_2 + i*4);

			printf("Reading Triangles address at %d\n", file->getCurrentAddress());
			file->readInt32E(&address);

			if (address) file->goToAddress(base_submesh_address + address);
			else {
				if (submesh_triangles_count > 1) {
					printf("More than 1 submesh triangle list but address is 0? Exception not handled.\n");
					getchar();
				}

				file->goToAddress(base_submesh_address + submesh_offset_2 + 4);
			}

			triangles[i].read(file);
		}

		// Read Vertices
		vertices.resize(vertex_count);
		for (size_t v = 0; v < vertex_count; v++) {
			file->goToAddress(base_submesh_address + vertex_address + v*vertex_size);
			vertices[v].read(file, vertex_type_flag);
		}
	}


	void EMDSubmesh::write(File *file) {
		unsigned int base_submesh_address = file->getCurrentAddress();
		for (size_t x = 0; x < 12; x++) file->writeFloat32E(&float_group[x]);

		file->writeNull(32);

		unsigned int submesh_name_address = file->getCurrentAddress() - base_submesh_address;
		file->writeString(&name);
		file->fixPadding(0x4);

		unsigned int submesh_offset_1 = file->getCurrentAddress() - base_submesh_address;
		for (size_t k = 0; k < definitions.size(); k++) {
			definitions[k].write(file);
		}

		// Write Triangles
		unsigned int submesh_offset_triangle_table = file->getCurrentAddress() - base_submesh_address;
		file->writeNull(triangles.size() * 4);
		for (size_t k = 0; k < triangles.size(); k++) {
			unsigned int triangles_address = file->getCurrentAddress() - base_submesh_address;
			file->goToAddress(base_submesh_address + submesh_offset_triangle_table + k * 4);
			file->writeInt32E(&triangles_address);
			file->goToAddress(base_submesh_address + triangles_address);
			triangles[k].write(file);
		}

		// Write Vertices
		unsigned int vertex_address = file->getCurrentAddress() - base_submesh_address;
		for (size_t v = 0; v < vertices.size(); v++) {
			vertices[v].write(file, vertex_type_flag);
		}

		// Write Header
		file->goToAddress(base_submesh_address + 0x30);
		unsigned int vertex_count = vertices.size();
		unsigned char definition_count = definitions.size();
		unsigned short triangles_count = triangles.size();
		file->writeInt32E(&vertex_type_flag);
		file->writeInt32E(&vertex_size);
		file->writeInt32E(&vertex_count);
		file->writeInt32E(&vertex_address);
		file->writeInt32E(&submesh_name_address);
		file->writeNull(1);
		file->writeUChar(&definition_count);
		file->writeInt16E(&triangles_count);
		file->writeInt32E(&submesh_offset_1);
		file->writeInt32E(&submesh_offset_triangle_table);

		file->goToEnd();
	}

	void EMDSubmesh::setVertexScale(float scale) {
		for (size_t i = 0; i < vertices.size(); i++) {
			vertices[i].x = vertices[i].x * scale;
			vertices[i].y = vertices[i].y * scale;
			vertices[i].z = vertices[i].z * scale;
		}
	}
}