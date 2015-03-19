namespace LibXenoverse {
	bool CBS::load(string filename) {
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

				if (element_name == "CBSScale") {
					CBSScale scale;
					scale.readXML(pElem);
					scales.push_back(scale);
				}
			}
		}
		else {
			File file(filename, LIBXENOVERSE_FILE_READ_BINARY);

			if (file.valid() && file.readHeader(LIBXENOVERSE_CBS_SIGNATURE)) {
				read(&file);
				file.close();
			}
			else return false;
		}

		return true;
	}

	void CBS::read(File *file) {
		file->goToAddress(8);

		unsigned int table_total = 0;
		unsigned int table_address = 0;
		file->readInt32E(&table_total);
		file->readInt32E(&table_address);

		scales.resize(table_total);
		for (size_t i = 0; i < table_total; i++) {
			file->goToAddress(table_address + i*16);
			scales[i].read(file);
		}
	}


	void CBS::save(string filename, bool big_endian) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.writeHeader(LIBXENOVERSE_CBS_SIGNATURE, big_endian);
			write(&file);
			file.close();
		}
	}

	void CBS::write(File *file) {
		// Header
		unsigned short unknown_1 = 0x0010;
		unsigned int table_total = scales.size();
		unsigned int table_address = 16;

		file->writeInt16E(&unknown_1);
		file->writeInt32E(&table_total);
		file->writeInt32E(&table_address);

		for (size_t i = 0; i < table_total; i++) {
			scales[i].write(file);
		}
	}

	void CBS::saveXML(string filename) {
		TiXmlDocument doc;
		TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
		doc.LinkEndChild(decl);

		TiXmlElement *root = new TiXmlElement("CBS");
		for (size_t i = 0; i < scales.size(); i++) {
			scales[i].writeXML(root);
		}
		doc.LinkEndChild(root);

		doc.SaveFile(filename);
	}
}