namespace LibXenoverse {
	void EMDSubmesh::read(File *file) {
		unsigned int address = 0;
		unsigned int base_submesh_address = file->getCurrentAddress();

		// Read Submesh
		printf("\nReading Submesh at %d\n", base_submesh_address);

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

		LOG_DEBUG("Submesh Floats\n");
		LOG_DEBUG("%f %f %f %f\n", aabb_center_x, aabb_center_y, aabb_center_z, aabb_center_w);
		LOG_DEBUG("%f %f %f %f\n", aabb_min_x, aabb_min_y, aabb_min_z, aabb_min_w);
		LOG_DEBUG("%f %f %f %f\n\n", aabb_max_x, aabb_max_y, aabb_max_z, aabb_max_w);

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

		file->goToAddress(base_submesh_address + submesh_name_address);
		file->readString(&name);

		// There are invalid material names in some meshes which Dimps left broken in the final release
		// Use a placeholder string instead
		if (!name.size()) name = "null";

		LOG_DEBUG("Submesh Triangles Count: %d\n", submesh_triangles_count);
		LOG_DEBUG("Submesh Material Name: %s\n", name.c_str());
		LOG_DEBUG("Submesh Definitions: %d\n", submesh_definition_count);

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

			LOG_DEBUG("Reading triangle list at %d from submesh\n", file->getCurrentAddress());
			triangles[i].read(file);
		}

		AABB vertex_aabb;

		// Read Vertices
		vertices.resize(vertex_count);
		for (size_t v = 0; v < vertex_count; v++) {
			file->goToAddress(base_submesh_address + vertex_address + v*vertex_size);
			vertices[v].read(file, vertex_type_flag);
			vertex_aabb.addPoint(vertices[v].x, vertices[v].y, vertices[v].z);
		}

		LOG_DEBUG("Submesh AABB Coordinates: %f %f %f %f %f %f\n", vertex_aabb.start_x, vertex_aabb.start_y, vertex_aabb.start_z,
																		   vertex_aabb.end_x, vertex_aabb.end_y, vertex_aabb.end_z);
		LOG_DEBUG("Submesh AABB Size: %f %f %f\n", vertex_aabb.sizeX(), vertex_aabb.sizeY(), vertex_aabb.sizeZ());
	}


	void EMDSubmesh::write(File *file) {
		unsigned int base_submesh_address = file->getCurrentAddress();

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