#include "EMDOgre.h"
#include "EMMOgre.h"
#include "EMBOgre.h"
#include "ESKOgre.h"
#include "EANOgre.h"
#include "ViewerGrid.h"
#include "OgreWidget.h"

namespace QtOgre
{
	void OgreWidget::createScene(void) {
		current_animation_index = 0;
		skeleton_debug = NULL;
		current_animation_state = NULL;
		animation = NULL;
		entity = NULL;
		enable_spinning = false;
		enable_panning = false;
		spin_x = 0.0f;
		spin_y = 0.0f;

		resetCamera();
		ViewerGrid *viewer_grid = new ViewerGrid();
		viewer_grid->createSceneNode(mSceneMgr);

		LibXenoverse::initializeDebuggingLog();
		// Create a blank texture
		Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual("Blank", XENOVIEWER_RESOURCE_GROUP, Ogre::TEX_TYPE_2D, 32, 32, 0, Ogre::PF_BYTE_BGRA, Ogre::TU_DEFAULT);
		Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
		pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
		const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
		Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);

		for (size_t j = 0; j < 32; j++) {
			for (size_t i = 0; i < 32; i++) {
				*pDest++ = 0;
				*pDest++ = 0;
				*pDest++ = 0;
				*pDest++ = 255;
			}

			pDest += pixelBox.getRowSkip() * Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
		}
		pixelBuffer->unlock();

		// Create listener for EMDs
		emd_render_object_listener = new EMDRenderObjectListener();
		mSceneMgr->addRenderObjectListener(emd_render_object_listener);

