namespace LibXenoverse {
	void EANKeyframe::read(File *file, unsigned char keyframe_size) {
		if (keyframe_size == 1) {
			file->readFloat16E(&x);
			file->readFloat16E(&y);
			file->readFloat16E(&z);
			file->readFloat16E(&w);
		}
		else if (keyframe_size == 2) {
			file->readFloat32E(&x);
			file->readFloat32E(&y);
			file->readFloat32E(&z);
			file->readFloat32E(&w);
		}
		else {
			LOG_DEBUG("Unknown Keyframe Size: %d\n", keyframe_size);
		}

		LOG_DEBUG("    Keyframe of size %d (%d at %d): %f %f %f %f\n", keyframe_size, frame, file->getCurrentAddress() - 16, x, y, z, w);
	}

	void EANKeyframe::readFrame(File *file, unsigned char index_size) {
		if (index_size == 0) {
			unsigned char c = 0;
			file->readUChar(&c);
			frame = c;
		}
		else if (index_size == 1) {
			unsigned short s = 0;
			file->readInt16E(&s);
			frame = s;
		}
		else {
			LOG_DEBUG("Unknown Frame Index Size: %d\n", index_size);
		}
	}

	void EANKeyframe::write(File *file) {

	}
}