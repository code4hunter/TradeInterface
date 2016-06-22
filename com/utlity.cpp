#include "utlity.h"
#include <sstream>
#include <float.h>
#include <boost/tokenizer.hpp>

std::string IntToString(int i)
{
	std::ostringstream os;
	os << i;
	return os.str();
}

std::string DoubleToString(double d, int p)
{
	if (std::isnan(d))
	{
		return "NAN";
	}
	std::ostringstream os;
	os.precision(p);
	os.setf(std::ios::fixed, std::ios::floatfield);
	os << d;
	return os.str();
}

std::string get_product_name(const std::string &contract)
{
	std::string p;
	for (size_t i = 0; i<contract.size(); i++)
	{
		if (isalpha(contract[i]))
		{
			p.push_back(contract[i]);
		}
	}
	return p;
}

long DoubleToLong(double d)
{
	long a =(long) (d * 100);

	long r = a / 100;
	long b = a % 100;

	if (b >= 50)
		return r + 1;
	else
		return r;
}

long upto100(long n)
{
	int lsg = (100 - n % 100);
	int ordnum = n;
	if ((lsg >0) && (n % 100 != 0))
		ordnum += lsg;
	return ordnum;
}

long downto100(long n)
{
	return n - (n % 100);
}

double GenerateNaN(void)
{
	unsigned long nan[2] = { 0xffffffff, 0x7fffffff };
	return *( double* )nan;    
}

bool IsNumber(double x)
{
	return (x == x);
}

bool IsFiniteNumber(double x)
{
	return (x <= DBL_MAX && x >= -DBL_MAX);
}

double DoubleToPrice(double p)
{
	if (IsNumber(p) == false || IsFiniteNumber(p) ==false || p<0)
	{
		p = 0;
	}
	return p;
}

void split_value_items(const std::string &value, std::vector<std::string> &values, const std::string &delimiter)
{
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep(delimiter.c_str(), 0, boost::keep_empty_tokens);
	tokenizer items(value, sep);

	for (tokenizer::iterator p = items.begin(); p != items.end(); ++p)
	{
		values.push_back(*p);
	}
}