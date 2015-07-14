#pragma once
//
//《Effetice C++》 第6条 37页《若不想使用编译器自动生成的函数，就应该明确拒绝》
//
namespace util {

namespace non_copyable_
{

class noncopyable
{
private:
	noncopyable(const noncopyable&);
	noncopyable& operator = (const noncopyable&);
protected:
	noncopyable(){};
	~noncopyable(){};
};
}

using noncopyable = non_copyable_::noncopyable;

}
