#pragma once
#include<vector>
#include<memory>
#include<string>
#include<initializer_list>
using std::initializer_list;
using std::shared_ptr;
using std::vector;
using std::string;
//����Blob,string�������ģ�壬ʹ����һ��vector���������ʹ��shared_ptr�������ڴ�
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
	template<class It> Blob(It b, It e);//��Աģ�壨���������������Ĺ��캯����
	Blob(const Blob& bl) :data(std::make_shared<vector<T>>((*b1).data)) {}//�������캯��
	Blob& operator=(const Blob &bl);//������ֵ�����
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
//�ǳ�Ա��������

template<class T>//check��Ա�����һ������������
void Blob<T>::check(size_type i, const string &msg) const {
	if (i >= data->size())
		throw std::out_of_range(msg);
}
template<class T>//back��Ա����
T& Blob<T>::back() const{
	check(0, "back on empty Blob!");
	return data->back;
}
template<class T>//�±������
T& Blob<T>::operator[](size_type i)const {
	check(i, "subscript out of range!");
	return (*data)[i];
}
template<class T>//����pop_back��Ա����
void Blob<T>::pop_back()const {
	check(0, "pop_back on empty Blob!");
	return data->pop_back;
}
template<class T>//���캯��
Blob<T>::Blob():data(std::make_shared<std::vector<T>>()) {}
template<class T>//����һ��initializer_list�Ĺ��캯��
Blob<T>::Blob(initializer_list<T> i1) : data(std::make_shared<vector<T>>(i1)) {}
template<class T>//������ֵ�����
Blob<T>& Blob<T>::operator=(const Blob &bl) {
	data = std::make_shared<vector<T>>(*bl.data);
	return *this;
}
template<class T>//front��Ա����
T& Blob<T>::front() const {
	check(0, "front on empty Blob!");
	return data->front;
}
template<class T>//== ����
bool operator==(const Blob<T>& b1, const Blob<T>& b2) {
	return b1.data == b2.data;
}
template<class T>//!=����
bool operator != (const Blob<T>&, const Blob<T>&) {
	return !(b1 == b2);
}
template<class T>//��Աģ�壨���������������Ĺ��캯����
template<class It>
Blob<T>::Blob(It b, It e):data(std::make_shared<vector<T>>(b,e)) {}



//ʹ�ö�̬�ڴ�����ڴ��������string����ģ��
template<class T>class BlobPtr {
	friend BlobPtr& operator+(const BlobPtr&, const BlobPtr&);
	friend BlobPtr& operator -(const BlobPtr&, const BlobPtr&);
	friend bool operator==(const BlobPtr&, const BlobPtr&);
	friend bool operator!=(const BlobPtr&, const BlobPtr&);
public:
	BlobPtr() :curr(0) {}
	BlobPtr(Blob<T> &a, size_t sz = 0) :wptr(a.data), curr(sz) {}

	T& operator[](std::size_t n) const { return wptr.lock[n]; }//�±������
	T& operator*() const {//�����������
		auto p = check(curr, "derefence past end!");
		return (*p)[curr];
	}
	T* operator->()const { return &this->operator*(); }//��ͷ�����
	BlobPtr& operator++();//ǰ�õ��������
	BlobPtr& operator--();//ǰ�õݼ������
	BlobPtr operator++(int);//���õ��������
	BlobPtr operator--(int);//���õݼ������
private:
	shared_ptr<vector<T>> check(std::size_t, const string&)const;
	std::weak_ptr<vector<T>> wptr;//��ָ�룬����ı�shared_ptr�Ķ�������ü���
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
//�ǳ�Ա��������

template<class T>//����check����
shared_ptr<vector<T>> BlobPtr<T>::check(std::size_t i, const std::string& msg) const {
	auto ret = wptr.lock();
	if (!ret)
		throw std::runtime_error("unbound StrBlobPtr");
	if (i >= ret->size())
		throw std::out_of_range(msg);
	return ret;
}
template<class T>//ǰ�õ��������
BlobPtr<T>& BlobPtr<T>::operator++() {
	check(curr, "increment past end of BlobPtr");
	++curr;
	return *this;
}
template<class T>//ǰ�õݼ������
BlobPtr<T>& BlobPtr<T>::operator--() {
	--curr;
	check(curr, "decrement past begin of BlobPtr!");
	return *this;
}
template<class T>//���õ��������
BlobPtr<T> BlobPtr<T>::operator++(int) {
	BlobPtr ret = *this;
	++*this;
	return ret;
}
template<class T>//���õݼ������
BlobPtr<T> BlobPtr<T>::operator--(int) {
	BlobPtr ret = *this;
	--*this;
	return ret;
}
template<class T>//+�����
BlobPtr<T>& operator+(const BlobPtr<T> &bp1, const BlobPtr<T> &bp2) {
	BlobPtr<T> bp;
	for (auto i : *(bp1.wptr))
		*(bp.wptr).push_back(i);
	for (auto i : *(bp2.wptr))
		*(bp.wptr).push_back(i);
	return bp;
}
template<class T>//-�����
BlobPtr<T>& operator -(const BlobPtr<T> &bp1, const BlobPtr<T> &bp2) {
	for (auto iter = *(bp1.wptr).begin(); iter != *(bp1.wptr).end(); ++iter) {
		for (auto it = *(bp2.wptr).begin(); it != *(bp2.wptr).end(); ++it)
		{
			if (*it == *iter)
				*(bp1.wptr).erase(iter);
		}
	}
}
template<class T>//==�����
bool operator==(const BlobPtr<T> &bp1, const BlobPtr<T> &bp2) {
	return *(bp1.wptr) == *(bp2.wptr) && bp1.curr == bp2.curr;
}
template<class T>//!=�����
bool operator!=(const BlobPtr<T> &bp1, const BlobPtr<T> &bp2) {
	return !(bp1 == bp2);
}