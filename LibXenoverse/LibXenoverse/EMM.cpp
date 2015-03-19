

namespace LibXenoverse {
	bool EMM::load(string filename) {
		name = nameFromFilenameNoExtension(filename, true);

		if (filename.find(".xml") != string::npos) {
			TiXmlDocument doc(filename);
			if (!doc.LoadFile()) {
				return false;
			}

			TiXmlHandle hDoc(&doc);
			TiXmlElement* pElem;
			TiXmlHandle hRoot(0);

			pElem = hDoc.FirstChildElement().Element();
			if (!pElem) {
				return false;
			}

			pElem = pElem->FirstChildElement();
			for (pElem; pElem; pElem = pElem->NextSiblingElement()) {
				string element_name = pElem->ValueStr();

				if (element_name == "EMMMaterial") {
					EMMMaterial *material = new EMMMaterial();
					material->readXML(pElem);
					materials.push_back(material);
				}
			}
		}
		else {
			File file(filename, LIBXENOVERSE_FILE_READ_BINARY);

			if (file.valid() && file.readHeader(LIBXENOVERSE_EMM_SIGNATURE)) {
				read(&file);
				file.close();
			}
			else return false;
		}

		return true;
	}

	void EMMParameter::readXML(TiXmlElement *root) {
		string temp_str = "";
		memset(name, 0, 32);
		root->QueryStringAttribute("name", &temp_str);
		strcpy(name, temp_str.c_str());

		root->QueryUnsignedAttribute("type", &type);

		if (type == 0x10001) {
			root->QueryUnsignedAttribute("value", &value);
		}
		else if (type == 0x0) {
			root->QueryFloatAttribute("value", (float *)(&value));
		}
		else {
			printf("Unknown Parameter Flag %x\n", type);
			getchar();
		}
	}

	void EMMMaterial::readXML(TiXmlElement *root) {
		string temp_str = "";
		memset(name, 0, 32);
		memset(shader_name, 0, 32);
		root->QueryStringAttribute("name", &temp_str);
		strcpy(name, temp_str.c_str());
		root->QueryStringAttribute("shader_name", &temp_str);
		strcpy(shader_name, temp_str.c_str());

		TiXmlElement *pElem = root->FirstChildElement();
		for (pElem; pElem; pElem = pElem->NextSiblingElement()) {
			string element_name = pElem->ValueStr();

			if (element_name == "EMMParameter") {
				EMMParameter *parameter = new EMMParameter();
				parameter->readXML(pElem);
				parameters.push_back(parameter);
			}
		}
	}

	void EMMParameter::read(File *file) {
		file->read(name, 32);
		file->readInt32E(&type);
		file->readInt32E(&value);

		printf("  Parameter %s - %d - %f\n", name, type, value);
	}

	void EMMMaterial::read(File *file) {
		file->read(name, 32);
		file->read(shader_name, 32);

		unsigned short parameter_count = 0;
		file->readInt16E(&parameter_count);
		file->moveAddress(2);

		printf("Reading Material %s with shader %s and %d parameters.\n", name, shader_name, parameter_count);

		parameters.resize(parameter_count);
		for (size_t i = 0; i < parameter_count; i++) {
			parameters[i] = new EMMParameter();
			parameters[i]->read(file);
		}
	}

	void EMM::read(File *file) {
		file->goToAddress(0xC);

		unsigned int material_base_address = 0;
		file->readInt32E(&material_base_address);

		unsigned short material_count = 0;
		file->goToAddress(material_base_address);
		file->readInt16E(&material_count);
		file->moveAddress(2);

		printf("Found %d materials.\n", material_count);

		materials.resize(material_count);
		for (size_t i = 0; i < material_count; i++) {
			file->goToAddress(material_base_address + i*4 + 4);

			unsigned int material_address = 0;
			file->readInt32E(&material_address);
			file->goToAddress(material_base_address + material_address);

			materials[i] = new EMMMaterial();
			materials[i]->read(file);
		}
	}

	void EMMParameter::write(File *file) {
		file->write(name, 32);
		file->writeInt32E(&type);
		file->writeInt32E(&value);
	}

	void EMMMaterial::write(File *file) {
		file->write(name, 32);
		file->write(shader_name, 32);

		unsigned short parameter_count = parameters.size();
		file->writeInt16E(&parameter_count);
		file->writeNull(2);

		for (size_t i = 0; i < parameter_count; i++) {
			parameters[i]->write(file);
		}
	}

	void EMM::write(File *file) {
		// Header
		unsigned short unknown_1 = 0x0010;
		file->writeInt16E(&unknown_1);
		file->writeNull(4);

		unsigned int material_base_address = 16;
		file->writeInt32E(&material_base_address);

		unsigned short material_count = materials.size();
		file->writeInt16E(&material_count);
		file->writeNull(2);

		file->writeNull(material_count * 4);
		for (size_t i = 0; i < material_count; i++) {
			unsigned int material_address = file->getCurrentAddress() - material_base_address;
			file->goToAddress(material_base_address + i * 4 + 4);
			file->writeInt32E(&material_address);
			file->goToAddress(material_address + material_base_address);
			materials[i]->write(file);
		}
	}

	void EMM::save(string filename, bool big_endian) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.writeHeader(LIBXENOVERSE_EMM_SIGNATURE, big_endian);
			write(&file);
			file.close();
		}
	}

	void EMMParameter::writeXML(TiXmlElement *root) {
		TiXmlElement* parameterRoot = new TiXmlElement("EMMParameter");
		parameterRoot->SetAttribute("name", ToString(name));
		parameterRoot->SetAttribute("type", ToString(type));

		if (type == 0x10001) {
			parameterRoot->SetAttribute("value", ToString(value));
		}
		else if (type == 0x0) {
			parameterRoot->SetAttribute("value", ToString(*(float *)(&value)));
		}
		else {
			printf("Unknown Parameter Flag %x\n", type);
			getchar();
		}

		root->LinkEndChild(parameterRoot);
	}


	void EMMMaterial::writeXML(TiXmlElement *root) {
		TiXmlElement* materialRoot = new TiXmlElement("EMMMaterial");
		materialRoot->SetAttribute("name", ToString(name));
		materialRoot->SetAttribute("shader_name", ToString(shader_name));

		for (size_t i = 0; i < parameters.size(); i++) {
			parameters[i]->writeXML(materialRoot);
		}

		root->LinkEndChild(materialRoot);
	}

	void EMM::saveXML(string filename) {
		TiXmlDocument doc;
		TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
		doc.LinkEndChild(decl);

		TiXmlElement *root = new TiXmlElement("EMM");
		for (size_t i = 0; i < materials.size(); i++) {
			materials[i]->writeXML(root);
		}
		doc.LinkEndChild(root);

		doc.SaveFile(filename);
	}
}