namespace LibXenoverse {
	void EMDVertex::read(File *file, unsigned int vertex_type_flag) {
		file->readFloat32E(&x);
		file->readFloat32E(&y);
		file->readFloat32E(&z);

		/*
		x *= 2.0f;
		y *= 2.0f;
		z *= 2.0f;
		*/

		file->readFloat16E(&nx);
		file->readFloat16E(&ny);
		file->readFloat16E(&nz);
		file->moveAddress(2);

		file->readFloat16E(&u);
		file->readFloat16E(&v);

		file->readUChar(&bone[0]);
		file->readUChar(&bone[1]);
		file->readUChar(&bone[2]);
		file->readUChar(&bone[3]);
		file->readFloat16E(&bone_weight[3]);
		file->readFloat16E(&bone_weight[2]);
		file->readFloat16E(&bone_weight[1]);
		file->moveAddress(2);

		bone_weight[0] = 1.0f - bone_weight[3] - bone_weight[2] - bone_weight[1];

		LOG_DEBUG("%d %d %d %d - %f %f %f %f\n", bone[0], bone[1], bone[2], bone[3], bone_weight[0], bone_weight[1], bone_weight[2], bone_weight[3]);
	}


	void EMDVertex::write(File *file, unsigned int vertex_type_flag) {
		file->writeFloat32E(&x);
		file->writeFloat32E(&y);
		file->writeFloat32E(&z);

		file->writeFloat16E(&nx);
		file->writeFloat16E(&ny);
		file->writeFloat16E(&nz);
		file->moveAddress(2);

		file->writeFloat16E(&u);
		file->writeFloat16E(&v);

		file->writeUChar(&bone[0]);
		file->writeUChar(&bone[1]);
		file->writeUChar(&bone[2]);
		file->writeUChar(&bone[3]);
		file->writeFloat16E(&bone_weight[3]);
		file->writeFloat16E(&bone_weight[2]);
		file->writeFloat16E(&bone_weight[1]);
		file->writeNull(2);
	}
}