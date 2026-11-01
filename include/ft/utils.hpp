#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>

namespace ft
{

template <typename T>
std::string to_string(const T& val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

}

#endif // UTILS_HPP
