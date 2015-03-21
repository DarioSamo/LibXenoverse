namespace LibXenoverse {
	void EANAnimation::read(File *file) {
		unsigned int base_animation_address = file->getCurrentAddress();

		unsigned short unknown_flag = 0;
		unsigned int nodes_count = 0;
		unsigned int nodes_offset = 0;
		file->readInt16E(&unknown_flag);
		file->readInt16E(&flag);
		file->readInt32E(&frame_count);
		file->readInt32E(&nodes_count);
		file->readInt32E(&nodes_offset);

		LOG_DEBUG("Reading Animation at %d\n", base_animation_address);
		LOG_DEBUG("Animation Flags: %d %d\n", unknown_flag, flag);
		LOG_DEBUG("Animation Frames: %d\n", frame_count);
		LOG_DEBUG("Animation Nodes (%d):\n", nodes_count);
		nodes.resize(nodes_count);
		for (size_t i = 0; i < nodes_count; i++) {
			file->goToAddress(base_animation_address + nodes_offset + i * 4);
			unsigned int address = 0;
			file->readInt32E(&address);
			file->goToAddress(base_animation_address + address);

			nodes[i].read(file, flag);
		}
	}

	void EANAnimation::readName(File *file) {
		file->readString(&name);
		LOG_DEBUG("Animation Name: %s\n", name.c_str());
	}

	void EANAnimation::write(File *file) {

	}
}