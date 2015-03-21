namespace LibXenoverse {
	void EANAnimation::read(File *file) {
		unsigned int base_animation_address = file->getCurrentAddress();

		unsigned int nodes_count = 0;
		unsigned int nodes_offset = 0;
		file->moveAddress(2);
		file->readUChar(&frame_index_size);
		file->readUChar(&frame_float_size);
		file->readInt32E(&frame_count);
		file->readInt32E(&nodes_count);
		file->readInt32E(&nodes_offset);

		LOG_DEBUG("Reading Animation at %d\n", base_animation_address);
		LOG_DEBUG("Animation Data Sizes: %d %d\n", frame_index_size, frame_float_size);
		LOG_DEBUG("Animation Frames: %d\n", frame_count);
		LOG_DEBUG("Animation Nodes (%d):\n", nodes_count);
		nodes.resize(nodes_count);
		for (size_t i = 0; i < nodes_count; i++) {
			file->goToAddress(base_animation_address + nodes_offset + i * 4);
			unsigned int address = 0;
			file->readInt32E(&address);
			file->goToAddress(base_animation_address + address);

			nodes[i].read(file, frame_index_size, frame_float_size);
		}
	}

	void EANAnimation::readName(File *file) {
		file->readString(&name);
		LOG_DEBUG("Animation Name: %s\n", name.c_str());
	}

	void EANAnimation::write(File *file) {

	}
}