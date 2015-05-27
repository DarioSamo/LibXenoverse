namespace LibXenoverse {
	
#ifdef LIBXENOVERSE_FBX_SUPPORT
	
	void EMB::exportShadersToFx(EMB *shader_pack_vs, EMB *shader_pack_ps) {
		
		vector<EMBFile *> vs_files = shader_pack_vs->getFiles();
		vector<EMBFile *> ps_files = shader_pack_ps->getFiles();

		for (size_t i = 0; i < vs_files.size(); i++)
		{
			EMBFile *vs_file = vs_files.at(i);

			string shader_name_vs = nameFromFilenameNoExtension(vs_file->getName());
			string extension_vs = extensionFromFilename(vs_file->getName());
			
			if ((extension_vs != "xvu") && (extension_vs != "xpu"))			//if not a shader, ignore
				continue;
			
			string test = shader_name_vs.substr(shader_name_vs.size() - 5);
			
			string shader_name = shader_name_vs;
			if (shader_name_vs.substr(shader_name_vs.size() - 5) == "_W_VS")			//filter like EMMOGre. Todo see why
				shader_name = shader_name_vs.substr(0, shader_name_vs.size() - 5);
			else if(shader_name_vs.substr(shader_name_vs.size() - 3) == "_PS")
				shader_name = shader_name_vs.substr(0, shader_name_vs.size() - 3);
			else
				continue;

			EMBFile *ps_file = NULL;
			for (size_t j = 0; j < ps_files.size(); j++)					//search  for a couple
			{
				string shader_name_ps = nameFromFilenameNoExtension(ps_files.at(j)->getName());				
				if ((shader_name_ps.substr(shader_name_ps.size() - 5) != "_W_VS") && (shader_name_ps.substr(shader_name_ps.size() - 3) != "_PS"))	//filter like EMMOGre. Todo see why
					continue;

				if ((shader_name_ps.size() >= shader_name.size() + 3) && (shader_name_ps.substr(0, shader_name.size()) == shader_name))
				{
					ps_file = ps_files.at(j);
					break;
				}
			}
			
			if (ps_file == NULL)			//if we not found a couple vs - ps, we do nothing
				continue;


			
			string extension_ps = extensionFromFilename(ps_file->getName());
			if ((extension_ps != "xvu") && (extension_ps != "xpu"))
				continue;

			if ((extension_vs != "xvu") || (extension_ps != "xpu"))			//resolve inversion or the same definition
			{
				if ((extension_vs == "xpu") && (extension_ps == "xvu"))		//inversion
				{
					EMBFile *file_tmp = vs_file;
					vs_file = ps_file;
					ps_file = file_tmp;
					extension_vs = "xvu";
					extension_ps = "xpu";
				} else {													//others
					continue;
				}
			}



			size_t size = vs_file->getSize();
			char *new_data_vs = HLSLASM::disassemble(vs_file->getData(), size);
			size = ps_file->getSize();
			char *new_data_ps = HLSLASM::disassemble(ps_file->getData(), size);


			// Fx declaration, with some parameters available to change in 3dsmax's DirectX material interface,
			// with definitions like Ogre for material/shader program
			string fx_str = "\
// This is used by 3dsmax to load the correct parser\n\
string ParamID = \"0x0\";\n\
\n\
// transformations\n\
float4x4 worldViewProj : WORLDVIEWPROJ;\n\
float4 cameraPos : CAMERAPOSITION;\n\
float4x3 world_matrix_array[24] : WORLDMATRIXARRAY;\n\
\n\
float4 lightPos : LIGHTPOS;\n\
\n\
texture g_ImageSampler1 <\n\
string UIName = \"g_ImageSampler1\";\n\
string name = \"xxx_0.dds\";\n\
string ResourceType = \"2D\";\n\
>;\n\
\n\
texture g_SamplerToon <\n\
string UIName = \"g_SamplerToon\";\n\
string name = \"xxx.dyt_0.dds\";\n\
string ResourceType = \"2D\";\n\
>;\n\
\n\
texture g_ImageSamplerTemp14 <\n\
string UIName = \"g_ImageSamplerTemp14\";\n\
string name = \"xxx.dyt_1.dds\";\n\
string ResourceType = \"2D\";\n\
>;\n\
\n\
float4 g_vFadeMulti_PS < string UIName = \"g_vFadeMulti_PS\"; > = float4(0.0f, 0.0f, 0.0f, 0.0f);\n\
float4 g_vFadeRim_PS 	 < string UIName = \"g_vFadeRim_PS\"; > = float4(0.0f, 0.0f, 0.0f, 0.0f);\n\
float4 g_vFadeAdd_PS 	 < string UIName = \"g_vFadeAdd_PS\"; > = float4(0.0f, 0.0f, 0.0f, 0.0f);\n\
\n\
float4 MatCol0 < string UIName = \"MatCol0\"; > = float4(0.0f, 0.0f, 0.0f, 1.0f);\n\
float4 MatCol1 < string UIName = \"MatCol1\"; > = float4(0.16129f, 0.10484f, 0.04032f, 1.0f);\n\
float4 MatCol2 < string UIName = \"MatCol2\"; > = float4(0.0f, 0.0f, 0.0f, 0.0f);\n\
float4 MatCol3 < string UIName = \"MatCol3\"; > = float4(0.0f, 0.0f, 1.0f, 1.0f);		//Battle Damage :   x = Scratch Mark Multiplier, y = Blood Mark Multiplier (forced to 0.0)\n\
float4 MatScale0 < string UIName = \"MatScale0\"; > = float4(1.0f, 1.0f, 1.0f, 0.03f);\n\
float4 MatScale1 < string UIName = \"MatScale1\"; > = float4(0.0f, 1.0f, 1.0f, 1.0f);\n\
\n\
float4 g_vColor0_PS 	 < string UIName = \"g_vColor0_PS\"; > = float4(0.0f, 0.0f, 0.0f, 0.0f);\n\
float4 g_vColor1_PS 	 < string UIName = \"g_vColor1_PS\"; > = float4(0.0f, 0.0f, 0.0f, 0.0f);\n\
float4 g_vColor2_PS 	 < string UIName = \"g_vColor2_PS\"; > = float4(0.0f, 0.0f, 0.0f, 0.0f);\n\
float4 g_vColor12_PS 	 < string UIName = \"g_vColor12_PS\"; > = float4(0.0f, 0.0f, 0.0f, 0.0f);\n\
float4 g_Color_Multiplier < string UIName = \"g_Color_Multiplier\"; > = float4(1.0f, 1.0f, 1.0f, 1.0f);\n\
float4 g_vParam4_PS 	 < string UIName = \"g_vParam4_PS\"; > = float4(0.0f, 0.0f, 0.0f, 0.0f);\n\
float4 g_vParam5_PS 	 < string UIName = \"g_vParam5_PS\"; > = float4(0.0f, 0.0f, 0.0f, 0.0f);\n\
float4 g_Toon_Detail 	 < string UIName = \"g_Toon_Detail\"; > = float4(0.0f, 23.2558f, 0.04587f, 0.0f);\n\
\n\
////////////////////////////////////////////////////////////////////////////////\n\
technique DefaultTechnique\n\
{\n\
	pass P0\n\
	{\n\
		CullMode = CW;\n\
		Lighting = true;\n\
		SHADEMODE = GOURAUD;\n\
		ZEnable = true;\n\
		ZWriteEnable = true;\n\
		\n\
		VertexShaderConstant[0] = <worldViewProj>;\n\
		VertexShaderConstant[72] = <cameraPos>;\n\
		VertexShaderConstant[104] = <world_matrix_array>;\n\
		\n\
		PixelShaderConstant[51] = <g_vFadeMulti_PS>;\n\
		PixelShaderConstant[52] = <g_vFadeRim_PS>;\n\
		PixelShaderConstant[53] = <g_vFadeAdd_PS>;\n\
		\n\
		PixelShaderConstant[64] = <lightPos>;\n\
		\n\
		PixelShaderConstant[84] = <MatCol0>;\n\
		PixelShaderConstant[85] = <MatCol1>;\n\
		PixelShaderConstant[86] = <MatCol2>;\n\
		PixelShaderConstant[87] = <MatCol3>;\n\
		PixelShaderConstant[92] = <MatScale0>;\n\
		PixelShaderConstant[93] = <MatScale1>;\n\
		\n\
		PixelShaderConstant[105] = <g_vColor0_PS>;\n\
		PixelShaderConstant[106] = <g_vColor1_PS>;\n\
		PixelShaderConstant[107] = <g_vColor2_PS>;\n\
		PixelShaderConstant[117] = <g_vColor12_PS>;\n\
		PixelShaderConstant[139] = <g_Color_Multiplier>;\n\
		PixelShaderConstant[140] = <g_vParam4_PS>;\n\
		PixelShaderConstant[141] = <g_vParam5_PS>;\n\
		PixelShaderConstant[142] = <g_Toon_Detail>;\n\
		\n\
		Texture[1] = <g_ImageSampler1>;\n\
		MinFilter[1] = Linear;\n\
		MagFilter[1] = Linear;\n\
		MipFilter[1] = Linear;\n\
		MipMapLodBias[1] = 0;\n\
		//AddressU[1] = Clamp;\n\
		//AddressV[1] = Clamp;\n\
		\n\
		Texture[4] = <g_SamplerToon>;\n\
		MinFilter[4] = None;\n\
		MagFilter[4] = None;\n\
		MipFilter[4] = None;\n\
		MipMapLodBias[4] = 0;\n\
		//AddressU[4] = Clamp;\n\
		//AddressV[4] = Clamp;\n\
		\n\
		Texture[14] = <g_ImageSamplerTemp14>;\n\
		MinFilter[14] = Linear;\n\
		MagFilter[14] = Linear;\n\
		MipFilter[14] = Linear;\n\
		MipMapLodBias[14] = 0;\n\
		//AddressU[14] = Clamp;\n\
		//AddressV[14] = Clamp;\n\
		\n\
		VertexShader = asm\n\
		{\n\
					" + string(new_data_vs) + " \n\
		};\n\
		\n\
		PixelShader = asm\n\
		{\n\
					" + string(new_data_ps) + " \n\
		};\n\
	}\n\
}\n";

			delete new_data_vs;
			delete new_data_ps;

			//save file
			{
				File file(shader_name +".fx", LIBXENOVERSE_FILE_WRITE_TEXT);
				if (file.valid()) {
					file.writeString(&fx_str);
					file.close();
				}
			}
		}
	}
#endif
}