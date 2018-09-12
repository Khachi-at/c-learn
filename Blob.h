#pragma once
#include<vector>
#include<memory>
#include<string>
#include<initializer_list>
using std::initializer_list;
using std::shared_ptr;
using std::vector;
using std::string;
//定义Blob,string管理类的模板，使用了一个vector来保存对象，使用shared_ptr来管理内存
template<typename> class BlobPtr;
template<typename> class Blob;
template<class T>class Blob {
	friend class BlobPtr<T>;
	friend bool operator==(const Blob<T>&,const Blob<T>&);
	friend bool operator != (const Blob<T>&, const Blob<T>&);
public:
	typedef T value_type;
	typedef typename vector<T>::size_type size_type;
	Blob();
	Blob(initializer_list<T> i1);
	template<class It> Blob(It b, It e);//成员模板（接受两个迭代器的构造函数）
	Blob(const Blob& bl) :data(std::make_shared<vector<T>>((*b1).data)) {}//拷贝构造函数
	Blob& operator=(const Blob &bl);//拷贝赋值运算符
	size_type size()const { return data->size(); }
	bool empty()const { return data->empty; }
	void push_back(const T &t) { data->push_back(t); }
	void push_back(T &&t) { data->push_back(std::move(t)); }
	void pop_back()const ;
	T& front() const;
	T& back() const;
	T& operator[](size_type i)const;
private:
	shared_ptr<vector<T>> data;
	void check(size_type i, const string &msg) const;
};
template<class T>
bool operator==(const Blob<T>&, const Blob<T>&);
template<class T>
bool operator != (const Blob<T>&, const Blob<T>&);
//非成员函数声明

template<class T>//check成员，检查一个给定的索引
void Blob<T>::check(size_type i, const string &msg) const {
	if (i >= data->size())
		throw std::out_of_range(msg);
}
template<class T>//back成员函数
T& Blob<T>::back() const{
	check(0, "back on empty Blob!");
	return data->back;
}
template<class T>//下标运算符
T& Blob<T>::operator[](size_type i)const {
	check(i, "subscript out of range!");
	return (*data)[i];
}
template<class T>//定义pop_back成员函数
void Blob<T>::pop_back()const {
	check(0, "pop_back on empty Blob!");
	return data->pop_back;
}
template<class T>//构造函数
Blob<T>::Blob():data(std::make_shared<std::vector<T>>()) {}
template<class T>//接受一个initializer_list的构造函数
Blob<T>::Blob(initializer_list<T> i1) : data(std::make_shared<vector<T>>(i1)) {}
template<class T>//拷贝赋值运算符
Blob<T>& Blob<T>::operator=(const Blob &bl) {
	data = std::make_shared<vector<T>>(*bl.data);
	return *this;
}
template<class T>//front成员函数
T& Blob<T>::front() const {
	check(0, "front on empty Blob!");
	return data->front;
}
template<class T>//== 重载
bool operator==(const Blob<T>& b1, const Blob<T>& b2) {
	return b1.data == b2.data;
}
template<class T>//!=重载
bool operator != (const Blob<T>&, const Blob<T>&) {
	return !(b1 == b2);
}
template<class T>//成员模板（接受两个迭代器的构造函数）
template<class It>
Blob<T>::Blob(It b, It e):data(std::make_shared<vector<T>>(b,e)) {}



//使用动态内存进行内存管理，管理string的类模板
template<class T>class BlobPtr {
	friend BlobPtr& operator+(const BlobPtr&, const BlobPtr&);
	friend BlobPtr& operator -(const BlobPtr&, const BlobPtr&);
	friend bool operator==(const BlobPtr&, const BlobPtr&);
	friend bool operator!=(const BlobPtr&, const BlobPtr&);
public:
	BlobPtr() :curr(0) {}
	BlobPtr(Blob<T> &a, size_t sz = 0) :wptr(a.data), curr(sz) {}

	T& operator[](std::size_t n) const { return wptr.lock[n]; }//下标运算符
	T& operator*() const {//解引用运算符
		auto p = check(curr, "derefence past end!");
		return (*p)[curr];
	}
	T* operator->()const { return &this->operator*(); }//箭头运算符
	BlobPtr& operator++();//前置递增运算符
	BlobPtr& operator--();//前置递减运算符
	BlobPtr operator++(int);//后置递增运算符
	BlobPtr operator--(int);//后置递减运算符
private:
	shared_ptr<vector<T>> check(std::size_t, const string&)const;
	std::weak_ptr<vector<T>> wptr;//弱指针，不会改变shared_ptr的对象的引用计数
	std::size_t curr;
};
template<class T>
BlobPtr<T>& operator+(const BlobPtr<T>&, const BlobPtr<T>&);
template<class T>
BlobPtr<T>& operator -(const BlobPtr<T>&, const BlobPtr<T>&);
template<class T>
bool operator==(const BlobPtr<T>&, const BlobPtr<T>&);
template<class T>
bool operator!=(const BlobPtr<T>&, const BlobPtr<T>&);
//非成员函数声明

template<class T>//定义check函数
shared_ptr<vector<T>> BlobPtr<T>::check(std::size_t i, const std::string& msg) const {
	auto ret = wptr.lock();
	if (!ret)
		throw std::runtime_error("unbound StrBlobPtr");
	if (i >= ret->size())
		throw std::out_of_range(msg);
	return ret;
}
template<class T>//前置递增运算符
BlobPtr<T>& BlobPtr<T>::operator++() {
	check(curr, "increment past end of BlobPtr");
	++curr;
	return *this;
}
template<class T>//前置递减运算符
BlobPtr<T>& BlobPtr<T>::operator--() {
	--curr;
	check(curr, "decrement past begin of BlobPtr!");
	return *this;
}
template<class T>//后置递增运算符
BlobPtr<T> BlobPtr<T>::operator++(int) {
	BlobPtr ret = *this;
	++*this;
	return ret;
}
template<class T>//后置递减运算符
BlobPtr<T> BlobPtr<T>::operator--(int) {
	BlobPtr ret = *this;
	--*this;
	return ret;
}
template<class T>//+运算符
BlobPtr<T>& operator+(const BlobPtr<T> &bp1, const BlobPtr<T> &bp2) {
	BlobPtr<T> bp;
	for (auto i : *(bp1.wptr))
		*(bp.wptr).push_back(i);
	for (auto i : *(bp2.wptr))
		*(bp.wptr).push_back(i);
	return bp;
}
template<class T>//-运算符
BlobPtr<T>& operator -(const BlobPtr<T> &bp1, const BlobPtr<T> &bp2) {
	for (auto iter = *(bp1.wptr).begin(); iter != *(bp1.wptr).end(); ++iter) {
		for (auto it = *(bp2.wptr).begin(); it != *(bp2.wptr).end(); ++it)
		{
			if (*it == *iter)
				*(bp1.wptr).erase(iter);
		}
	}
}
template<class T>//==运算符
bool operator==(const BlobPtr<T> &bp1, const BlobPtr<T> &bp2) {
	return *(bp1.wptr) == *(bp2.wptr) && bp1.curr == bp2.curr;
}
template<class T>//!=运算符
bool operator!=(const BlobPtr<T> &bp1, const BlobPtr<T> &bp2) {
	return !(bp1 == bp2);
}