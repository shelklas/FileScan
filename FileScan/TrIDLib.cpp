#include "TrIDLib.h"

int __stdcall trid::Analyze()
{
	return pAnalyze();
}
int __stdcall trid::GetInfo(int infoType, int infoIdx, char *out)
{
	return pGetInfo(infoType, infoIdx, out);
}
int __stdcall trid::LoadDefsPack(char *fileName)
{
	return pLoadDefsPack(fileName);
}
int __stdcall trid::SubmitFileA(char *fileName)
{
	return pSubmitFileA(fileName);
}
trid::trid(char *defsPath, char *tridLibDllPath)
{
	hm = LoadLibraryA(tridLibDllPath ? tridLibDllPath : "TrIDLib.dll");
	if (!hm)
		throw TRID_MISSING_LIBRARY;

	pAnalyze       = (int(__stdcall *)())					 GetProcAddress(hm, "TrID_Analyze");
	pGetInfo       = (int(__stdcall *)(int, int, char *))    GetProcAddress(hm, "TrID_GetInfo");
	pLoadDefsPack  = (int(__stdcall *)(char *))				 GetProcAddress(hm, "TrID_LoadDefsPack");
	pSubmitFileA   = (int(__stdcall *)(char *))				 GetProcAddress(hm, "TrID_SubmitFileA");

	if (!(pAnalyze && pGetInfo && pLoadDefsPack && pSubmitFileA))
		throw TRID_MISSING_LIBRARY;

	if (defsPath)
		LoadDefsPack(defsPath);
}
trid::~trid()
{
	FreeLibrary(hm);
}
