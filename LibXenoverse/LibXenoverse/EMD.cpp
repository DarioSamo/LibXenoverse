namespace LibXenoverse {
	bool EMD::load(string filename) {
		name = nameFromFilenameNoExtension(filename, true);

		File file(filename, LIBXENOVERSE_FILE_READ_BINARY);
		if (file.valid() && file.readHeader(LIBXENOVERSE_EMD_SIGNATURE)) {
			read(&file);
			file.close();
		}
		else return false;

		return true;
	}

	void EMD::read(File *file) {
		file->goToAddress(0x10);

		unsigned int address = 0;

		unsigned short unknown_total = 0;
		unsigned short model_total = 0;
		file->readInt16E(&unknown_total);
		file->readInt16E(&model_total);

		LOG_DEBUG("Total Unknown: %d\n", unknown_total);
		LOG_DEBUG("Total Models: %d\n", model_total);

		unsigned int root_model_address = 0;
		unsigned int root_names_address = 0;

		file->readInt32E(&root_model_address);
		file->readInt32E(&root_names_address);

		// Root Model Section
		for (size_t i = 0; i < model_total; i++) {
			file->goToAddress(root_model_address + i * 4);
			file->readInt32E(&address);
			file->goToAddress(address);

			EMDModel *emd_model = new EMDModel();
			emd_model->read(file);
			models.push_back(emd_model);
		}

		// Root Names Section
		for (size_t i = 0; i < model_total; i++) {
			file->goToAddress(root_names_address + i * 4);
			file->readInt32E(&address);
			file->goToAddress(address);

			string name = "";
			file->readString(&name);
			model_names.push_back(name);

			printf("Found something with index %d and name %s\n", i, name.c_str());
		}
	}

	void EMD::write(File *file) {
		unsigned short unknown_1 = 0x001C;
		unsigned int unknown_2 = 0x00010001;
		unsigned short model_total = models.size();
		file->writeInt16E(&unknown_1);
		file->writeInt32E(&unknown_2);
		file->writeNull(4);
		file->writeNull(2);
		file->writeInt16E(&model_total);
		file->writeNull(8);

		// Model Table
		unsigned int model_table_address = file->getCurrentAddress();
		file->writeNull(model_total * 4);

		for (size_t i = 0; i < model_total; i++) {
			file->fixPadding(16);
			unsigned int model_address = file->getCurrentAddress();

			// Fix Offset On Table
			file->goToAddress(model_table_address + i*4);
			file->writeInt32E(&model_address);

			// Write Model
			file->goToAddress(model_address);
			models[i]->write(file);
		}


		// Name Table
		unsigned int name_table_address = file->getCurrentAddress();
		file->writeNull(model_total * 4);

		for (size_t i = 0; i < model_total; i++) {
			unsigned int name_address = file->getCurrentAddress();

			// Fix Offset On Table
			file->goToAddress(name_table_address + i * 4);
			file->writeInt32E(&name_address);

			// Write Name
			file->goToAddress(name_address);
			file->writeString(&model_names[i]);
		}

		file->goToAddress(0x14);
		file->writeInt32E(&model_table_address);
		file->writeInt32E(&name_table_address);
		file->goToEnd();
	}


	void EMD::save(string filename, bool big_endian) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.writeHeader(LIBXENOVERSE_EMD_SIGNATURE, big_endian);
			write(&file);
			file.close();
		}
	}

	void EMD::setVertexScale(float scale) {
		for (size_t i = 0; i < models.size(); i++) {
			models[i]->setVertexScale(scale);
		}
	}
}