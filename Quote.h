#pragma once//Sales_data类的继承体系
#include<string>
#include<iostream>
#include<set>

using std::string;
using std::size_t;
using std::ostream;
using std::multiset;
using std::cout;
using std::endl;

#ifndef QUOTE_H_
#define QUOTE_H_
class Quote {
public:
	Quote() = default;
	Quote(const string& book, double Sales_price) :bookNo(book), price(Sales_price) {}
	Quote(const Quote& q) :bookNo(q.bookNo), price(q.price) { cout << "Quote 拷贝构造函数" << endl; }//拷贝构造函数
	Quote& operator=(const Quote& q) {
		bookNo = q.bookNo;
		price = q.price;
		cout << "Quote拷贝赋值运算符" << endl;
		return *this;
	}//类值拷贝赋值运算符
	string isbn()const { return bookNo; }

	virtual Quote debug() const { cout << "bookNo: " << bookNo << " # price: " << price << endl;
	                        return *this;
	                      }//虚函数
	virtual double net_price(size_t n) const { return price * n; }//虚函数
	virtual Quote* clone() const& { return new Quote(*this); }//返回动态函数当前对象的一份动态分配的拷贝
	virtual Quote* clone() && {return new Quote(std::move(*this)); }//返回动态函数当前对象的一份动态分配的拷贝
	virtual ~Quote() = default;//虚析构函数
private:
	string bookNo;
protected:
	double price = 0.0;
};
#endif

#ifndef DISC_QUOTE_H_
#define DISC_QUOTE_H_

class Disc_quote:public Quote {
public:
	Disc_quote() = default;
	Disc_quote(const string& book, double sales_price, size_t qt, double dis) :Quote(book,sales_price),quantity(qt),discount(dis){}
	Disc_quote(const Disc_quote& dq) :Quote(dq), quantity(dq.quantity), discount(dq.discount) { cout << "Disc_quote拷贝构造函数" << endl; }//拷贝构造函数
	Disc_quote& operator=(const Disc_quote&);
	double net_price(size_t) const = 0;//纯虚函数（含有纯虚函数的类是抽象基类）
	//Disc_quote* clone() const& { return new Disc_quote(*this); }//错误原因是不能创建抽象基类的对象
protected:
	size_t quantity = 0;
	double discount = 0.0;
};
Disc_quote& Disc_quote::operator=(const Disc_quote& dq) {
	Quote::operator=(dq);
	quantity = dq.quantity;
	discount = dq.discount;
	cout << "Disc_quote拷贝赋值运算符" << endl;
	return*this;
}//类值拷贝赋值运算符
#endif

#ifndef BULK_QUOTE_H_
#define BULK_QUOTE_H_

class Bulk_quote:public Disc_quote {//继承自DISC_quote类
public:
	Bulk_quote() = default;
	using Disc_quote::Disc_quote::Disc_quote;
	//Bulk_quote(const string& book, double sales_price, size_t n, double disc) :Disc_quote(book,sales_price,n,disc){}
	Bulk_quote(const Bulk_quote& bq) :Disc_quote(bq) { cout << "Bulk_quote拷贝构造函数" << endl; }//拷贝构造函数
	Bulk_quote& operator=(const Bulk_quote& bq) {
		Disc_quote::operator=(bq);
		cout << "Bulk_quote拷贝赋值运算符" << endl;
		return *this;
	}//类值拷贝赋值运算符
	Quote debug()const override { cout << "bookNo: " << isbn() << " # price" << price << " # min_qty: " << quantity << " # discount: " << discount << endl;
	                              return *this;
	                            }//重载虚函数
	double net_price(size_t) const override;//重载虚函数声明
	Bulk_quote* clone()const& { return new Bulk_quote(*this); }//重载返回当前对象的一份动态分配的拷贝的函数，
	Bulk_quote* clone() && {return new Bulk_quote(std::move(*this)); }//由虚函数作用域关系可以得知在此作用域内，Quote中的clone函数将被覆盖
//private:
	//size_t min_qty = 0;
	//double discount = 0.0;
};

double Bulk_quote::net_price(size_t n) const {//重载虚函数，超过一定数量打折
	if (n >= quantity)
		return n * (1 - discount)*price;
	else
		return n * price;
}
#endif

#ifndef LES_QUOTE_H_
#define LES_QUOTE_H_

class Les_quote :public Disc_quote {//继承自Quote类
public:
	Les_quote() = default;
	Les_quote(const string& book, double sales_price, size_t cnt, double dis) :Disc_quote(book, sales_price, cnt, dis) {}

	Quote debug()const override {
		cout << "bookNo: " << isbn() << " #price: " << price << " #max_qty: " << quantity << " #discount: " << discount << endl;
		return *this;
	}//重载虚函数
	double net_price(size_t)const override;//重载虚函数声明
										   //private:
										   //size_t max_qty = 0;
										   //double discount = 0.0;
};

double Les_quote::net_price(size_t n)const {//重载虚函数，未超出规定数量就打折，超出部分按原价
	if (n <= quantity)
		return n * (1 - discount)*price;
	else
		return quantity * (1 - discount)*price + (n - quantity)*price;
}
#endif

double print_total(ostream& os, const Quote& item, size_t n);

#ifndef BASKET_H_
#define BASKET_H_
class Basket {
public:
	//void add_items(const shared_ptr<Quote> &sale) { items.insert(sale); }
	void add_items(const Quote& sale) { items.insert(std::shared_ptr<Quote>(sale.clone())); }
	void add_items(Quote&& sale) { items.insert(std::shared_ptr<Quote>(std::move(sale).clone())); }
	double total_receipt(ostream&) const;
private:
	static bool compare(const std::shared_ptr<Quote> &lhs, const std::shared_ptr<Quote> &rhs) { return lhs->Quote::isbn()< rhs->Quote::isbn(); }
	multiset<std::shared_ptr<Quote>, decltype(compare)*>items{ compare };//初始化compare并且，并采用compare函数进行比较
};
double Basket::total_receipt(ostream &os)const {
	double sum = 0.0;
	for (auto iter = items.cbegin(); iter != items.cend(); iter = items.upper_bound(*iter)) {//upper_bound返回与iter关键字相等的元素的末尾元素的下一位置
		sum += print_total(os, **iter, items.count(*iter));//iter解引用后是一个shared_ptr,所以需要解两次
	}
	os << "Total sales: " << sum << endl;
	return sum;
}
#endif

double print_total(ostream& os, const Quote& item, size_t n) {
	auto ret = item.net_price(n);
	cout << "Isbn: " << item.isbn() << " # sold: " << n << " total due: " << ret << endl;
	return ret;
}