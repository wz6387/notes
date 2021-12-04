#ifndef _GUARD_H_
#define _GUARD_H_
#include <functional>

namespace molo {
class FuncGuard
{
public:
	typedef std::function<void()> Func;

	FuncGuard(const Func& func)
		: func_(func) {}

	~FuncGuard() {
		func_();
	}
private:
	Func func_;
};
//usage: FuncGuard guardImgMem(std::bind(&FreeImage_CloseMemory, imgMem));


template<typename T>
class ArrayGuard {
public:
	ArrayGuard(T* p) {
		std::shared_ptr<T> array(p, std::default_delete<T[]>());
		ptr = std::move(array);
	}
	~ArrayGuard() {}
private:
	std::shared_ptr<T> ptr;
};
//usage: int* p = new int[256]; ArrayGuard<int> guard(p);

template<typename T>
class ObjGuard {
public:
	ObjGuard(T* p) {
		std::shared_ptr<T> pObj(p, std::default_delete<T>());
		ptr = std::move(pObj);
	}
	~ObjGuard() {}
private:
	std::shared_ptr<T> ptr;
};
//usage: int* p = new int; ObjGuard<int> guard(p);

} //namespace molo

#endif // _FUNC_GUARD_H