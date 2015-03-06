namespace LibXenoverse {
	void CBSScale::read(File *file) {
		file->readInt16E(&id_1);
		file->readInt16E(&id_2);
		file->readFloat32E(&x);
		file->readFloat32E(&y);
		file->readFloat32E(&z);

		/*
		x = 0.25 + (rand() % 2000) / 1000.0f;
		y = 0.25 + (rand() % 2000) / 1000.0f;
		z = 0.25 + (rand() % 2000) / 1000.0f;
		*/
	}

	void CBSScale::write(File *file) {
		file->writeInt16E(&id_1);
		file->writeInt16E(&id_2);
		file->writeFloat32E(&x);
		file->writeFloat32E(&y);
		file->writeFloat32E(&z);
	}

	void CBSScale::readXML(TiXmlElement *root) {
		unsigned int temp_int = 0;
		root->QueryUnsignedAttribute("id_1", &temp_int);
		id_1 = temp_int;
		root->QueryUnsignedAttribute("id_2", &temp_int);
		id_2 = temp_int;

		root->QueryFloatAttribute("x", &x);
		root->QueryFloatAttribute("y", &y);
		root->QueryFloatAttribute("z", &z);
	}

	void CBSScale::writeXML(TiXmlElement *root) {
		TiXmlElement* scaleRoot = new TiXmlElement("CBSScale");
		scaleRoot->SetAttribute("id_1", ToString(id_1));
		scaleRoot->SetAttribute("id_2", ToString(id_2));
		scaleRoot->SetAttribute("x", ToString(x));
		scaleRoot->SetAttribute("y", ToString(y));
		scaleRoot->SetAttribute("z", ToString(z));
		root->LinkEndChild(scaleRoot);
	}
}