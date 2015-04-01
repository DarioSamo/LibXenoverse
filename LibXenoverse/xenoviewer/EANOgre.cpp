#include "EANOgre.h"

EANOgre::EANOgre() {
	skeleton = NULL;
	fps = 60.0f;
}

Ogre::NodeAnimationTrack *EANOgre::createOgreAnimationTrack(Ogre::Animation *mAnim, Ogre::Bone *mBone, EANAnimation *animation, EANAnimationNode *node) {
	Ogre::NodeAnimationTrack *node_track = mAnim->createNodeTrack(mBone->getHandle(), mBone);
	unsigned int frame_count = animation->getFrameCount();

	for (size_t i = 0; i < frame_count; i++) {
		float frame = (float)i / fps;
		Ogre::TransformKeyFrame* mKey = node_track->createNodeKeyFrame(frame);
		float px = 0, py = 0, pz = 0, pw = 0;
		float rx = 0, ry = 0, rz = 0, rw = 1.0f;
		float sx = 1.0f, sy = 1.0f, sz = 1.0f, sw = 0;
		node->getInterpolatedFrame(i, LIBXENOVERSE_EAN_KEYFRAMED_ANIMATION_FLAG_POSITION, px, py, pz, pw);
		node->getInterpolatedFrame(i, LIBXENOVERSE_EAN_KEYFRAMED_ANIMATION_FLAG_ROTATION, rx, ry, rz, rw);
		node->getInterpolatedFrame(i, LIBXENOVERSE_EAN_KEYFRAMED_ANIMATION_FLAG_SCALE,    sx, sy, sz, sw);

		Ogre::Vector3 bone_position = mBone->getPosition();
		Ogre::Quaternion bone_rotation = mBone->getOrientation();

		Ogre::Vector3 translate(px, py, pz);
		Ogre::Quaternion rotation(rw, rx, ry, rz);
		Ogre::Vector3 scale(sx, sy, sz);

		//LOG_DEBUG("Bone %s (%d) Position: %f %f %f vs %f %f %f\n", mBone->getName().c_str(), i, px, py, pz, (float)bone_position.x, (float)bone_position.y, (float)bone_position.z);
		//LOG_DEBUG("Bone %s (%d) Rotation: %f %f %f %f vs %f %f %f %f\n", mBone->getName().c_str(), i, rx, ry, rz, rw, (float)bone_rotation.x, (float)bone_rotation.y, (float)bone_rotation.z, (float)bone_rotation.w);
		//LOG_DEBUG("Bone %s (%d) Scale: %f %f %f\n", mBone->getName().c_str(), i, sx, sy, sz);
		
		/*
		if (skeleton->getFlag() == 1) {
			rotation = bone_rotation.Inverse() * rotation;
		}
		else if (skeleton->getFlag() == 0xFFFF) {
			translate -= bone_position;
			rotation = bone_rotation.Inverse() * rotation;
		}
		*/

		if (mBone->getName().find("f_") == string::npos) {
			rotation = bone_rotation.Inverse() * rotation;
		}
		else {
			translate -= bone_position;
			rotation = bone_rotation.Inverse() * rotation;
		}

		mKey->setTranslate(translate);
		mKey->setRotation(rotation);
		mKey->setScale(scale);
	}
	return node_track;
}

Ogre::Animation *EANOgre::createOgreAnimation(EANAnimation *animation) {
	int keyframes_count = animation->getFrameCount();
	vector<Ogre::NodeAnimationTrack *> node_tracks;

	Ogre::Skeleton *ogre_skeleton = esk_skeleton->getOgreSkeleton();
	if (!ogre_skeleton) {
		return NULL;
	}

	if (ogre_skeleton->hasAnimation(animation->getName())) {
		return NULL;
	}

	Ogre::Animation* mAnim = ogre_skeleton->createAnimation(animation->getName(), (float)keyframes_count / fps);
	vector<EANAnimationNode> &nodes = animation->getNodes();
	vector<ESKBone *> &bones = skeleton->getBones();

	// Search for matching bones with animation nodes
	for (size_t i = 0; i < nodes.size(); i++) {
		unsigned int bone_index = nodes[i].getBoneIndex();
		if (bone_index < bones.size()) {
			string bone_name = bones[bone_index]->getName();
			if (ogre_skeleton->hasBone(bone_name)) {
				Ogre::Bone *mBone = ogre_skeleton->getBone(bone_name);
				Ogre::NodeAnimationTrack *node_track = createOgreAnimationTrack(mAnim, mBone, animation, &nodes[i]);
			}
		}
	}

	return mAnim;
}

void EANOgre::createOgreAnimations(ESKOgre *v) {
	esk_skeleton = v;

	for (size_t i = 0; i < animations.size(); i++) {
		createOgreAnimation(&animations[i]);
	}
}