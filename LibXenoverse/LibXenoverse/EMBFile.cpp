namespace LibXenoverse {
	EMBFile::EMBFile(string filename) {
		File file(filename, LIBXENOVERSE_FILE_READ_BINARY);
		if (file.valid()) {
			name = nameFromFilename(filename);

			// Read entire file into data buffer
			data_size = file.getFileSize();
			data = (char *)malloc(data_size);
			file.read(data, data_size);

			file.close();
		}
	}

	void EMBFile::write(File *file) {
		file->write(data, data_size);
	}

	void EMBFile::save(string filename) {
		if (!data) return;

		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.write(data, data_size);
			file.close();
		}
	}
}