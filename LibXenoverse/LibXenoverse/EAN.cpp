namespace LibXenoverse {
	bool EAN::load(string filename) {
		name = nameFromFilenameNoExtension(filename, true);

		File file(filename, LIBXENOVERSE_FILE_READ_BINARY);
		if (file.valid() && file.readHeader(LIBXENOVERSE_EAN_SIGNATURE)) {
			read(&file);
			file.close();
		}
		else return false;

		return true;
	}

	void EAN::read(File *file) {
		file->goToAddress(8);

		unsigned int unknown_total = 0;
		unsigned int skeleton_offset = 0;
		unsigned int animation_keyframes_offset = 0;
		unsigned int animation_names_offset = 0;
		unsigned short animation_count = 0;

		file->readInt32E(&unknown_total);
		file->moveAddress(6);
		file->readInt16E(&animation_count);
		file->readInt32E(&skeleton_offset);
		file->readInt32E(&animation_keyframes_offset);
		file->readInt32E(&animation_names_offset);

		// Read Skeleton
		file->goToAddress(skeleton_offset);
		skeleton = new ESK();
		skeleton->read(file);

		// Read Animations
		animations.resize(animation_count);
		for (size_t i = 0; i < animation_count; i++) {
			// Read Name
			file->goToAddress(animation_names_offset + i*4);
			unsigned int address = 0;
			file->readInt32E(&address);
			file->goToAddress(address);
			animations[i].readName(file);

			// Read
			file->goToAddress(animation_keyframes_offset + i*4);
			address = 0;
			file->readInt32E(&address);
			file->goToAddress(address);

			animations[i].setParent(this);
			animations[i].read(file);
		}
	}

	void EAN::save(string filename, bool big_endian) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.writeHeader(LIBXENOVERSE_EAN_SIGNATURE, big_endian);
			write(&file);
			file.close();
		}
	}

	void EAN::write(File *file) {

	}


