namespace LibXenoverse {
	void EANAnimationNode::read(File *file, unsigned char index_size, unsigned char keyframe_size) {
		unsigned int base_animation_node_address = file->getCurrentAddress();

		unsigned short keyframed_animation_count = 0;
		unsigned int keyframed_animation_offset = 0;
		file->readInt16E(&bone_index);
		file->readInt16E(&keyframed_animation_count);
		file->readInt32E(&keyframed_animation_offset);

		LOG_DEBUG("Reading Animation Node at %d\n", base_animation_node_address);
		LOG_DEBUG("Animation Node Keyframed Animations (%d) for node %d:\n", keyframed_animation_count, bone_index);
		keyframed_animations.resize(keyframed_animation_count);
		for (size_t i = 0; i < keyframed_animation_count; i++) {
			file->goToAddress(base_animation_node_address + keyframed_animation_offset + i * 4);
			unsigned int address = 0;
			file->readInt32E(&address);
			file->goToAddress(base_animation_node_address + address);

			keyframed_animations[i].read(file, index_size, keyframe_size);
		}
	}

	void EANAnimationNode::write(File *file) {

	}


	void EANAnimationNode::getInterpolatedFrame(unsigned int frame, unsigned int flag, float &x, float &y, float &z, float &w) {
		for (size_t i = 0; i < keyframed_animations.size(); i++) {
			if (keyframed_animations[i].getFlag() == flag) {
				keyframed_animations[i].getInterpolatedFrame(frame, x, y, z, w);
				return;
			}
		}
	}
}