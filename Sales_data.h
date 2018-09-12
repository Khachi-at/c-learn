#pragma once//图书销售管理的类，主要是记录图书的销售纪录，并对其进行一些运算（销售金额，数量等等，还对销售记录进行了+、-等的重载）
#include<cstring>
#include<string>
#include<iostream>

#ifndef SALES_DATA_H
#define SALES_DATA_H
using std::string;
using std::istream;
using std::ostream;
using std::cout;

class Sales_data {
	friend ostream& operator<<(ostream&,const Sales_data&);//重载输入运算符
	friend istream& operator>>(istream&, Sales_data&);//重载输入输出运算符
	friend istream&read(istream &is,Sales_data &item);
	friend ostream&print(ostream &os,const Sales_data &item);
	friend Sales_data& operator+(const Sales_data&,const Sales_data&);
	friend Sales_data& operator-(const Sales_data&, const Sales_data&);
public:
	//Sales_data() = default;//默认构造函数
	Sales_data() :Sales_data("", 0, 0) {}//委托构造函数
	//Sales_data(const string &s) :bookNo(s) {};
	Sales_data(const string &s) :Sales_data(s, 0, 0) {}//委托构造函数
	Sales_data(const string &s, unsigned n, double p) :
		bookNo(s), units_sold(n), revenue(p*n) {}
	Sales_data(istream &is) :Sales_data() { read(is, *this); }//委托构造函数
	inline string isbn()const {
		return bookNo;
	}//常量成员函数等价于std::string Sales_data::isbn(const Sales_data const* this) {}
	Sales_data& combine(const Sales_data &);
	bool compareIsbn(const Sales_data &, const Sales_data&);
	Sales_data& operator=(const Sales_data&);//拷贝赋值运算符
	Sales_data& operator=(const string&);
	Sales_data& operator+=(const Sales_data&);//重载复合赋值运算符
	Sales_data& operator-=(const Sales_data&);
	explicit operator string() const { return bookNo; }//显示的类型转换运算符
	explicit operator double() const { return revenue; }//显示的类型转换运算符
private:
	double avg_price() const;

	std::string bookNo;
	unsigned units_sold = 0;
	double revenue=0.0;
};
Sales_data bookNo,units_sold,revenue;

//非成员函数的声明
Sales_data& operator-(const Sales_data&, const Sales_data&);
istream& operator>>(istream&, Sales_data&);
ostream& operator<<(ostream&, const Sales_data&);
Sales_data add(const Sales_data&, const Sales_data&);
std::ostream &print(std::ostream&, const Sales_data&);
std::istream &read(std::istream&, Sales_data&);
Sales_data& operator+(const Sales_data&,const Sales_data&);

//运算符重载
Sales_data& Sales_data::operator=(const string& str) {
	Sales_data s;
	s.bookNo = str;
	return s;
}
Sales_data& operator-(const Sales_data& s1, const Sales_data& s2) {
	if (s1.bookNo == s2.bookNo) {
		auto num = s1;
		num -= s2;
		return num;
	}
	else
		cout << "请输入相同的Isbn" << std::endl;
}
Sales_data& Sales_data::operator-=(const Sales_data& str) {
	if (bookNo == str.bookNo) {
		units_sold -= str.units_sold;
		revenue -= str.revenue;
		return *this;
	}
	else
		cout << "Isbn不一致，请重新输入Isbn" << std::endl;
}
istream& operator>>(istream& is, Sales_data& str) {//重载输入运算符
	double price;
	is >> str.bookNo>>str.units_sold>>price;
	if (is)
		str.revenue = str.units_sold*price;
	else
		str = Sales_data();
	return is;
}
ostream& operator<<(ostream& os, const Sales_data& str) {
	os << str.bookNo<<" "<<str.units_sold<<" "<<str.revenue<<" "<<str.avg_price();
	return os;
}
Sales_data& Sales_data::operator+=(const Sales_data& sd) {//重载复合赋值运算符
	if (bookNo == sd.bookNo) {
		units_sold += sd.units_sold;
		revenue += sd.revenue;
		return *this;
	}
	else
		cout << "请输入相同的Isbn" << std::endl;
}
Sales_data& operator+(const Sales_data& sd1,const Sales_data& sd2) {
	Sales_data sd;
	if (sd1.bookNo == sd2.bookNo) {
		sd.bookNo = sd1.bookNo;
		sd.units_sold = sd1.units_sold + sd2.units_sold;
		sd.revenue = sd1.revenue + sd2.revenue;
		return sd;
	}
	else 
		cout << "请输入相同的Isbn" << std::endl;
}
//成员函数的定义
inline double Sales_data::avg_price() const {
if (units_sold)
return revenue / units_sold;
else
return 0;
}
Sales_data& Sales_data::combine(const Sales_data &rhs) {
units_sold += rhs.units_sold;
revenue += rhs.revenue;
return *this;
}
istream &read(istream &is,Sales_data &item) {
	double price = 0;
	is >> item.bookNo >> item.units_sold >> price;
	item.revenue = price * item.units_sold;
	return is;
}
//打印，输出函数
ostream &print(ostream &os,const Sales_data &item) {
	os << item.isbn() << " " << item.units_sold << " " << item.revenue << " " << item.avg_price();
	return os;
}
Sales_data add(const Sales_data &lhs,const Sales_data &rhs) {
	Sales_data sum = lhs;
	sum.combine(rhs);
	return sum;
}
bool Sales_data::compareIsbn(const Sales_data &s1,const Sales_data&s2) {
	return(s1.isbn()<s2.isbn());
}
Sales_data& Sales_data::operator=(const Sales_data& sd) {//拷贝赋值运算符
	bookNo = sd.bookNo;
	units_sold = sd.units_sold;
	revenue = sd.revenue;
	return *this;
}
#endif