namespace LibXenoverse {
	bool AGD::load(string filename) {
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

			unsigned int base_xp = 0;

			pElem = pElem->FirstChildElement();
			for (pElem; pElem; pElem = pElem->NextSiblingElement()) {
				string element_name = pElem->ValueStr();
				if (element_name == "AGDGrowth") {
					AGDGrowth growth;
					growth.readXML(pElem);
					growth.base_xp = base_xp;
					base_xp += growth.xp_to_next_level;
					growths.push_back(growth);
				}
			}
		}
		else {
			File file(filename, LIBXENOVERSE_FILE_READ_BINARY);

			if (file.valid() && file.readHeader(LIBXENOVERSE_AGD_SIGNATURE)) {
				read(&file);
				file.close();
			}
			else return false;
		}

		return true;
	}

	void AGD::read(File *file) {
		file->goToAddress(8);

		unsigned int table_total = 0;
		unsigned int table_address = 0;
		file->readInt32E(&table_total);
		file->readInt32E(&table_address);

		growths.resize(table_total);
		for (size_t i = 0; i < table_total; i++) {
			file->goToAddress(table_address + i * 16);
			growths[i].read(file);
		}
	}


	void AGD::save(string filename, bool big_endian) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.writeHeader(LIBXENOVERSE_AGD_SIGNATURE, big_endian);
			write(&file);
			file.close();
		}
	}

	void AGD::write(File *file) {
		// Header
		unsigned short unknown_1 = 0x0010;
		unsigned int table_total = growths.size();
		unsigned int table_address = 16;

		file->writeInt16E(&unknown_1);
		file->writeInt32E(&table_total);
		file->writeInt32E(&table_address);

		for (size_t i = 0; i < table_total; i++) {
			growths[i].write(file);
		}
	}

	void AGD::saveXML(string filename) {
		TiXmlDocument doc;
		TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
		doc.LinkEndChild(decl);

		TiXmlElement *root = new TiXmlElement("AGD");
		for (size_t i = 0; i < growths.size(); i++) {
			growths[i].writeXML(root);
		}
		doc.LinkEndChild(root);

		doc.SaveFile(filename);
	}
}