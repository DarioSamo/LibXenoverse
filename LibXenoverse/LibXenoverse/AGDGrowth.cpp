namespace LibXenoverse {
	void AGDGrowth::read(File *file) {
		file->readInt32E(&level);
		file->readInt32E(&xp_to_next_level);
		file->readInt32E(&base_xp);
		file->readInt32E(&flag);
	}

	void AGDGrowth::write(File *file) {
		file->writeInt32E(&level);
		file->writeInt32E(&xp_to_next_level);
		file->writeInt32E(&base_xp);
		file->writeInt32E(&flag);
	}

	void AGDGrowth::readXML(TiXmlElement *root) {
		root->QueryUnsignedAttribute("level", &level);
		root->QueryUnsignedAttribute("xp_to_next_level", &xp_to_next_level);
		root->QueryUnsignedAttribute("flag", &flag);
	}

	void AGDGrowth::writeXML(TiXmlElement *root) {
		TiXmlElement* scaleRoot = new TiXmlElement("AGDGrowth");
		scaleRoot->SetAttribute("level", ToString(level));
		scaleRoot->SetAttribute("xp_to_next_level", ToString(xp_to_next_level));
		scaleRoot->SetAttribute("flag", ToString(flag));
		root->LinkEndChild(scaleRoot);
	}
}