#ifdef LIBXENOVERSE_FBX_SUPPORT
	FbxAnimCurveNode *EAN::createFBXAnimationCurveNode(FbxNode *fbx_node, EANAnimation *animation, EANAnimationNode* anim_node, FbxAnimStack *lAnimStack, FbxAnimLayer* lAnimLayer) {


		float fps = 60.0f;			//TODO configure.

		FbxAnimCurveNode *fbx_animCurveNode = fbx_node->GeometricTranslation.GetCurveNode(lAnimLayer, true);
		

		FbxAnimCurve *fbx_animCurve_translation_x = fbx_node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
		FbxAnimCurve *fbx_animCurve_translation_y = fbx_node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
		FbxAnimCurve *fbx_animCurve_translation_z = fbx_node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
		FbxAnimCurve *fbx_animCurve_rotation_x = fbx_node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
		FbxAnimCurve *fbx_animCurve_rotation_y = fbx_node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
		FbxAnimCurve *fbx_animCurve_rotation_z = fbx_node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
		FbxAnimCurve *fbx_animCurve_scale_x = fbx_node->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
		FbxAnimCurve *fbx_animCurve_scale_y = fbx_node->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
		FbxAnimCurve *fbx_animCurve_scale_z = fbx_node->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
		
		
		if (fbx_animCurve_translation_x)
			fbx_animCurve_translation_x->KeyModifyBegin();
		if (fbx_animCurve_translation_y)
			fbx_animCurve_translation_y->KeyModifyBegin();
		if (fbx_animCurve_translation_z)
			fbx_animCurve_translation_z->KeyModifyBegin();
		if (fbx_animCurve_rotation_x)
			fbx_animCurve_rotation_x->KeyModifyBegin();
		if (fbx_animCurve_rotation_y)
			fbx_animCurve_rotation_y->KeyModifyBegin();
		if (fbx_animCurve_rotation_z)
			fbx_animCurve_rotation_z->KeyModifyBegin();
		if (fbx_animCurve_scale_x)
			fbx_animCurve_scale_x->KeyModifyBegin();
		if (fbx_animCurve_scale_y)
			fbx_animCurve_scale_y->KeyModifyBegin();
		if (fbx_animCurve_scale_z)
			fbx_animCurve_scale_z->KeyModifyBegin();

		if ((string(animation->getName()) == "APL_BAS_STAND") || (string(animation->getName()) == "APL_ATC_012") || (string(animation->getName()) == "APL_NAG_D") )
			int aa = 42;


		FbxTime lTime;
		int lKeyIndex = 0;
		size_t frame_count = animation->getFrameCount();
		for (size_t i = 0; i < frame_count; i++) {

			float frame = (float)i / fps;
			
			float px = 0, py = 0, pz = 0, pw = 0;
			float rx = 0, ry = 0, rz = 0, rw = 1.0f;
			float sx = 1.0f, sy = 1.0f, sz = 1.0f, sw = 0;
			anim_node->getInterpolatedFrame(i, LIBXENOVERSE_EAN_KEYFRAMED_ANIMATION_FLAG_POSITION, px, py, pz, pw);
			anim_node->getInterpolatedFrame(i, LIBXENOVERSE_EAN_KEYFRAMED_ANIMATION_FLAG_ROTATION, rx, ry, rz, rw);
			anim_node->getInterpolatedFrame(i, LIBXENOVERSE_EAN_KEYFRAMED_ANIMATION_FLAG_SCALE, sx, sy, sz, sw);


			//fbx part
			lTime.SetSecondDouble(frame);

			
			if (string(fbx_node->GetName()).find("f_") != string::npos) {

				FbxDouble3 bone_position = fbx_node->LclTranslation.Get();
				px -= bone_position[0];
				py -= bone_position[1];
				pz -= bone_position[2];
			}

			//Test
			FbxDouble3 bone_position = fbx_node->LclTranslation.Get();
			px += bone_position[0];
			py += bone_position[1];
			pz += bone_position[2];


			//position
			lKeyIndex = fbx_animCurve_translation_x->KeyAdd(lTime);
			fbx_animCurve_translation_x->KeySetValue(lKeyIndex, px);
			fbx_animCurve_translation_x->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);		//TODO tester FbxAnimCurveDef::eInterpolationCubic

			lKeyIndex = fbx_animCurve_translation_y->KeyAdd(lTime);
			fbx_animCurve_translation_y->KeySetValue(lKeyIndex, py);
			fbx_animCurve_translation_y->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

			lKeyIndex = fbx_animCurve_translation_z->KeyAdd(lTime);
			fbx_animCurve_translation_z->KeySetValue(lKeyIndex, pz);
			fbx_animCurve_translation_z->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);


			//rotation
			FbxQuaternion rotation(rx, ry, rz, rw);


			FbxDouble3 bone_rotation_eulerAngles = fbx_node->LclRotation.Get();
			FbxQuaternion bone_rotation;
			bone_rotation.ComposeSphericalXYZ(bone_rotation_eulerAngles);


			FbxQuaternion bone_rotation_inv(bone_rotation);
			bone_rotation_inv.Inverse();
			rotation = bone_rotation_inv * rotation;

			FbxDouble4 rotation_eulerAngles = rotation.DecomposeSphericalXYZ();
			if (rotation_eulerAngles[0] != rotation_eulerAngles[0])		//check Nan possibility
				rotation_eulerAngles[0] = 0;
			if (rotation_eulerAngles[1] != rotation_eulerAngles[1])		//check Nan possibility
				rotation_eulerAngles[1] = 0;
			if (rotation_eulerAngles[2] != rotation_eulerAngles[2])		//check Nan possibility
				rotation_eulerAngles[2] = 0;


			lKeyIndex = fbx_animCurve_rotation_x->KeyAdd(lTime);
			fbx_animCurve_rotation_x->KeySetValue(lKeyIndex, rotation_eulerAngles[0]);
			fbx_animCurve_rotation_x->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

			lKeyIndex = fbx_animCurve_rotation_y->KeyAdd(lTime);
			fbx_animCurve_rotation_y->KeySetValue(lKeyIndex, rotation_eulerAngles[1]);
			fbx_animCurve_rotation_y->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

			lKeyIndex = fbx_animCurve_rotation_z->KeyAdd(lTime);
			fbx_animCurve_rotation_z->KeySetValue(lKeyIndex, rotation_eulerAngles[2]);
			fbx_animCurve_rotation_z->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);



			//scale
			lKeyIndex = fbx_animCurve_scale_x->KeyAdd(lTime);
			fbx_animCurve_scale_x->KeySetValue(lKeyIndex, sx);
			fbx_animCurve_scale_x->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

			lKeyIndex = fbx_animCurve_scale_y->KeyAdd(lTime);
			fbx_animCurve_scale_y->KeySetValue(lKeyIndex, sy);
			fbx_animCurve_scale_y->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

			lKeyIndex = fbx_animCurve_scale_z->KeyAdd(lTime);
			fbx_animCurve_scale_z->KeySetValue(lKeyIndex, sz);
			fbx_animCurve_scale_z->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);
		}
		
		
		if (fbx_animCurve_translation_x)
		{
			fbx_animCurve_translation_x->KeyModifyEnd();
			fbx_animCurve_translation_x->ConnectSrcObject(fbx_animCurveNode);
		}
		if (fbx_animCurve_translation_y)
		{
			fbx_animCurve_translation_y->KeyModifyEnd();
			fbx_animCurve_translation_y->ConnectSrcObject(fbx_animCurveNode);
		}
		if (fbx_animCurve_translation_z)
		{
			fbx_animCurve_translation_z->KeyModifyEnd();
			fbx_animCurve_translation_z->ConnectSrcObject(fbx_animCurveNode);
		}
		if (fbx_animCurve_rotation_x)
		{
			fbx_animCurve_rotation_x->KeyModifyEnd();
			fbx_animCurve_rotation_x->ConnectSrcObject(fbx_animCurveNode);
		}
		if (fbx_animCurve_rotation_y)
		{
			fbx_animCurve_rotation_y->KeyModifyEnd();
			fbx_animCurve_rotation_y->ConnectSrcObject(fbx_animCurveNode);
		}
		if (fbx_animCurve_rotation_z)
		{
			fbx_animCurve_rotation_z->KeyModifyEnd();
			fbx_animCurve_rotation_z->ConnectSrcObject(fbx_animCurveNode);
		}
		if (fbx_animCurve_scale_x)
		{
			fbx_animCurve_scale_x->KeyModifyEnd();
			fbx_animCurve_scale_x->ConnectSrcObject(fbx_animCurveNode);
		}
		if (fbx_animCurve_scale_y)
		{
			fbx_animCurve_scale_y->KeyModifyEnd();
			fbx_animCurve_scale_y->ConnectSrcObject(fbx_animCurveNode);
		}
		if (fbx_animCurve_scale_z)
		{
			fbx_animCurve_scale_z->KeyModifyEnd();
			fbx_animCurve_scale_z->ConnectSrcObject(fbx_animCurveNode);
		}
		return fbx_animCurveNode;
	}

	
	
	
	vector<FbxAnimCurveNode *> EAN::exportFBXAnimations(FbxScene *scene, std::vector<FbxAnimStack *> list_AnimStack, FbxNode *fbx_node, size_t indexBone) {
		
		vector<FbxAnimCurveNode *> fbx_anims;
		if (fbx_node == NULL)
			return fbx_anims;
		
		EANAnimation *animation;
		int keyframes_count;
		FbxAnimStack* lAnimStack;
		FbxAnimLayer* lAnimLayer;

		size_t nbAnims = animations.size();
		for (size_t i = 0; i < nbAnims; i++) {
			
			if (i >= list_AnimStack.size())
				break;

			animation = &(animations.at(i));
			keyframes_count = animation->getFrameCount();
			vector<EANAnimationNode> &anim_nodes = animation->getNodes();
			
			lAnimStack = list_AnimStack.at(i);
			lAnimLayer = (FbxAnimLayer*)lAnimStack->GetMember(0);


			size_t nbAnimsNodes = anim_nodes.size();
			for (size_t j = 0; j < nbAnimsNodes; j++) {

				if (indexBone == anim_nodes.at(j).getBoneIndex())		//animations and bones are linked by the indexBone
				{
					FbxAnimCurveNode* fbx_anim_curveNode = createFBXAnimationCurveNode(fbx_node, animation, &anim_nodes.at(j), lAnimStack, lAnimLayer);
					fbx_anims.push_back(fbx_anim_curveNode);
				}
			}
		}






		



		/*
		//partie ecriture.
		int i;
		FbxTime lTime;
		FbxAnimCurveKey key;
		FbxAnimCurve* lCurve = NULL;

		// Create one animation stack
		FbxAnimStack* lAnimStack = FbxAnimStack::Create(scene, "Stack001");

		// this stack animation range is limited from 0 to 1 second
		lAnimStack->LocalStop = FBXSDK_TIME_ONE_SECOND;
		lAnimStack->Description = "This is the animation stack description field.";

		// all animation stacks need, at least, one layer.
		FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(scene, "Base Layer");	// the AnimLayer object name is "Base Layer"
		lAnimStack->AddMember(lAnimLayer);											// add the layer to the stack


		
		// Set and get the blend mode bypass of the layer
		bool val;
		lAnimLayer->SetBlendModeBypass(eFbxTypeCount, true);       // set the bypass to all the datatypes.
		val = lAnimLayer->GetBlendModeBypass(eFbxBool);           // val = true
		lAnimLayer->SetBlendModeBypass(eFbxBool, false);          // overwrite just for the bool datatype.
		val = lAnimLayer->GetBlendModeBypass(eFbxBool);           // val = false
		val = lAnimLayer->GetBlendModeBypass(eFbxChar);           // val = true
		val = lAnimLayer->GetBlendModeBypass(eFbxDateTime);        // val = true
		val = lAnimLayer->GetBlendModeBypass((EFbxType)-1);     // invalid type, val = false
		val = lAnimLayer->GetBlendModeBypass((EFbxType)120);    // invalid type (>MAX_TYPES), val = false


		// we want to animate the layer's weight property.
		FbxAnimCurveNode* wcn = lAnimLayer->CreateCurveNode(lAnimLayer->Weight);
		if (wcn)
		{
			// the curve node from the Weight property already contains 1 channel (Weight).
			i = wcn->GetChannelsCount();                            // i = 1


			/*
			// Now, let's add a second channel to the animation node. Note that this code
			// is useless and has only been provided to show the usage of the AddChannel and
			// ResetChannels
			bool ret;
			ret = wcn->AddChannel<int>("MyAddedIntChannel", 99);    // this call will succed
			i = wcn->GetChannelsCount();                            // i = 2
			ret = wcn->AddChannel<int>("MyAddedIntChannel", 10);    // this call will fail, since the channel already exists.
			i = wcn->GetChannelsCount();                            // i = 2
			wcn->ResetChannels();                                   // remove any added channels
			i = wcn->GetChannelsCount();                            // i = 1
		}
		*//*

		// get the Weight curve (and create it if it does not exist, wich is the case!)
		lCurve = lAnimLayer->Weight.GetCurve(lAnimLayer, true);
		if (lCurve)
		{
			// add two keys at time 0 sec and 1 sec with values 0 and 100 respectively.
			lCurve->KeyModifyBegin();
			for (i = 0; i < 2; i++)
			{
				lTime.SetSecondDouble((float)i);
				key.Set(lTime, i*100.0f);
				lCurve->KeyAdd(lTime, key);
			}
			lCurve->KeyModifyEnd();
		}

		//
		// now create a 3 components curvenode and animate two of the three channels.
		//
		// first, we need a "dummy" property so we can call the CreateTypedCurveNode
		FbxProperty p = FbxProperty::Create(scene, FbxDouble3DT, "Vector3Property");
		p.Set(FbxDouble3(1.1, 2.2, 3.3));
		FbxAnimCurveNode* lCurveNode = FbxAnimCurveNode::CreateTypedCurveNode(p, scene);

		// let's make sure the curveNode is added to the animation layer.
		lAnimLayer->AddMember(lCurveNode);

		// and to the "Vector3Property" since CreateTypedCurveNode does not make any connection
		p.ConnectSrcObject(lCurveNode);

		//Example of channel get value:
		//double v1 = lCurveNode->GetChannelValue<double>(0U, 0.0);   // v1 = 1.1
		//float  v2 = lCurveNode->GetChannelValue<float> (1U, 0.0f);  // v2 = 2.2
		//int    v3 = lCurveNode->GetChannelValue<int>   (2U, 0);     // v3 = 3

		//
		// create two free curves (not connected to anything)
		//

		// first curve
		lCurve = FbxAnimCurve::Create(scene, "curve1");
		if (lCurve)
		{
			// add two keys at time 0 sec and 1 sec with values 0 and 10 respectively.
			lCurve->KeyModifyBegin();
			for (i = 0; i < 2; i++)
			{
				lTime.SetSecondDouble((float)i);
				key.Set(lTime, i*10.0f);
				lCurve->KeyAdd(lTime, key);
			}
			lCurve->KeyModifyEnd();
		}

		// connect it to the second channel
		lCurveNode->ConnectToChannel(lCurve, 1);



		// second curve
		lCurve = FbxAnimCurve::Create(scene, "curve2");
		if (lCurve)
		{
			// add three keys at time 1, 2 and 3 sec with values 3.33, 6.66 and 9.99 respectively
			lCurve->KeyModifyBegin();
			for (i = 1; i < 4; i++)
			{
				lTime.SetSecondDouble((float)i);
				key.Set(lTime, i*3.33f);
				lCurve->KeyAdd(lTime, key);
			}
			lCurve->KeyModifyEnd();
		}
		// connect it to the third channel
		lCurveNode->ConnectToChannel(lCurve, "Z"); // for backward compatibility, string identifier are still
		// allowed for the X,Y,Z and W components or "0", "1", ... "9", "A", "B", ... "F" for the Matrix44 datatype

		*/

		/*

		#define FBXSDK_CURVENODE_TRANSFORM		"Transform"
		#define FBXSDK_CURVENODE_TRANSLATION	"T"
		#define FBXSDK_CURVENODE_ROTATION		"R"
		#define FBXSDK_CURVENODE_SCALING		"S"
		#define FBXSDK_CURVENODE_COMPONENT_X	"X"
		#define FBXSDK_CURVENODE_COMPONENT_Y	"Y"
		#define FBXSDK_CURVENODE_COMPONENT_Z	"Z"
		#define FBXSDK_CURVENODE_COLOR			"Color"
		#define FBXSDK_CURVENODE_COLOR_RED		FBXSDK_CURVENODE_COMPONENT_X
		#define FBXSDK_CURVENODE_COLOR_GREEN	FBXSDK_CURVENODE_COMPONENT_Y
		#define FBXSDK_CURVENODE_COLOR_BLUE		FBXSDK_CURVENODE_COMPONENT_Z


		void AnimateSkeleton(FbxScene* pScene, FbxNode* pSkeletonRoot)
		{
			FbxString lAnimStackName;
			FbxTime lTime;
			int lKeyIndex = 0;

			FbxNode* lRoot = pSkeletonRoot;
			FbxNode* lLimbNode1 = pSkeletonRoot->GetChild(0);

			// First animation stack.
			lAnimStackName = "Bend on 2 sides";
			FbxAnimStack* lAnimStack = FbxAnimStack::Create(pScene, lAnimStackName);

			// The animation nodes can only exist on AnimLayers therefore it is mandatory to
			// add at least one AnimLayer to the AnimStack. And for the purpose of this example,
			// one layer is all we need.
			FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(pScene, "Base Layer");
			lAnimStack->AddMember(lAnimLayer);

			// Create the AnimCurve on the Rotation.Z channel
			FbxAnimCurve* lCurve = lRoot->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
			if (lCurve)
			{
				lCurve->KeyModifyBegin();
				lTime.SetSecondDouble(0.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 0.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);

				lTime.SetSecondDouble(1.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 45.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);

				lTime.SetSecondDouble(2.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, -45.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);

				lTime.SetSecondDouble(3.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 0.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
				lCurve->KeyModifyEnd();
			}

			// Same thing for the next object
			lCurve = lLimbNode1->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
			if (lCurve)
			{
				lCurve->KeyModifyBegin();
				lTime.SetSecondDouble(0.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 0.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);

				lTime.SetSecondDouble(1.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, -90.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);

				lTime.SetSecondDouble(2.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 90.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);

				lTime.SetSecondDouble(3.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 0.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
				lCurve->KeyModifyEnd();
			}

			// Second animation stack.
			lAnimStackName = "Bend and turn around";
			lAnimStack = FbxAnimStack::Create(pScene, lAnimStackName);

			// The animation nodes can only exist on AnimLayers therefore it is mandatory to
			// add at least one AnimLayer to the AnimStack. And for the purpose of this example,
			// one layer is all we need.
			lAnimLayer = FbxAnimLayer::Create(pScene, "Base Layer");
			lAnimStack->AddMember(lAnimLayer);

			// Create the AnimCurve on the Rotation.Y channel
			lCurve = lRoot->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
			if (lCurve)
			{
				lCurve->KeyModifyBegin();
				lTime.SetSecondDouble(0.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 0.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);

				lTime.SetSecondDouble(2.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 720.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
				lCurve->KeyModifyEnd();
			}

			lCurve = lLimbNode1->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
			if (lCurve)
			{
				lCurve->KeyModifyBegin();
				lTime.SetSecondDouble(0.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 0.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);

				lTime.SetSecondDouble(1.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 90.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);

				lTime.SetSecondDouble(2.0);
				lKeyIndex = lCurve->KeyAdd(lTime);
				lCurve->KeySetValue(lKeyIndex, 0.0);
				lCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
				lCurve->KeyModifyEnd();
			}
		}
		*/


		return fbx_anims;
	}
#endif
}