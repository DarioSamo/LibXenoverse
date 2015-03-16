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

// Xenoverse Includes
#include "XenoFiles.h"
#include "EMB.h"
#include "EMD.h"
#include "EMM.h"
#include "ESK.h"
#include "CBS.h"
#include "CUS.h"
#include "AGD.h"
#include "MSG.h"
#include "XenoMath.h"

using namespace std;

#ifndef LIBXENOVERSE_H_INCLUDED
#define LIBXENOVERSE_H_INCLUDED

//#define LIBXENOVERSE_DEBUGGING_LOG

namespace LibXenoverse {
	extern FILE *global_debugging_log;

	void initializeDebuggingLog();
	void flushDebuggingLog();
	void closeDebuggingLog();
}

#endif