#include "XenoviewerApplication.h"
#include "EMDOgre.h"
#include "EMMOgre.h"
#include "EMBOgre.h"
#include "ESKOgre.h"
#include "EANOgre.h"


//---------------------------------------------------------------------------
XenoviewerApplication::XenoviewerApplication(void)
{
}
//---------------------------------------------------------------------------
XenoviewerApplication::~XenoviewerApplication(void)
{
}

//---------------------------------------------------------------------------
void XenoviewerApplication::createScene(void)
{
	LibXenoverse::initializeDebuggingLog();

	emd_render_object_listener = new EMDRenderObjectListener();
	mSceneMgr->addRenderObjectListener(emd_render_object_listener);

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));

	Ogre::Light *direct_light = mSceneMgr->createLight("Xenoviewer Direct Light");
	direct_light->setSpecularColour(Ogre::ColourValue::White);
	direct_light->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));
	direct_light->setType(Ogre::Light::LT_DIRECTIONAL);
	direct_light->setDirection(Ogre::Vector3(1, -1, 1).normalisedCopy());

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
	
	string folder = "../data/data/chara/";
	string character_name = "GOK";
	string character_index = "000";
	string character_prefix = folder + character_name + "/" + character_name + "_" + character_index;
	string skeleton_filename = character_prefix + ".esk";
	string animation_filename = "000_GOK_KMH.ean";

	ESKOgre *skeleton = NULL; 
	skeleton = new ESKOgre();
	if (skeleton->load(skeleton_filename)) {
		skeleton->createOgreSkeleton();
	}
	else {
		delete skeleton;
		skeleton = NULL;
	}

	
	EANOgre *animation = NULL;
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

	current_animation_state = NULL;

	Ogre::Entity *entity = skeleton->getSharedEntity();
	if (entity && animation) {
		/*
		skeleton_debug = new SkeletonDebug(entity, mSceneMgr, mCamera, 0.01);
		skeleton_debug->showBones(true);
		skeleton_debug->showNames(true);
		skeleton_debug->showAxes(true);
		*/

		vector<EANAnimation> &animations = animation->getAnimations();

		if (animations.size()) {
			string animation_name = animations[0].getName();
			if (entity->hasAnimationState(animation_name)) {
				current_animation_state = entity->getAnimationState(animation_name);
				current_animation_state->setLoop(true);
				current_animation_state->setEnabled(true);
			}
		}
	}
}


bool XenoviewerApplication::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	if (!BaseApplication::frameRenderingQueued(evt)) {
		return false;
	}

	if (current_animation_state) {
		current_animation_state->addTime(evt.timeSinceLastFrame);
	}

	if (skeleton_debug) {
		skeleton_debug->update();
	}

	return true;
}


//---------------------------------------------------------------------------

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        XenoviewerApplication app;

        try {
            app.go();
        } catch(Ogre::Exception& e)  {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occurred: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
