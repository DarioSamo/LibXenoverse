namespace LibXenoverse {
	void EANKeyframedAnimation::read(File *file, unsigned char index_size, unsigned char keyframe_size) {
		unsigned int base_keyframed_animation_address = file->getCurrentAddress();
		unsigned int keyframes_count = 0;
		unsigned int indices_offset = 0;
		unsigned int keyframes_offset = 0;

		file->readInt32E(&flag);
		file->readInt32E(&keyframes_count);
		file->readInt32E(&indices_offset);
		file->readInt32E(&keyframes_offset);

		keyframes.resize(keyframes_count);
		LOG_DEBUG("Reading Keyframed Animation at %d\n", base_keyframed_animation_address);
		LOG_DEBUG("  Keyframed Animation Flag: %d\n", flag);

		file->goToAddress(base_keyframed_animation_address + indices_offset);
		for (size_t i = 0; i < keyframes_count; i++) {
			keyframes[i].readFrame(file, index_size);
		}

		LOG_DEBUG("  Keyframes (%d):\n", keyframes_count);
		file->goToAddress(base_keyframed_animation_address + keyframes_offset);
		for (size_t i = 0; i < keyframes_count; i++) {
			keyframes[i].read(file, keyframe_size);
		}
		LOG_DEBUG("\n");
	}

	void EANKeyframedAnimation::write(File *file) {

	}

	void EANKeyframedAnimation::getFrame(EANKeyframe *keyframe, float &x, float &y, float &z, float &w) {
		x = keyframe->getX();
		y = keyframe->getY();
		z = keyframe->getZ();
		w = keyframe->getW();
	}

	void EANKeyframedAnimation::getInterpolatedFrame(unsigned int frame, float &x, float &y, float &z, float &w) {
		EANKeyframe *lower_keyframe = NULL;
		EANKeyframe *upper_keyframe = NULL;

		for (size_t i = 0; i < keyframes.size(); i++) {
			unsigned int key_frame = keyframes[i].getFrame();
			if ((key_frame <= frame) && (!lower_keyframe || (lower_keyframe->getFrame() <= key_frame))) {
				lower_keyframe = &keyframes[i];
			}

			if ((key_frame >= frame) && (!upper_keyframe || (upper_keyframe->getFrame() >= key_frame))) {
				upper_keyframe = &keyframes[i];
			}
		}

		// Interpolate XYZW with the frames
		if (lower_keyframe == upper_keyframe) getFrame(lower_keyframe, x, y, z, w);
		else if (lower_keyframe && upper_keyframe) {
			float lower_frame = (float) lower_keyframe->getFrame();
			float upper_frame = (float) upper_keyframe->getFrame();
			float factor = ((float)frame - lower_frame) / (upper_frame - lower_frame);

			x = lerp(lower_keyframe->getX(), upper_keyframe->getX(), factor);
			y = lerp(lower_keyframe->getY(), upper_keyframe->getY(), factor);
			z = lerp(lower_keyframe->getZ(), upper_keyframe->getZ(), factor);
			w = lerp(lower_keyframe->getW(), upper_keyframe->getW(), factor);
		}
		else if (lower_keyframe) getFrame(lower_keyframe, x, y, z, w);
		else if (upper_keyframe) getFrame(upper_keyframe, x, y, z, w);
	}
}