#include "EMBOgre.h"

EMBOgreDataStream::EMBOgreDataStream(EMBFile *file_p, Ogre::uint16 accessMode) : Ogre::DataStream(accessMode) {
	current_offset = 0;
	emb_file = file_p;

	emb_data = emb_file->getData();
	emb_data_size = emb_file->getSize();
}

size_t EMBOgreDataStream::read(void* buf, size_t count) {
	size_t bytes_to_read = min(count, emb_data_size - current_offset);
	memcpy(buf, emb_data + current_offset, bytes_to_read);
	current_offset += bytes_to_read;
	return bytes_to_read;
}

void EMBOgreDataStream::skip(long count) {
	current_offset += count;
}

void EMBOgreDataStream::seek(size_t pos) {
	current_offset = pos;
}

size_t EMBOgreDataStream::tell(void) const {
	return current_offset;
}

bool EMBOgreDataStream::eof(void) const {
	return current_offset >= emb_data_size;
}

void EMBOgreDataStream::close(void) {

}


EMBOgre::EMBOgre() {

}

void EMBOgre::createOgreTexture(EMBFile *file, size_t index) {
	string ogre_emb_name = name + "_";
	string emb_texture_name = file->getName();

	if (emb_texture_name.size()) {
		ogre_emb_name += emb_texture_name;
	}
	else {
		ogre_emb_name += ToString(index);
	}

	Ogre::DataStreamPtr data_stream(new EMBOgreDataStream(file));
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createResource(ogre_emb_name, XENOVIEWER_RESOURCE_GROUP).staticCast<Ogre::Texture>();
	Ogre::Image image;
	image.load(data_stream, "DDS");
	texture->loadImage(image);

	ogre_textures[index] = texture;
}

void EMBOgre::createOgreTextures() {
	ogre_textures.resize(files.size());

	for (size_t i = 0; i < files.size(); i++) {
		createOgreTexture(files[i], i);
	}
}

void EMBOgre::createOgreShader(EMBFile *file) {
	string shader_name = nameFromFilenameNoExtension(file->getName());
	string extension = extensionFromFilename(file->getName());
	bool vertex_shader = false;

	if (extension == "xpu");
	else if (extension == "xvu") vertex_shader = true;
	else return;

	size_t size = file->getSize();
	char *new_data = HLSLASM::disassemble(file->getData(), size);

	if (vertex_shader) {
		Ogre::GpuProgramPtr program = Ogre::GpuProgramManager::getSingletonPtr()->createProgramFromString(shader_name, XENOVIEWER_RESOURCE_GROUP, Ogre::String(new_data), Ogre::GPT_VERTEX_PROGRAM, "vs_3_0");
		ogre_shaders.push_back(program);
	}
	else {
		Ogre::GpuProgramPtr program = Ogre::GpuProgramManager::getSingletonPtr()->createProgramFromString(shader_name, XENOVIEWER_RESOURCE_GROUP, Ogre::String(new_data), Ogre::GPT_FRAGMENT_PROGRAM, "ps_3_0");
		ogre_shaders.push_back(program);
	}

	delete new_data;
}

void EMBOgre::createOgreShaders() {
	for (size_t i = 0; i < files.size(); i++) {
		createOgreShader(files[i]);
	}
}


EMBOgre::~EMBOgre() {
	for (size_t i = 0; i < ogre_textures.size(); i++) {
		Ogre::TextureManager::getSingleton().remove(ogre_textures[i]->getName());
	}

	for (size_t i = 0; i < ogre_shaders.size(); i++) {
		Ogre::GpuProgramManager::getSingleton().remove(ogre_shaders[i]->getName());
	}
}