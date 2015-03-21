#ifndef EMBOGRE_H_INCLUDED
#define EMBOGRE_H_INCLUDED

class EMBOgreDataStream : public Ogre::DataStream {
	protected:
		EMBFile *emb_file;
		size_t current_offset;

		char *emb_data;
		size_t emb_data_size;
	public:
		EMBOgreDataStream(EMBFile *file_p, Ogre::uint16 accessMode = READ);
		size_t read(void* buf, size_t count);
		void skip(long count);
		void seek(size_t pos);
		size_t tell(void) const;
		bool eof(void) const;
		void close(void);
};

class EMBOgre : public EMB {
protected:
	vector<Ogre::TexturePtr> ogre_textures;
public:
	EMBOgre();

	void createOgreTexture(EMBFile *file, size_t index);
	void createOgreShader(EMBFile *file);
	void createOgreTextures();
	void createOgreShaders();

	vector<Ogre::TexturePtr> getOgreTextures() {
		return ogre_textures;
	}
};

#endif