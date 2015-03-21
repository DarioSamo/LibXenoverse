#ifndef LIBXENOVERSE_MATH_H_INCLUDED
#define LIBXENOVERSE_MATH_H_INCLUDED

#define LIBXENOVERSE_AABB_MAX_START  FLT_MAX 
#define LIBXENOVERSE_AABB_MIN_END    -FLT_MAX

namespace LibXenoverse {
	class AABB {
	public:
		float start_x;
		float start_y;
		float start_z;
		float end_x;
		float end_y;
		float end_z;

		AABB() {
			reset();
		}

		void reset() {
			start_x = LIBXENOVERSE_AABB_MAX_START;
			start_y = LIBXENOVERSE_AABB_MAX_START;
			start_z = LIBXENOVERSE_AABB_MAX_START;

			end_x = LIBXENOVERSE_AABB_MIN_END;
			end_y = LIBXENOVERSE_AABB_MIN_END;
			end_z = LIBXENOVERSE_AABB_MIN_END;
		}

		void addPoint(float point_x, float point_y, float point_z) {
			if (point_x < start_x) start_x = point_x;
			if (point_y < start_y) start_y = point_y;
			if (point_z < start_z) start_z = point_z;

			if (point_x > end_x) end_x = point_x;
			if (point_y > end_y) end_y = point_y;
			if (point_z > end_z) end_z = point_z;
		}

		AABB half(int axis, int side);

		float centerX();
		float centerY();
		float centerZ();
		float sizeX();
		float sizeY();
		float sizeZ();
		float sizeMax();
		float size();

		inline AABB& operator = (const AABB& aabb) {
			start_x = aabb.start_x;
			start_y = aabb.start_y;
			start_z = aabb.start_z;
			end_x = aabb.end_x;
			end_y = aabb.end_y;
			end_z = aabb.end_z;

			return *this;
		}

		bool intersects(const AABB& aabb);
		void merge(const AABB& aabb);

		void expandX(float v) {
			start_x -= v;
			end_x += v;
		}

		void expandY(float v) {
			start_y -= v;
			end_y += v;
		}

		void expandZ(float v) {
			start_z -= v;
			end_z += v;
		}

		void expand(float v) {
			start_x -= v;
			start_y -= v;
			start_z -= v;
			end_x += v;
			end_y += v;
			end_z += v;
		}
	};

	float lerp(float a, float b, float f);
};

#endif