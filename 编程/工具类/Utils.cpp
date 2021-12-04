#include "Utils.h"
#include <algorithm>

#ifdef __WINDOWS__
#include <Windows.h>
#else
#include <limits.h>
#endif

using namespace molo;

size_t Utils::getExecutablePath(std::string& exepath)
{
#ifdef __WINDOWS__
	char szExe[512] = { 0 };
	GetModuleFileName(NULL, szExe, 512);
	exepath = szExe;
	size_t pos = exepath.find_last_of('\\');
	exepath = exepath.substr(0, pos);
	exepath += '\\';
	return exepath.size();
#else
	char szExe[512] = { 0 };
	if (readlink("/proc/self/exe", szExe, sizeof(szExe)) <= 0)
		return -1;
	exepath = szExe;
	size_t pos = exepath.find_last_of('/');
	exepath = exepath.substr(0, pos);
	exepath += '/';
	return exepath.size();
#endif
}