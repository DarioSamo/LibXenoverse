namespace LibXenoverse {
	bool CUS::load(string filename) {
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

			}
		}
		else {
			File file(filename, LIBXENOVERSE_FILE_READ_BINARY);

			if (file.valid() && file.readHeader(LIBXENOVERSE_CUS_SIGNATURE)) {
				read(&file);
				file.close();
			}
			else return false;
		}

		return true;
	}

	void CUS::read(File *file) {
		file->goToAddress(8);
		unsigned int unknown_total = 0;
		unsigned int unknown_address = 0;
		unsigned int unknown_1_total = 0;
		unsigned int unknown_1_address = 0;
		unsigned int unknown_2_total = 0;
		unsigned int unknown_2_address = 0;
		unsigned int unknown_3_total = 0;
		unsigned int unknown_3_address = 0;
		unsigned int unknown_4_total = 0;
		unsigned int unknown_4_address = 0;
		unsigned int unknown_5_total = 0;
		unsigned int unknown_5_address = 0;

		file->readInt32E(&unknown_total);
		file->readInt32E(&unknown_address);

		file->readInt32E(&unknown_1_total);
		file->readInt32E(&unknown_2_total);
		file->readInt32E(&unknown_3_total);
		file->readInt32E(&unknown_4_total);
		file->readInt32E(&unknown_5_total);
		file->readInt32E(&unknown_1_address);
		file->readInt32E(&unknown_2_address);
		file->readInt32E(&unknown_3_address);
		file->readInt32E(&unknown_4_address);
		file->readInt32E(&unknown_5_address);

		for (size_t i = 0; i < unknown_total; i++) {
			file->goToAddress(unknown_address + i*32);
		}

		for (size_t i = 0; i < unknown_1_total; i++) {
			file->goToAddress(unknown_1_address + i * 48);
		}

		for (size_t i = 0; i < unknown_2_total; i++) {
			file->goToAddress(unknown_2_address + i * 48);
		}

		for (size_t i = 0; i < unknown_3_total; i++) {
			file->goToAddress(unknown_3_address + i * 48);
		}

		for (size_t i = 0; i < unknown_4_total; i++) {
			file->goToAddress(unknown_4_address + i * 48);
		}

		for (size_t i = 0; i < unknown_5_total; i++) {
			file->goToAddress(unknown_5_address + i * 48);
		}


		printf("Found %d totals\n", unknown_total);
	}


	void CUS::save(string filename, bool big_endian) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.writeHeader(LIBXENOVERSE_CUS_SIGNATURE, big_endian);
			write(&file);
			file.close();
		}
	}

	void CUS::write(File *file) {
		
	}

	void CUS::saveXML(string filename) {
	}
}