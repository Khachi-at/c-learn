#pragma once//Sales_data��ļ̳���ϵ
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
	Quote(const Quote& q) :bookNo(q.bookNo), price(q.price) { cout << "Quote �������캯��" << endl; }//�������캯��
	Quote& operator=(const Quote& q) {
		bookNo = q.bookNo;
		price = q.price;
		cout << "Quote������ֵ�����" << endl;
		return *this;
	}//��ֵ������ֵ�����
	string isbn()const { return bookNo; }

	virtual Quote debug() const { cout << "bookNo: " << bookNo << " # price: " << price << endl;
	                        return *this;
	                      }//�麯��
	virtual double net_price(size_t n) const { return price * n; }//�麯��
	virtual Quote* clone() const& { return new Quote(*this); }//���ض�̬������ǰ�����һ�ݶ�̬����Ŀ���
	virtual Quote* clone() && {return new Quote(std::move(*this)); }//���ض�̬������ǰ�����һ�ݶ�̬����Ŀ���
	virtual ~Quote() = default;//����������
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
	Disc_quote(const Disc_quote& dq) :Quote(dq), quantity(dq.quantity), discount(dq.discount) { cout << "Disc_quote�������캯��" << endl; }//�������캯��
	Disc_quote& operator=(const Disc_quote&);
	double net_price(size_t) const = 0;//���麯�������д��麯�������ǳ�����ࣩ
	//Disc_quote* clone() const& { return new Disc_quote(*this); }//����ԭ���ǲ��ܴ����������Ķ���
protected:
	size_t quantity = 0;
	double discount = 0.0;
};
Disc_quote& Disc_quote::operator=(const Disc_quote& dq) {
	Quote::operator=(dq);
	quantity = dq.quantity;
	discount = dq.discount;
	cout << "Disc_quote������ֵ�����" << endl;
	return*this;
}//��ֵ������ֵ�����
#endif

#ifndef BULK_QUOTE_H_
#define BULK_QUOTE_H_

class Bulk_quote:public Disc_quote {//�̳���DISC_quote��
public:
	Bulk_quote() = default;
	using Disc_quote::Disc_quote::Disc_quote;
	//Bulk_quote(const string& book, double sales_price, size_t n, double disc) :Disc_quote(book,sales_price,n,disc){}
	Bulk_quote(const Bulk_quote& bq) :Disc_quote(bq) { cout << "Bulk_quote�������캯��" << endl; }//�������캯��
	Bulk_quote& operator=(const Bulk_quote& bq) {
		Disc_quote::operator=(bq);
		cout << "Bulk_quote������ֵ�����" << endl;
		return *this;
	}//��ֵ������ֵ�����
	Quote debug()const override { cout << "bookNo: " << isbn() << " # price" << price << " # min_qty: " << quantity << " # discount: " << discount << endl;
	                              return *this;
	                            }//�����麯��
	double net_price(size_t) const override;//�����麯������
	Bulk_quote* clone()const& { return new Bulk_quote(*this); }//���ط��ص�ǰ�����һ�ݶ�̬����Ŀ����ĺ�����
	Bulk_quote* clone() && {return new Bulk_quote(std::move(*this)); }//���麯���������ϵ���Ե�֪�ڴ��������ڣ�Quote�е�clone������������
//private:
	//size_t min_qty = 0;
	//double discount = 0.0;
};

double Bulk_quote::net_price(size_t n) const {//�����麯��������һ����������
	if (n >= quantity)
		return n * (1 - discount)*price;
	else
		return n * price;
}
#endif

#ifndef LES_QUOTE_H_
#define LES_QUOTE_H_

class Les_quote :public Disc_quote {//�̳���Quote��
public:
	Les_quote() = default;
	Les_quote(const string& book, double sales_price, size_t cnt, double dis) :Disc_quote(book, sales_price, cnt, dis) {}

	Quote debug()const override {
		cout << "bookNo: " << isbn() << " #price: " << price << " #max_qty: " << quantity << " #discount: " << discount << endl;
		return *this;
	}//�����麯��
	double net_price(size_t)const override;//�����麯������
										   //private:
										   //size_t max_qty = 0;
										   //double discount = 0.0;
};

double Les_quote::net_price(size_t n)const {//�����麯����δ�����涨�����ʹ��ۣ��������ְ�ԭ��
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
	multiset<std::shared_ptr<Quote>, decltype(compare)*>items{ compare };//��ʼ��compare���ң�������compare�������бȽ�
};
double Basket::total_receipt(ostream &os)const {
	double sum = 0.0;
	for (auto iter = items.cbegin(); iter != items.cend(); iter = items.upper_bound(*iter)) {//upper_bound������iter�ؼ�����ȵ�Ԫ�ص�ĩβԪ�ص���һλ��
		sum += print_total(os, **iter, items.count(*iter));//iter�����ú���һ��shared_ptr,������Ҫ������
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