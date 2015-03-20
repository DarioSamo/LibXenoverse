#ifdef LIBXENOVERSE_ZLIB_SUPPORT

namespace LibXenoverse {
	bool EMZ::load(string filename) {
		File file(filename, LIBXENOVERSE_FILE_READ_BINARY);
		if (file.valid() && file.readHeader(LIBXENOVERSE_EMZ_SIGNATURE, LIBXENOVERSE_LITTLE_ENDIAN)) {
			read(&file);
			file.close();
		}
		else return false;

		return true;
	}

	void EMZ::save(string filename) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			write(&file);
			file.close();
		}
	}

	void EMZ::saveUncompressed(string filename) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.write(data, data_size);
			file.close();
		}
	}

	string EMZ::detectNewExtension() {
		if ((!data) || (data_size <= 4)) {
			return ".emz.invalid";
		}

		char data_header[5]="#EMZ";
		strncpy(data_header, (char *) data, 4);

		if (!strcmp(data_header, LIBXENOVERSE_EMB_SIGNATURE)) {
			return LIBXENOVERSE_EMB_EXTENSION;
		}

		return ".emz.uncompressed";
	}

	void EMZ::read(File *file) {
		// Read Header
		file->goToAddress(8);
		unsigned int uncompressed_size = 0;
		unsigned int compressed_data_offset = 0;
		file->readInt32E(&uncompressed_size);
		file->readInt32E(&compressed_data_offset);

		data = new unsigned char[uncompressed_size];
		data_size = uncompressed_size;

		// Read Compressed Data
		size_t buffer_size = file->getFileSize() - compressed_data_offset;
		unsigned char *compressed_data = new unsigned char[buffer_size];
		file->goToAddress(compressed_data_offset);
		file->read(compressed_data, buffer_size);
		
		// Decompress Data
		int ret;
		z_stream zstream;
		zstream.zalloc = (alloc_func)Z_NULL;
		zstream.zfree = (free_func)Z_NULL;
		zstream.opaque = (voidpf)Z_NULL;

		int zlib_status = inflateInit2(&zstream, -15);

		if (zlib_status == Z_OK) {
			size_t current_offset = 0;
			size_t current_data_offset = 0;

			do {
				// Calculate Remaining Bytes
				zstream.avail_in = min(LIBXENOVERSE_EMZ_CHUNK, max(buffer_size - current_offset, 0));
				if (zstream.avail_in == 0) break;

				// Move in pointer to the current compressed data chunk
				zstream.next_in = compressed_data + current_offset;
				current_offset += zstream.avail_in;
				do
				{
					zstream.avail_out = LIBXENOVERSE_EMZ_CHUNK;
					zstream.next_out = data + current_data_offset;
					ret = inflate(&zstream, Z_NO_FLUSH);
					current_data_offset += LIBXENOVERSE_EMZ_CHUNK - zstream.avail_out;

					if (ret != Z_OK) {
						break;
					}
				} while (zstream.avail_out == 0);
			} while (ret != Z_STREAM_END);

			inflateEnd(&zstream);
		}
		else {
			return;
		}

		delete[] compressed_data;
	}

	void EMZ::write(File *file) {
		char data_header[5] = "#EMZ";
		file->write(data_header, 4);
		file->writeNull(4); // FIXME: Figure out what this part is. Probably a hash?
		file->writeInt32E(&data_size);
		unsigned int data_offset = 16;
		file->writeInt32E(&data_offset);

		// Write compressed data
		int ret, flush;
		unsigned out_size;
		z_stream zstream;
		unsigned char out[LIBXENOVERSE_EMZ_CHUNK];
		zstream.zalloc = (alloc_func)Z_NULL;
		zstream.zfree = (free_func)Z_NULL;
		zstream.opaque = (voidpf)Z_NULL;

		// Probably not the same compression settings as the game, but similar enough
		ret = deflateInit2(&zstream, Z_BEST_COMPRESSION, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);

		size_t current_offset = 0;
		do {
			zstream.avail_in = min(LIBXENOVERSE_EMZ_CHUNK, max(data_size - current_offset, 0));
			zstream.next_in = data + current_offset;
			current_offset += zstream.avail_in;
			flush = (current_offset >= data_size) ? Z_FINISH : Z_NO_FLUSH;

			do {
				zstream.avail_out = LIBXENOVERSE_EMZ_CHUNK;
				zstream.next_out = out;
				ret = deflate(&zstream, flush);
				out_size = LIBXENOVERSE_EMZ_CHUNK - zstream.avail_out;
				file->write(out, out_size);
			} while (zstream.avail_out == 0);

		} while (flush != Z_FINISH);

		(void)deflateEnd(&zstream);
	}
}

#endif