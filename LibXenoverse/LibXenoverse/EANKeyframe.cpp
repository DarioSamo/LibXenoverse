namespace LibXenoverse {
	void EANKeyframe::read(File *file) {
		file->readFloat32E(&x);
		file->readFloat32E(&y);
		file->readFloat32E(&z);
		file->readFloat32E(&w);

		LOG_DEBUG("    Keyframe(%d): %f %f %f %f\n", frame, x, y, z, w);
	}

	void EANKeyframe::readFrame(File *file, unsigned short animation_flag) {
		if (animation_flag == 257) {
			unsigned short s = 0;
			file->readInt16E(&s);
			frame = s;
		}
		else if (animation_flag == 512) {
			unsigned char c=0;
			file->readUChar(&c);
			frame = c;
		}
		else {
			printf("Unknown Animation Flag: %d\n", animation_flag);
			getchar();
		}
	}

	void EANKeyframe::write(File *file) {

	}
}