//如果希望类不能复制，就从这个类派生即可
class NonCopyable
{
protected:
	NonCopyable() = default;
	~NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;//禁用复制构造函数
	NonCopyable& operate = (const NonCopyable&) = delete;//禁用赋值构造函数
};