#ifndef _UTLITY_H_
#define _UTLITY_H_

#include <list>
#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <vector>

std::string IntToString(int i);
std::string DoubleToString(double d, int p);
std::string get_product_name(const std::string &contract);

long DoubleToLong(double d); // 四舍五入

long upto100(long n);

long downto100(long n);

double GenerateNaN(void);

double DoubleToPrice(double p);

void split_value_items(const std::string &value, std::vector<std::string> &values,const std::string &delimiter="#");

std::string cvt_stk_code(const std::string &code);

#endif