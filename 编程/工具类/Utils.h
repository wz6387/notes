#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

namespace molo {
class Utils
{
public:
	static size_t getExecutablePath(std::string& exepath);
};
}// namespace molo

#endif // _UTILS_H_