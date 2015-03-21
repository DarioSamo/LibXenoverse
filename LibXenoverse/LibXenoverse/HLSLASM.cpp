#ifdef LIBXENOVERSE_WINSDK_SUPPORT

namespace LibXenoverse {
	char *HLSLASM::assembleFromFile(string filename, size_t &data_size) {
		char *data = getBytesFrom(filename, data_size);
		char *new_data = assemble(data, data_size);
		delete data;
		return new_data;
	}

	char *HLSLASM::assemble(char *data, size_t &data_size) {
		if (!data) {
			return NULL;
		}

		// Compile shader
		LPD3DXBUFFER psBuffer = NULL;
		LPD3DXBUFFER errorBuffer = NULL;
		HRESULT hr = D3DXAssembleShader(data, data_size, NULL, NULL, 0, &psBuffer, &errorBuffer);
		if (hr != D3D_OK) {
			printf("Failed compiling pixel shader %08X\n", hr);
			getchar();
			return NULL;
		}

		data_size = (psBuffer)->GetBufferSize();
		return (char *)(psBuffer)->GetBufferPointer();
	}

	char *HLSLASM::disassembleFromFile(string filename, size_t &data_size) {
		char *data = getBytesFrom(filename, data_size);
		char *new_data = disassemble(data, data_size);
		delete data;
		return new_data;
	}

	char *HLSLASM::disassemble(char *data, size_t &data_size) {
		if (!data) {
			return NULL;
		}

		// Disassemble shader
		ID3DBlob* shader_blob = NULL;
		D3DCreateBlob(data_size, &shader_blob);
		memcpy(shader_blob->GetBufferPointer(), data, data_size);

		ID3DBlob* shader_disassembly = NULL;
		D3DDisassemble(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), 0, 0, &shader_disassembly);

		data_size = shader_disassembly->GetBufferSize();
		char *mdata = new char[data_size];
		memcpy(mdata, shader_disassembly->GetBufferPointer(), data_size);

		shader_blob->Release();
		shader_disassembly->Release();
		return mdata;
	}
}

#endif