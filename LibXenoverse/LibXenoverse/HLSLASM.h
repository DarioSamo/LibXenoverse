#ifdef LIBXENOVERSE_WINSDK_SUPPORT

#ifndef LIBXENOVERSE_HLSLASM_H_INCLUDED
#define LIBXENOVERSE_HLSLASM_H_INCLUDED

#define LIBXENOVERSE_XPU_EXTENSION    ".xpu"
#define LIBXENOVERSE_XVU_EXTENSION    ".xvu"

namespace LibXenoverse {
	class HLSLASM {
	public:
		/** Compile an HLSL Vertex/Pixel shader from either a text file 
			or an array of bytes containing said text.

			@return: array of bytes containing the new assembled shader
		*/
		static char *assembleFromFile(string filename, size_t &data_size);
		static char *assemble(char *data, size_t &data_size);

		/** Dissassemble an HLSL Vertex/Pixel shader from either a text file
			or an array of bytes containing the assembled shaders.

			@return: array of bytes containing the new disassembled shader
			in text form.
		*/
		static char *disassembleFromFile(string filename, size_t &data_size);
		static char *disassemble(char *data, size_t &data_size);
	};
}

#endif

#endif