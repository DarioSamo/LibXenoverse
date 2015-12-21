#include <stdio.h>  /* defines FILENAME_MAX */
//#ifdef WINDOWS
#ifdef WIN32
	#include <direct.h>
	#define GetCurrentDir _getcwd
#else
	#include <unistd.h>
	#define GetCurrentDir getcwd
#endif




namespace LibXenoverse {
	wstring_convert<codecvt<char16_t, char, mbstate_t>, char16_t> convert16;

	FILE *global_debugging_log = NULL;

	void initializeDebuggingLog() {
		#ifdef LIBXENOVERSE_DEBUGGING_LOG
		

		char cCurrentPath[FILENAME_MAX];
		if (GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
		{
			cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
		}else{
			cCurrentPath[0] = '\0';
		}
		string basePath(cCurrentPath);

		OutputDebugStringA( ("WorkingDir: " + basePath +"\n").c_str());

		global_debugging_log = fopen( (basePath + "\\libxenoverse.log").c_str(), "wt");
		#endif
	}

	void flushDebuggingLog() {
		if (!global_debugging_log) return;
		#ifdef LIBXENOVERSE_DEBUGGING_LOG
		fflush(global_debugging_log);
		#endif
	}

	void closeDebuggingLog() {
		if (!global_debugging_log) return;
		#ifdef LIBXENOVERSE_DEBUGGING_LOG
		fclose(global_debugging_log);
		#endif
	}

	void endianSwap(unsigned int& x) {
		x = (x>>24) |
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
	}

	void endianSwap(int& x) {
	    x = (x>>24) |
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
	}

	void endianSwap(unsigned short& x) {
	    x = (x>>8) | (x<<8);
	}

	File::File(string filename, string mode) {
		file_ptr=fopen(filename.c_str(), mode.c_str());

		path=filename;
		global_offset = 0;

		if (!file_ptr) {
			return;
		}
	}

	int File::endOfFile() {
		if (file_ptr) {
			return feof(file_ptr);
		}
		else {
			return true;
		}
	}

	bool File::valid() {
		if (file_ptr) return true;
		else return false;
	}

	void File::close() {
		if (file_ptr) {
			fclose(file_ptr);
		}
		else {
		}
	}

	bool File::readSafeCheck(void *dest) {
		if (!file_ptr) {
			return false;
		}
		if (!dest) {
			return false;
		}

		return true;
	}

	void File::read(void *dest, size_t sz) {
		if (!readSafeCheck(dest)) return;
		fread(dest, sz, 1, file_ptr);
	}

	void File::readInt16(unsigned short *dest) {
		if (!readSafeCheck(dest)) return;
		fread(dest, sizeof(short), 1, file_ptr);
	}
	
	void File::readInt16BE(unsigned short *dest) {
		if (!readSafeCheck(dest)) return;
		fread(dest, sizeof(short), 1, file_ptr);
		endianSwap(*dest);
	}

	void File::readInt32(unsigned int *dest) {
		if (!readSafeCheck(dest)) return;
		fread(dest, sizeof(int), 1, file_ptr);
	}

	void File::readInt32BE(unsigned int *dest) {
		if (!readSafeCheck(dest)) return;
		fread(dest, sizeof(int), 1, file_ptr);
		endianSwap(*dest);
	}

	void File::readFloat8(float *dest) {
		if (!readSafeCheck(dest)) return;
		unsigned char v=0;
		fread(&v, sizeof(char), 1, file_ptr);
		*dest = v / 256.0f;
	}

	void File::readFloat16(float *dest) {
		if (!readSafeCheck(dest)) return;
		
		unsigned short v=0;
		readInt16(&v);
		unsigned int f=half_to_float(v);
		*dest = *(float*)&(f);
	}

	void File::readFloat16BE(float *dest) {
		if (!readSafeCheck(dest)) return;
		
		unsigned short v=0;
		readInt16BE(&v);
		unsigned int f=half_to_float(v);
		*dest = *(float*)&(f);
	}

	void File::readFloat32(float *dest) {
		if (!readSafeCheck(dest)) return;
		fread(dest, sizeof(int), 1, file_ptr);
	}

	void File::readFloat32BE(float *dest) {
		if (!readSafeCheck(dest)) return;
		fread(dest, sizeof(int), 1, file_ptr);
		endianSwap(*((unsigned int *) dest));
	}

	void File::readUChar(unsigned char *dest) {
		if (!readSafeCheck(dest)) return;
		fread(dest, sizeof(char), 1, file_ptr);
	}

	void File::readString(string *dest) {
		if (!readSafeCheck(dest)) return;
		char c=1;
		*dest = "";
		while (c && !endOfFile()) {
			fread(&c, sizeof(char), 1, file_ptr);
			if (c) *dest += c;
		}
	}


	void File::readString16(u16string *dest) {
		if (!readSafeCheck(dest)) return;
		char16_t c = 1;
		dest->clear();
		while (c && !endOfFile()) {
			fread(&c, sizeof(char16_t), 1, file_ptr);
			if (c) *dest += c;
		}
	}

	bool File::readLine(string *dest) {
		char string_buffer[LIBXENOVERSE_FILE_STRING_BUFFER];
		if (fgets(string_buffer, LIBXENOVERSE_FILE_STRING_BUFFER, file_ptr)) {
			size_t ln = strlen(string_buffer) - 1;
			if (string_buffer[ln] == '\n') {
				string_buffer[ln] = '\0';
			}

			*dest = ToString(string_buffer);
			return true;
		}

		return false;
	}

	void File::readString(string *dest, size_t n) {
		if (!readSafeCheck(dest)) return;
		char c=1;
		*dest = "";
		for (size_t i=0; i<n; i++) {
			fread(&c, sizeof(char), 1, file_ptr);
			if (c) *dest += c;
		}
	}


	void File::readInt16E(unsigned short *dest) {
		if (big_endian) readInt16BE(dest);
		else readInt16(dest);
	}

	void File::readInt32E(unsigned int *dest) {
		if (big_endian) readInt32BE(dest);
		else readInt32(dest);
	}

	void File::readFloat16E(float *dest) {
		if (big_endian) readFloat16BE(dest);
		else readFloat16(dest);
	}

	void File::readFloat32E(float *dest) {
		if (big_endian) readFloat32BE(dest);
		else readFloat32(dest);
	}


	void File::writeInt16E(unsigned short *dest) {
		if (big_endian) writeInt16BE(dest);
		else writeInt16(dest);
	}

	void File::writeInt32E(unsigned int *dest) {
		if (big_endian) writeInt32BE(dest);
		else writeInt32(dest);
	}

	void File::writeFloat16E(float *dest) {
		if (big_endian) writeFloat16BE(dest);
		else writeFloat16(dest);
	}

	void File::writeFloat32E(float *dest) {
		if (big_endian) writeFloat32BE(dest);
		else writeFloat32(dest);
	}

	bool File::write(void *dest, size_t sz) {
		if (!readSafeCheck(dest)) return false;
		return (fwrite(dest, sz, 1, file_ptr) == sz);
	}
	
	void File::writeString(const char *dest) {
		if (!readSafeCheck((void *) dest)) return;

		fwrite(dest, sizeof(char), strlen(dest), file_ptr);
	}

	void File::writeString(string *dest) {
		if (!readSafeCheck(dest)) return;

		if (!dest->size()) writeNull(1);
		else fwrite(dest->c_str(), sizeof(char), dest->size()+1, file_ptr);
	}

	void File::writeUChar(unsigned char *dest) {
		if (!readSafeCheck(dest)) return;

		fwrite(dest, sizeof(char), 1, file_ptr);
	}

	void File::writeInt16(unsigned short *dest) {
		if (!readSafeCheck(dest)) return;

		unsigned short target=*dest;
		fwrite(&target, sizeof(short), 1, file_ptr);
	}

	void File::writeInt16BE(unsigned short *dest) {
		if (!readSafeCheck(dest)) return;

		unsigned short target=*dest;
		endianSwap(target);
		fwrite(&target, sizeof(short), 1, file_ptr);
	}

	void File::writeInt32(unsigned int *dest) {
		if (!readSafeCheck(dest)) return;

		fwrite(dest, sizeof(int), 1, file_ptr);
	}

	void File::writeInt32BE(unsigned int *dest) {
		if (!readSafeCheck(dest)) return;

		unsigned int target=*dest;
		endianSwap(target);
		fwrite(&target, sizeof(int), 1, file_ptr);
	}

	void File::writeFloat32(float *dest) {
		if (!readSafeCheck(dest)) return;
		fwrite(dest, sizeof(float), 1, file_ptr);
	}

	void File::writeFloat32BE(float *dest) {
		if (!readSafeCheck(dest)) return;

		float target=*dest;
		endianSwap(*((unsigned int *) &target));
		fwrite(&target, sizeof(float), 1, file_ptr);
	}

	void File::writeFloat16(float *dest) {
		if (!readSafeCheck(dest)) return;

		unsigned short v = half_from_float(*((uint32_t *)dest));
		writeInt16(&v);
	}

	void File::writeFloat16BE(float *dest) {
		if (!readSafeCheck(dest)) return;

		unsigned short v = half_from_float(*((uint32_t *) dest));
		writeInt16BE(&v);
	}

	void File::writeNull(size_t size) {
		unsigned char zero=0;
		for (size_t i=0; i<size; i++) {
			fwrite(&zero, sizeof(char), 1, file_ptr);
		}
	}

	void File::writeText(const char *dest) {
		fprintf(file_ptr, dest);
	}

	size_t File::fixPadding(size_t multiple) {
		size_t address=getCurrentAddress();
		size_t extra=multiple - (address%multiple);

		if (extra == multiple) {
			extra = 0;
			return 0;
		}

		size_t zero=0;
		for (size_t c=0; c<extra; c++) fwrite(&zero, sizeof(char), 1, file_ptr);
		return extra;
	}

	size_t File::fixPaddingRead(size_t multiple) {
		size_t address=getCurrentAddress();
		size_t extra=multiple - (address%multiple);

		if (extra == multiple) {
			extra = 0;
			return 0;
		}
		goToAddress(address+extra);
		return extra;
	}

	size_t File::getFileSize() {
		if (!file_ptr) {
			return 0;
		}

		size_t address=getCurrentAddress();
		fseek(file_ptr, 0, SEEK_END);
		size_t sz=ftell(file_ptr);
		goToAddress(address);
		return sz;
	}

	bool File::readHeader(string verify_signature, int endianness) {
		if (!file_ptr) {
			return false;
		}

		big_endian = false;
		memset(signature, 0, 8);
		read(signature, 4);

		if (!strcmp(signature, verify_signature.c_str())) {
			if (endianness == -1) {
				unsigned short endian_flag = 0;
				readInt16(&endian_flag);

				if (endian_flag == 0xFEFF) {
					big_endian = true;
				}
				else if (endian_flag == 0xFFFE) {
					big_endian = false;
				}
			}
			else {
				big_endian = (endianness == 1);
			}
			return true;
		}
		else {
			return false;
		}
	}

	void File::writeHeader(string new_signature, bool is_big_endian) {
		if (!file_ptr) {
			return;
		}

		big_endian = is_big_endian;

		memset(signature, 0, 8);
		strcpy(signature, new_signature.c_str());

		write(signature, 4);
		unsigned short endian_flag = (big_endian ? 0xFEFF : 0xFFFE);
		writeInt16(&endian_flag);
	}

	size_t File::getCurrentAddress() {
		if (file_ptr) {
			return ftell(file_ptr)-global_offset;
		}
		else {
			return 0;
		}
	}

	void File::goToAddress(size_t address) {
		if (file_ptr) {
			fseek(file_ptr, address+global_offset, SEEK_SET);
		}
		else {
		}
	}

	void File::moveAddress(size_t address) {
		if (file_ptr) {
			fseek(file_ptr, address, SEEK_CUR);
		}
		else {
		}
	}

	void File::clone(string dest) {
		File file(dest, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			size_t sz=getFileSize();
			char *data=new char[sz];

			read(data, sz);
			file.write(data, sz);

			delete [] data;
			file.close();
		}
	}

	bool File::compare(File *file) {
		if (!file) return false;
		if (getFileSize() != file->getFileSize()) return false;

		unsigned char source=0;
		unsigned char dest=0;

		goToAddress(0);
		file->goToAddress(0);

		while (!endOfFile()) {
			readUChar(&source);
			file->readUChar(&dest);

			if (source != dest) {
				return false;
			}
		}

		return true;
	}


	void File::createComparison(size_t sz) {
		comparison_bytes = new unsigned char[sz];
		comparison_bytes_min = new unsigned char[sz];
		comparison_bytes_max = new unsigned char[sz];
		comparison_size = sz;

		for (size_t i=0; i<comparison_size; i++) {
			comparison_bytes[i] = 0;
			comparison_bytes_min[i] = 255;
			comparison_bytes_max[i] = 0;
		}
	}

	void File::readComparison() {
		read(comparison_bytes, comparison_size);

		for (size_t i=0; i<comparison_size; i++) {
			if (comparison_bytes[i] < comparison_bytes_min[i]) comparison_bytes_min[i] = comparison_bytes[i];
			if (comparison_bytes[i] > comparison_bytes_max[i]) comparison_bytes_max[i] = comparison_bytes[i];
		}
	}

	void File::printComparisonResults() {
		for (size_t i=0; i<comparison_size; i++) {
			printf("Byte #%d Range: [%d, %d]\n", i, (int)comparison_bytes_min[i], (int)comparison_bytes_max[i]);
		}
		getchar();
	}

	void File::deleteComparison() {
		delete [] comparison_bytes;
		delete [] comparison_bytes_min;
		delete [] comparison_bytes_max;
	}

	bool fileCheck(string filename) {
		FILE *fp=fopen(filename.c_str(), "rb");

		if (fp) {
			fclose(fp);
			return true;
		}

		return false;
	}

	string extensionFromFilename(string filename, bool last_dot) {
		string name = filename;
		size_t sep = name.find_last_of("\\/");
		if (sep != std::string::npos) {
			name = name.substr(sep + 1, name.size() - sep - 1);
		}
	
		size_t dot = string::npos;
		if (last_dot) dot = name.find_last_of(".");
		else dot = name.find_first_of(".");

		if (dot != string::npos) {
			name = name.substr(dot+1, name.size()-dot-1);
		}
		else {
			name = "";
		}

		return name;
	}
	
	string nameFromFilename(string filename) {
		string name = filename;
		size_t sep = name.find_last_of("\\/");
		if (sep != std::string::npos) {
			name = name.substr(sep + 1, name.size() - sep - 1);
		}
		return name;
	}

	string nameFromFilenameNoExtension(string filename, bool last_dot) {
		string name = filename;
		size_t sep = name.find_last_of("\\/");
		if (sep != std::string::npos) {
			name = name.substr(sep + 1, name.size() - sep - 1);
		}
	
		size_t dot = string::npos;
		if (last_dot) dot = name.find_last_of(".");
		else dot = name.find_first_of(".");
		
		if (dot != string::npos) {
			name = name.substr(0, dot);
		}

		return name;
	}

	string filenameNoExtension(string filename) {
		string name = filename;

		size_t dot = string::npos;
		dot = name.find_last_of(".");

		if (dot != string::npos) {
			name = name.substr(0, dot);
		}

		return name;
	}

	string folderFromFilename(string filename) {
		size_t sep = filename.find_last_of("\\/");
		if (sep != std::string::npos) {
			return filename.substr(0, sep+1);
		}

		return "";
	}

	char *getBytesFrom(string filename, size_t &data_size) {
		File file(filename, LIBXENOVERSE_FILE_READ_BINARY);

		if (file.valid()) {
			data_size = file.getFileSize();
			char *data = new char[data_size];
			file.read(data, data_size);
			file.close();
			return data;
		}

		data_size = 0;
		return NULL;
	}

	bool writeBytesTo(string filename, char *data, size_t data_size) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.write(data, data_size);
			file.close();
			return true;
		}

		return false;
	}

	bool writeTextTo(string filename, char *data, size_t data_size) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.writeText(data);
			file.close();
			return true;
		}

		return false;
	}
}