		// Create Lighting
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
		Ogre::Light *direct_light = mSceneMgr->createLight("Xenoviewer Direct Light");
		direct_light->setSpecularColour(Ogre::ColourValue::White);
		direct_light->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));
		direct_light->setType(Ogre::Light::LT_DIRECTIONAL);
		direct_light->setDirection(Ogre::Vector3(1, -1, 1).normalisedCopy());

		// Load Shaders
		vector<string> shader_names;
		shader_names.push_back("adam_shader/shader_age_ps.emb");
		shader_names.push_back("adam_shader/shader_age_vs.emb");
		shader_names.push_back("adam_shader/shader_default_ps.emb");
		shader_names.push_back("adam_shader/shader_default_vs.emb");
		for (size_t i = 0; i < shader_names.size(); i++) {
			EMBOgre *shader_pack = new EMBOgre();
			if (shader_pack->load(shader_names[i])) {
				shader_pack->createOgreShaders();
			}
			delete shader_pack;
		}

		// Load Character Models/Skeletons/Animations/Materials/Textures
		string folder = "";
		string character_name = "GOK";
		string character_index = "000";
		string character_prefix = folder + character_name + "/" + character_name + "_" + character_index;
		string skeleton_filename = character_prefix + ".esk";
		string animation_filename = "GOK/GOK.ean";

		ESKOgre *skeleton = NULL;
		skeleton = new ESKOgre();
		if (skeleton->load(skeleton_filename)) {
			skeleton->createOgreSkeleton();
		}
		else {
			delete skeleton;
			skeleton = NULL;
		}


		animation = new EANOgre();
		if (animation->load(animation_filename)) {
			animation->setSkeleton(skeleton);
			animation->createOgreAnimations();
		}
		else {
			SHOW_SMSG("Couldn't load " + animation_filename + " for animation.");
			delete animation;
			animation = NULL;
		}

		vector<string> model_names;
		model_names.push_back(character_prefix + "_Bust");
		model_names.push_back(character_prefix + "_Boots");
		model_names.push_back(character_prefix + "_Face_base");
		model_names.push_back(character_prefix + "_Face_eye");
		model_names.push_back(character_prefix + "_Face_forehead");
		model_names.push_back(character_prefix + "_Pants");
		model_names.push_back(character_prefix + "_Rist");

		for (size_t i = 0; i < model_names.size(); i++) {
			string emb_filename = model_names[i] + ".emb";
			string emb_dyt_filename = model_names[i] + ".dyt.emb";

			EMBOgre *texture_pack = new EMBOgre();
			if (texture_pack->load(emb_filename)) {
				texture_pack->createOgreTextures();
			}
			else {
				delete texture_pack;
				texture_pack = NULL;
			}

			EMBOgre *texture_dyt_pack = new EMBOgre();
			if (texture_dyt_pack->load(emb_dyt_filename)) {
				texture_dyt_pack->createOgreTextures();
			}
			else {
				delete texture_dyt_pack;
				texture_dyt_pack = NULL;
			}

			EMMOgre *material = new EMMOgre();
			if (material->load(model_names[i] + ".emm")) {
				material->createOgreMaterials();
			}
			else {
				delete material;
				material = NULL;
			}

			EMDOgre *model = new EMDOgre();
			if (model->load(model_names[i] + ".emd")) {
				if (skeleton) {
					model->setSkeleton(skeleton);
				}

				Ogre::SceneNode *emd_root_node = model->createOgreSceneNode(mSceneMgr, texture_pack, texture_dyt_pack);
			}
			else {
				delete model;
				model = NULL;
			}

			delete model;
			delete texture_pack;
			delete texture_dyt_pack;
			delete material;
		}

		// Enable an animation if the package exists
		entity = skeleton->getSharedEntity();
		if (entity && animation) {
			switchCurrentAnimation();
		}
	}

	void OgreWidget::switchCurrentAnimation() {
		vector<EANAnimation> &animations = animation->getAnimations();
		if (animations.size()) {
			string animation_name = animations[current_animation_index].getName();
			if (entity->hasAnimationState(animation_name)) {
				if (current_animation_state) {
					current_animation_state->setEnabled(false);
					current_animation_state = NULL;
				}

				current_animation_state = entity->getAnimationState(animation_name);
				current_animation_state->setLoop(true);
				current_animation_state->setEnabled(true);
			}
		}
	}

	bool OgreWidget::frameRenderingQueued(const Ogre::FrameEvent& evt) {
		if (current_animation_state) {
			current_animation_state->addTime(evt.timeSinceLastFrame);
		}

		if (skeleton_debug) {
			skeleton_debug->update();
		}

		repositionCamera();
		return true;
	}

	void OgreWidget::mousePressEvent(QMouseEvent * event) {
		if (event->button() == Qt::LeftButton) {
			enable_spinning = true;
		}

		if (event->button() == Qt::RightButton) {
			enable_panning = true;
		}
	}

	void OgreWidget::mouseMoveEvent(QMouseEvent * event) {
		int mouse_x = event->x();
		int mouse_y = event->y();
		int delta_x = mouse_x - last_mouse_x;
		int delta_y = mouse_y - last_mouse_y;
		float delta_f_x = (float)delta_x / (float)width();
		float delta_f_y = (float)delta_y / (float)height();

		if (enable_spinning) {
			spinCamera(delta_f_x, delta_f_y);
		}

		if (enable_panning) {
			panCamera(delta_f_x, delta_f_y);
		}

		last_mouse_x = mouse_x;
		last_mouse_y = mouse_y;
	}

	void OgreWidget::keyPressEvent(QKeyEvent * event) {
		if (event->key() == Qt::Key_Left) {
			current_animation_index--;
			if (current_animation_index < 0) {
				current_animation_index = animation->getAnimations().size() - 1;
			}
			switchCurrentAnimation();
		}
		if (event->key() == Qt::Key_Right) {
			current_animation_index++;
			if (current_animation_index >= animation->getAnimations().size()) {
				current_animation_index = 0;
			}
			switchCurrentAnimation();
		}
	}

	void OgreWidget::mouseReleaseEvent(QMouseEvent * event) {
		if (event->button() == Qt::LeftButton) {
			enable_spinning = false;
		}

		if (event->button() == Qt::RightButton) {
			enable_panning = false;
		}
	}

	void OgreWidget::wheelEvent(QWheelEvent * event) {
		zoomCamera(event->delta());
	}

	void OgreWidget::spinCamera(float delta_x, float delta_y) {
		viewer_angle_x += delta_x * -4.0f;
		viewer_angle_y += delta_y * -4.0f;

		if (viewer_angle_x >= Ogre::Math::TWO_PI) viewer_angle_x -= Ogre::Math::TWO_PI;
		if (viewer_angle_x < 0) viewer_angle_x += Ogre::Math::TWO_PI;

		if (viewer_angle_y >= Ogre::Math::HALF_PI - 0.1) viewer_angle_y = Ogre::Math::HALF_PI - 0.1;
		if (viewer_angle_y < -Ogre::Math::HALF_PI + 0.1) viewer_angle_y = -Ogre::Math::HALF_PI + 0.1;
	}

	void OgreWidget::panCamera(float delta_x, float delta_y) {
		viewer_center += mCamera->getOrientation() * Ogre::Vector3(delta_x * -2.0f, -delta_y * -2.0f, 0.0);
	}

	void OgreWidget::zoomCamera(float delta) {
		zoom += delta * -0.001f;

		if (zoom < 0.01f) zoom = 0.01f;
	}

	void OgreWidget::repositionCamera() {
		Ogre::Quaternion rotation_x;
		Ogre::Quaternion rotation_y;
		rotation_x.FromAngleAxis(Ogre::Radian(viewer_angle_x), Ogre::Vector3::UNIT_Y);
		rotation_y.FromAngleAxis(Ogre::Radian(viewer_angle_y), Ogre::Vector3::UNIT_X);

		Ogre::Vector3 new_position = viewer_center + ((rotation_x * rotation_y) * Ogre::Vector3(0, 0, 3.0 * zoom));
		mCamera->setPosition(new_position);
		mCamera->lookAt(viewer_center);
	}

	void OgreWidget::resetCamera() {
		viewer_center = Ogre::Vector3(0, 0.5, 0);
		viewer_angle_x = Ogre::Math::PI;
		viewer_angle_y = 0.0f;
		zoom = 1.0f;
	}
}
