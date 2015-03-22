// Common Includes
#include "stdio.h"
#include <vector>
#include "half.h"
#include <sstream>
#include <iostream>
#include <locale>
#include <string>
#include <cvt/wstring>
#include "windows.h"

// TinyXML
#include "tinyxml/tinyxml.h"

// FBX Includes
#ifdef LIBXENOVERSE_FBX_SUPPORT
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#endif

#ifdef LIBXENOVERSE_ZLIB_SUPPORT
// ZLIB Includes
#include "zlib.h"
#endif

#ifdef LIBXENOVERSE_WINSDK_SUPPORT
#include "D3Dcompiler.h"
#include <d3d9.h>
#include <d3dx9.h>
#endif

// Xenoverse Includes
#include "XenoFiles.h"
#include "XenoMath.h"
#include "HLSLASM.h"
#include "AGD.h"
#include "BCS.h"
#include "CBS.h"
#include "CUS.h"
#include "EAN.h"
#include "EMB.h"
#include "EMD.h"
#include "EMM.h"
#include "EMZ.h"
#include "ESK.h"
#include "MSG.h"

using namespace std;

#ifndef LIBXENOVERSE_H_INCLUDED
#define LIBXENOVERSE_H_INCLUDED

//#define LIBXENOVERSE_DEBUGGING_LOG

#ifdef LIBXENOVERSE_DEBUGGING_LOG
#define LOG_DEBUG(fmt, ...) if (global_debugging_log) fprintf(global_debugging_log, fmt, __VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

namespace LibXenoverse {
	extern FILE *global_debugging_log;

	void initializeDebuggingLog();
	void flushDebuggingLog();
	void closeDebuggingLog();
}

#endif