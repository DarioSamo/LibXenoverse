#ifndef LIBXENOVERSE_EAN_H_INCLUDED
#define LIBXENOVERSE_EAN_H_INCLUDED

#define LIBXENOVERSE_EAN_SIGNATURE    "#EAN"

#define LIBXENOVERSE_EAN_KEYFRAMED_ANIMATION_FLAG_POSITION  1792
#define LIBXENOVERSE_EAN_KEYFRAMED_ANIMATION_FLAG_ROTATION  1793
#define LIBXENOVERSE_EAN_KEYFRAMED_ANIMATION_FLAG_SCALE     1794

namespace LibXenoverse {
	class EANBone {
		protected:
			unsigned short parent_index;
			unsigned short child_index;
			unsigned short sibling_index;
			unsigned short index_4;

			string name;
			float matrix[12];

			unsigned int flag_start;
			unsigned int flag_end;
		public:
			EANBone() {
			}

			void readName(File *file);
			void readMatrix(File *file);
			void readIndices(File *file);
			void readFlags(File *file);

			void write(File *file);

			string getName() {
				return name;
			}
	};

	class EANSkeleton {
	protected:
		vector<EANBone> bones;
	public:
		EANSkeleton() {
		}

		void read(File *file);
		void write(File *file);

		vector<EANBone> &getBones() {
			return bones;
		}
	};

	class EANKeyframe {
	protected:
		float x;
		float y;
		float z;
		float w;
		unsigned int frame;
	public:
		EANKeyframe() {
		}

		void read(File *file);
		void readFrame(File *file, unsigned short animation_flag);
		void write(File *file);

		unsigned int &getFrame() {
			return frame;
		}

		float &getX() {
			return x;
		}

		float &getY() {
			return y;
		}

		float &getZ() {
			return z;
		}

		float &getW() {
			return w;
		}
	};

	class EANKeyframedAnimation {
	protected:
		vector<EANKeyframe> keyframes;
		unsigned int flag;
	public:
		EANKeyframedAnimation() {
		}

		void read(File *file, unsigned short animation_flag);
		void write(File *file);

		void getInterpolatedFrame(unsigned int frame, float &x, float &y, float &z, float &w);
		void getFrame(EANKeyframe *keyframe, float &x, float &y, float &z, float &w);

		unsigned int getFlag() {
			return flag;
		}
	};

	class EANAnimationNode {
	protected:
		vector<EANKeyframedAnimation> keyframed_animations;
		unsigned short bone_index;
	public:
		EANAnimationNode() {
		}

		void read(File *file, unsigned short animation_flag);
		void write(File *file);

		unsigned int getBoneIndex() {
			return bone_index;
		}

		void getInterpolatedFrame(unsigned int frame, unsigned int flag, float &x, float &y, float &z, float &w);
	};

	class EANAnimation {
	protected:
		string name;
		vector<EANAnimationNode> nodes;
		unsigned short flag;
		unsigned int frame_count;
	public:
		EANAnimation() {
			name = "";
		}

		void read(File *file);
		void readName(File *file);
		void write(File *file);

		unsigned int getFrameCount() {
			return frame_count;
		}

		string getName() {
			return name;
		}

		vector<EANAnimationNode> &getNodes() {
			return nodes;
		}
	};

	class EAN {
	protected:
		string name;
		vector<EANAnimation> animations;
		EANSkeleton *skeleton;
	public:
		EAN() {
			name = "";
			skeleton = NULL;
		}

		bool load(string filename);
		void save(string filename, bool big_endian = false);

		void read(File *file);
		void write(File *file);

		EANSkeleton *getSkeleton() {
			return skeleton;
		}

		vector<EANAnimation> &getAnimations() {
			return animations;
		}
	};
}

#endif