#include "EMMOgre.h"

EMMOgre::EMMOgre() {
	material_resources_created = false;
}

Ogre::Material *EMMOgre::createOgreMaterial(EMMMaterial *emm_material) {
	Ogre::Material *compile_material = Ogre::MaterialManager::getSingleton().create(name + "_" + emm_material->getName(), XENOVIEWER_RESOURCE_GROUP).getPointer();
	if (!compile_material) {
		return NULL;
	}

	Ogre::Pass *pass = compile_material->getTechnique(0)->getPass(0);

	// Force it to use 16 total texture units and full ambient lighting
	for (size_t i = 0; i<16; i++) {
		Ogre::TextureUnitState *texture_unit_state = pass->createTextureUnitState("Blank");
	}
	pass->setAmbient(1.0, 1.0, 1.0);

	pass->getTextureUnitState((size_t)1)->setTextureName(name + "_0");     //   ImageSampler1        s1       1
	pass->getTextureUnitState((size_t)4)->setTextureName(name + ".dyt_0"); //   SamplerToon          s4       1
	pass->getTextureUnitState((size_t)14)->setTextureName(name + ".dyt_1");    //   ImageSamplerTemp14   s14      1

	string shader_name = emm_material->getShaderName();
	string pixel_shader_name = shader_name + "_PS";
	string vertex_shader_name = shader_name + "_W_VS";

	bool pixel_shader_exists = Ogre::GpuProgramManager::getSingleton().resourceExists(pixel_shader_name);
	if (pixel_shader_exists) {
		pass->setFragmentProgram(pixel_shader_name);

		Ogre::GpuProgramParametersSharedPtr fp_parameters = pass->getFragmentProgramParameters();
		fp_parameters->setTransposeMatrices(false);

		Ogre::Vector4 MatCol0 = Ogre::Vector4::ZERO;
		Ogre::Vector4 MatCol1 = Ogre::Vector4::ZERO;
		Ogre::Vector4 MatCol2 = Ogre::Vector4::ZERO;
		Ogre::Vector4 MatCol3 = Ogre::Vector4::ZERO;
		Ogre::Vector4 MatScale0 = Ogre::Vector4::ZERO;
		Ogre::Vector4 MatScale1 = Ogre::Vector4::ZERO;

		vector<EMMParameter *> &parameters = emm_material->getParameters();
		for (size_t i = 0; i < parameters.size(); i++) {
			string parameter_name = parameters[i]->name;
			if (parameter_name == "MatCol0R") MatCol0.x = parameters[i]->valueFloat();
			if (parameter_name == "MatCol0G") MatCol0.y = parameters[i]->valueFloat();
			if (parameter_name == "MatCol0B") MatCol0.z = parameters[i]->valueFloat();
			if (parameter_name == "MatCol0A") MatCol0.w = parameters[i]->valueFloat();

			if (parameter_name == "MatCol1R") MatCol1.x = parameters[i]->valueFloat();
			if (parameter_name == "MatCol1G") MatCol1.y = parameters[i]->valueFloat();
			if (parameter_name == "MatCol1B") MatCol1.z = parameters[i]->valueFloat();
			if (parameter_name == "MatCol1A") MatCol1.w = parameters[i]->valueFloat();

			if (parameter_name == "MatCol2R") MatCol2.x = parameters[i]->valueFloat();
			if (parameter_name == "MatCol2G") MatCol2.y = parameters[i]->valueFloat();
			if (parameter_name == "MatCol2B") MatCol2.z = parameters[i]->valueFloat();
			if (parameter_name == "MatCol2A") MatCol2.w = parameters[i]->valueFloat();

			if (parameter_name == "MatCol3R") MatCol3.x = parameters[i]->valueFloat();
			if (parameter_name == "MatCol3G") MatCol3.y = parameters[i]->valueFloat();
			if (parameter_name == "MatCol3B") MatCol3.z = parameters[i]->valueFloat();
			if (parameter_name == "MatCol3A") MatCol3.w = parameters[i]->valueFloat();

			if (parameter_name == "MatScale0X") MatScale0.x = parameters[i]->valueFloat();
			if (parameter_name == "MatScale0Y") MatScale0.y = parameters[i]->valueFloat();
			if (parameter_name == "MatScale0Z") MatScale0.z = parameters[i]->valueFloat();
			if (parameter_name == "MatScale0W") MatScale0.w = parameters[i]->valueFloat();

			if (parameter_name == "MatScale1X") MatScale1.x = parameters[i]->valueFloat();
			if (parameter_name == "MatScale1Y") MatScale1.y = parameters[i]->valueFloat();
			if (parameter_name == "MatScale1Z") MatScale1.z = parameters[i]->valueFloat();
			if (parameter_name == "MatScale1W") MatScale1.w = parameters[i]->valueFloat();
		}

		// Override Battle Damage
		MatCol3.x = 0.0; // Scratch Mark Multiplier
		MatCol3.y = 0.0; // Blood Mark Multiplier

		fp_parameters->setConstant(51, Ogre::Vector4(0.0));  //   g_vFadeMulti_PS      c51      1
		fp_parameters->setConstant(52, Ogre::Vector4(0.0));  //   g_vFadeRim_PS        c52      1
		fp_parameters->setConstant(53, Ogre::Vector4(0.0));  //   g_vFadeAdd_PS        c53      1
		fp_parameters->setAutoConstant(64, Ogre::GpuProgramParameters::ACT_LIGHT_POSITION, 0);  //   g_vLightVec0_PS      c64      1
		fp_parameters->setConstant(84, MatCol0);
		fp_parameters->setConstant(85, MatCol1);
		fp_parameters->setConstant(86, MatCol2);
		fp_parameters->setConstant(87, MatCol3);
		fp_parameters->setConstant(92, MatScale0);
		fp_parameters->setConstant(93, MatScale1);
		fp_parameters->setConstant(105, Ogre::Vector4(0.0));  //   g_vColor0_PS         c105     1
		fp_parameters->setConstant(106, Ogre::Vector4(0.0));  //   g_vColor1_PS         c106     1
		fp_parameters->setConstant(107, Ogre::Vector4(0.0));  //   g_vColor2_PS         c107     1
		fp_parameters->setConstant(117, Ogre::Vector4(0.0));  //   g_vColor12_PS        c117     1
		fp_parameters->setConstant(139, Ogre::Vector4(1.0));  //   Color Multiplier
		fp_parameters->setConstant(140, Ogre::Vector4(0.0));  //   g_vParam4_PS         c140     1
		fp_parameters->setConstant(141, Ogre::Vector4(0.0));  //   g_vParam5_PS         c141     1
		fp_parameters->setConstant(143, Ogre::Vector4(0.0, 23.2558, 0.04587, 0.0));  // Toon Detail Parameter
	}

	bool vertex_shader_exists = Ogre::GpuProgramManager::getSingleton().resourceExists(vertex_shader_name);
	if (vertex_shader_exists) {
		pass->setVertexProgram(vertex_shader_name);
		Ogre::GpuProgramParametersSharedPtr vp_parameters = pass->getVertexProgramParameters();
		vp_parameters->setTransposeMatrices(false);

		vp_parameters->setAutoConstant((size_t)0, Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
		vp_parameters->setAutoConstant((size_t)72, Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);
		vp_parameters->setAutoConstant((size_t)104, Ogre::GpuProgramParameters::ACT_WORLD_MATRIX_ARRAY_3x4);
	}

	return compile_material;
}

void EMMOgre::createOgreMaterials() {
	if (material_resources_created) {
		return;
	}

	for (size_t i = 0; i < materials.size(); i++) {
		createOgreMaterial(materials[i]);
	}
	material_resources_created = true;
}