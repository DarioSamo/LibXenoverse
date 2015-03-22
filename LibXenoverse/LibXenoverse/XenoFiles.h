#ifndef LIBXENOVERSE_FILE_H_INCLUDED
#define LIBXENOVERSE_FILE_H_INCLUDED

#define LIBXENOVERSE_FILE_H_ERROR_READ_FILE_BEFORE   "Can't open the specified file: "
#define LIBXENOVERSE_FILE_H_ERROR_READ_FILE_AFTER    ". The file doesn't exist, or is currently open by another program."
#define LIBXENOVERSE_FILE_H_ERROR_FILE_CLOSING       "Attempted to close a file with an undefined reference."
#define LIBXENOVERSE_FILE_H_ERROR_FILE_HEADER        "Attempted to read the header of a file with an undefined reference."
#define LIBXENOVERSE_FILE_H_ERROR_FILE_BOOKMARK      "Attempted to go to address on a file with an undefined reference."
#define LIBXENOVERSE_FILE_H_ERROR_FILE_GET_ADDRESS   "Attempted to get address from a file with an undefined reference."
#define LIBXENOVERSE_FILE_H_ERROR_READ_NULL          "Attempted to read variable from file, but the destination is undefined."
#define LIBXENOVERSE_FILE_H_ERROR_READ_FILE_NULL     "Attempted to read variable from an file with an undefined reference."

#define LIBXENOVERSE_FILE_HEADER_ROOT_TYPE_ADDRESS           4
#define LIBXENOVERSE_FILE_HEADER_ROOT_NODE_ADDRESS           12

#define LIBXENOVERSE_FILE_HEADER_ROOT_ADDRESS_DEFAULT        24
#define LIBXENOVERSE_FILE_HEADER_ROOT_ADDRESS_NEXT_GEN       16

#define LIBXENOVERSE_FILE_STRING_BUFFER                      1024

#define LIBXENOVERSE_FILE_READ_TEXT                          "rt"
#define LIBXENOVERSE_FILE_WRITE_TEXT                         "wt"
#define LIBXENOVERSE_FILE_READ_BINARY                        "rb"
#define LIBXENOVERSE_FILE_WRITE_BINARY                       "wb"

#define LIBXENOVERSE_LITTLE_ENDIAN                           0
#define LIBXENOVERSE_BIG_ENDIAN                              1

#include "stdio.h"
#include <string>
#include <list>

using namespace std;

namespace LibXenoverse {
	void endianSwap(unsigned int& x);
	void endianSwap(int& x);

	template <class internT, class externT, class stateT>
	struct codecvt : std::codecvt<internT, externT, stateT>
	{
		~codecvt(){}
	};

	extern wstring_convert<codecvt<char16_t, char, mbstate_t>, char16_t> convert16;

	class File {
		protected:
			FILE *file_ptr;
			string name;
			string path;
			size_t global_offset;

			unsigned char *comparison_bytes;
			unsigned char *comparison_bytes_min;
			unsigned char *comparison_bytes_max;
			size_t comparison_size;

			char signature[8];
			bool big_endian;
		public:
			File(string filename, string mode);
			void writeHeader(string new_signature, bool is_big_endian);
			bool readHeader(string verify_signature, int endianness=-1);

			void setGlobalOffset(size_t v) {
				global_offset = v;
				goToAddress(0);
			}

			void goToAddress(size_t address);
			void goToEnd() {
				fseek(file_ptr, 0L, SEEK_END);
			}
			void moveAddress(size_t address);
			size_t getCurrentAddress();


			string getPath() {
				return path;
			}

			// BE: Big Endian
			// BEA: Big Endian Address (Offset by Root Node automatically)
			bool readSafeCheck(void *dest);
			void read(void *dest, size_t sz);
			void readInt16(unsigned short *dest);
			void readInt16BE(unsigned short *dest);
			void readInt32(unsigned int *dest);
			void readInt32BE(unsigned int *dest);
			void readFloat8(float *dest);
			void readFloat16(float *dest);
			void readFloat16BE(float *dest);
			void readFloat32(float *dest);
			void readFloat32BE(float *dest);
			void readUChar(unsigned char *dest);
			void readString(string *dest);
			void readString(string *dest, size_t n);
			void readString16(u16string *dest);

			void readInt16E(unsigned short *dest);
			void readInt32E(unsigned int *dest);
			void readInt32EA(size_t *dest);
			void readFloat16E(float *dest);
			void readFloat32E(float *dest);

			bool readLine(string *dest);

			void write(void *dest, size_t sz);
			void writeString(const char *dest);
			void writeString(string *dest);
			void writeUChar(unsigned char *dest);
			void writeInt16(unsigned short *dest); 
			void writeInt16BE(unsigned short *dest);
			void writeInt32(unsigned int *dest);
			void writeInt32A(size_t *dest, bool add_to_table=true);
			void writeInt32BE(unsigned int *dest);
			void writeFloat32(float *dest);
			void writeFloat32BE(float *dest);
			void writeFloat16(float *dest);
			void writeFloat16BE(float *dest);
			void writeNull(size_t size);
			void writeText(const char *dest);

			void writeInt16E(unsigned short *dest);
			void writeInt32E(unsigned int *dest);
			void writeFloat16E(float *dest);
			void writeFloat32E(float *dest);

			size_t fixPadding(size_t multiple=4);
			size_t fixPaddingRead(size_t multiple=4);
			size_t getFileSize();

			FILE *getPointer() {
				return file_ptr;
			}

			// Reverse-Engineering functions, just to track down what some values could be by printing their minimum and maximum values
			void createComparison(size_t sz);
			void readComparison();
			void printComparisonResults();
			void deleteComparison();


			int endOfFile();
			bool valid();
			bool compare(File *file);
			void close();
			void clone(string dest);
	};

	bool fileCheck(string filename);
	string extensionFromFilename(string filename, bool last_dot=false);
	string nameFromFilename(string filename);
	string nameFromFilenameNoExtension(string filename, bool last_dot=false);
	string folderFromFilename(string filename);
	string filenameNoExtension(string filename);
	char *getBytesFrom(string filename, size_t &data_size);
	bool writeBytesTo(string filename, char *data, size_t data_size);
	bool writeTextTo(string filename, char *data, size_t data_size);
};

#endif