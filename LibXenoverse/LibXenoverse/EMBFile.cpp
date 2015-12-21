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

	bool EMBFile::write(File *file) {
		return file->write(data, data_size);
	}

 bool EMBFile::save(string filename) {
   if (!data)  {
     return false;
   }
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			bool success = file.write(data, data_size);
			file.close();
      return success;
		}
    return false;
	}
}