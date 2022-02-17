#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>

namespace molo {
class Utils
{
public:
	//获取当前执行程序路径
	static size_t getExecutablePath(std::string& exepath);
	//gbk与utf8转换
	static std::string gbkToUtf8(const std::string& utf16Str);
    static std::string utf8ToGbk(const std::string& utf8Str);
	
	//字符串str使用分隔符delim进行分割，得到tokens列表
	static void splitString(const std::string& str, char delim, std::vector<std::string>& elems);
	static void splitString(const std::string& str, const char* delim, std::vector<std::string>& elems);
	static void splitString(const std::string& str, const std::string& delim, std::vector<std::string>& elems);
	
	// 字符串转大小写
	static std::string toLower(const std::string& data);
	static std::string toUpper(const std::string& data);
	
};
}// namespace molo

#endif // _UTILS_H_