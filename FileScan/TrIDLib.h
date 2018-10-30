#pragma once

#include <windows.h>

#define TRID_GET_RES_NUM          1     // Get the number of results available
#define TRID_GET_RES_FILETYPE     2     // Filetype descriptions
#define TRID_GET_RES_FILEEXT      3     // Filetype extension
#define TRID_GET_RES_POINTS       4     // Matching points

#define TRID_GET_VER              1001  // TrIDLib version (major * 100 + minor)
#define TRID_GET_DEFSNUM          1004  // Number of filetypes definitions loaded


// Additional constants for the full version

#define TRID_GET_DEF_ID           100   // Get the id of the filetype's definition for a given result
#define TRID_GET_DEF_FILESCANNED  101   // Various info about that def
#define TRID_GET_DEF_AUTHORNAME   102   //     "
#define TRID_GET_DEF_AUTHOREMAIL  103   //     "
#define TRID_GET_DEF_AUTHORHOME   104   //     "
#define TRID_GET_DEF_FILE         105   //     "
#define TRID_GET_DEF_REMARK       106   //     "
#define TRID_GET_DEF_RELURL       107   //     "
#define TRID_GET_DEF_TAG          108   //     "
#define TRID_GET_DEF_MIMETYPE     109   //     "

#define TRID_GET_ISTEXT           1005  // Check if the submitted file is text or binary one

#define TRID_MISSING_LIBRARY      12321

class trid
{

	int (__stdcall *pAnalyze)();
	int (__stdcall *pGetInfo)(int infoType, int infoIdx, char *out);
	int (__stdcall *pLoadDefsPack)(char *fileName);
	int (__stdcall *pSubmitFileA)(char *fileName);

	HMODULE hm;

public:
	int __stdcall Analyze();
	int __stdcall GetInfo(int infoType, int infoIdx, char *out);
	int __stdcall LoadDefsPack(char *fileName);
	int __stdcall SubmitFileA(char *fileName);
	trid(char *defsPath=0, char *tridLibDllPath=0);
	~trid();
};