#include "Utils.h"
#include <algorithm>
#include <sstream>

#ifdef __WINDOWS__
#include <Windows.h>
#else
#include <limits.h>
#include <iconv.h>
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

std::string Utils::gbkToUtf8(const std::string& source)
{
    std::string utf8Str;
    utf8Str.resize(source.length() * 2, 0);
#ifdef WIN32

    int len = MultiByteToWideChar(CP_ACP, 0, source.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, source.c_str(), -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    std::string strTemp = str;
    if (wstr) {
        delete[] wstr;
    }
    if (str) {
        delete[] str;
    }
    return strTemp;
#else
    iconv_t cd;
    cd = iconv_open("UTF-8", "GBK");
    if ((iconv_t) - 1 == cd) {
        printf("iconv_open error");
        return "";
    }

    char* uniCh = const_cast<char*>(source.c_str());
    size_t uniSize = source.length();

    char* utfCh = const_cast<char*>(utf8Str.c_str());
    size_t utfSize = utf8Str.length();
    int retsize = iconv(cd, &uniCh, &uniSize, &utfCh, &utfSize);
    iconv_close(cd);
    if ((size_t) - 1 == retsize) {
        return "";
    }
#endif
    return utf8Str;
}

std::string Utils::utf8ToGbk(const std::string& utf8Str)
{
    std::string unicodeStr;
    unicodeStr.resize(utf8Str.length() * 2, 0);
#ifdef WIN32
    // Convert headers from ASCII to Unicode.
    size_t dwMinSize = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), utf8Str.length(), NULL, 0);
    WCHAR* pWTmp = new WCHAR[dwMinSize + 1];
    memset(pWTmp, 0, dwMinSize * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), utf8Str.length(), pWTmp, dwMinSize);

    dwMinSize = WideCharToMultiByte(CP_ACP, NULL, pWTmp, -1, NULL, 0, NULL, FALSE);
    if (utf8Str.length() < dwMinSize) {
        unicodeStr.resize(dwMinSize, 0);
    }

    char* tmp = const_cast<char*>(unicodeStr.c_str());
    WideCharToMultiByte(CP_ACP, NULL, pWTmp, -1, tmp, unicodeStr.length(), NULL, FALSE);
    delete[]pWTmp;
    pWTmp = NULL;
#else
    iconv_t cd;
    cd = iconv_open("GBK", "UTF-8");
    if ((iconv_t) - 1 == cd) {
        return "";
    }
    char* utf8Ch = const_cast<char*>(utf8Str.c_str());
    size_t utf8Size = utf8Str.length();

    char* uniCh = const_cast<char*>(unicodeStr.c_str());
    size_t uniSize = unicodeStr.length();

    int retsize = iconv(cd, &utf8Ch, &utf8Size, &uniCh, &uniSize);
    iconv_close(cd);
    if (retsize < 0) {
        return "";
    }
#endif
    return unicodeStr;
}

void Utils::splitString(const std::string& src, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(src);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		if(!item.empty())
		{
			elems.push_back(item);
		}
	}
}

void Utils::splitString(const std::string& s, const char* delim, std::vector<std::string>& elems)
{
	if (s.empty()) {
		return;
	}
    elems.clear();                                 // 1.
    char* buffer = new char[s.size() + 1];      // 2.
    buffer[s.size()] = '\0';
    std::copy(s.begin(), s.end(), buffer);      // 3.
    char* p = strtok(buffer, delim);       // 4.
    do {
        elems.push_back(p);                        // 5.
    } while ((p = strtok(NULL, delim)));   // 6.
    delete[] buffer;
}

void Utils::splitString(const std::string& str, const std::string& pattern, std::vector<std::string>& elems)
{
	std::string::size_type pos;
	std::string tmpStr = str + pattern;//扩展字符串以方便操作
	int size = tmpStr.size();

	for (int i = 0; i < size; i++) {
		pos = tmpStr.find(pattern, i);
		if (pos < size) {
			std::string s = tmpStr.substr(i, pos - i);
			elems.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
}

void Utils::split(const std::string &text, std::vector<std::string> &tokens, const std::string &delimiter) {
	std::string::size_type bPos = text.find_first_not_of(delimiter, 0);
	std::string::size_type ePos = text.find_first_of(delimiter, bPos);
	while (std::string::npos != bPos || std::string::npos != ePos) {
		tokens.emplace_back(text.substr(bPos, ePos - bPos));
		bPos = text.find_first_not_of(delimiter, ePos);
		ePos = text.find_first_of(delimiter, bPos);
	}
}

std::string Utils::toLower(const std::string& data)
{
	std::string result(data);
	for(std::string::iterator iter = result.begin(); iter != result.end(); ++iter)
	{
		if(*iter >= 'A' && *iter <= 'Z')
		{
			*iter = *iter - ('A' - 'a');
		}
	}

	return result;
}

std::string Utils::toUpper(const std::string& data)
{
	std::string result(data);
	for(std::string::iterator iter = result.begin(); iter != result.end(); ++iter)
	{
		if(*iter >= 'a' && *iter <= 'z')
		{
			*iter = *iter + ('A' - 'a');
		}
	}

	return result;
}