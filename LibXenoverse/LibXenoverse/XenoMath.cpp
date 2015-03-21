namespace LibXenoverse {
	float AABB::centerX() {
		return (start_x + end_x) / 2.0f;
	}

	float AABB::centerY() {
		return (start_y + end_y) / 2.0f;
	}

	float AABB::centerZ() {
		return (start_z + end_z) / 2.0f;
	}

	float AABB::sizeX() {
		return end_x - start_x;
	}

	float AABB::sizeY() {
		return end_y - start_y;
	}

	float AABB::sizeZ() {
		return end_z - start_z;
	}

	float AABB::sizeMax() {
		float max_size = sizeX();
		float temp_size = sizeY();
		if (temp_size > max_size) max_size = temp_size;

		temp_size = sizeZ();
		if (temp_size > max_size) max_size = temp_size;

		return max_size;
	}

	float AABB::size() {
		return sizeX() * sizeY() * sizeZ();
	}

	void AABB::merge(const AABB& aabb) {
		if (aabb.start_x < start_x) start_x = aabb.start_x;
		if (aabb.start_y < start_y) start_y = aabb.start_y;
		if (aabb.start_z < start_z) start_z = aabb.start_z;

		if (aabb.end_x > end_x) end_x = aabb.end_x;
		if (aabb.end_y > end_y) end_y = aabb.end_y;
		if (aabb.end_z > end_z) end_z = aabb.end_z;
	}

	float lerp(float a, float b, float f) {
		return a + f * (b - a);
	}
};