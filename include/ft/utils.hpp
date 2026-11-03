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

// Helper function to convert a string to lowercase for case-insensitive string manipulation
std::string to_lower(const std::string &str);

// return converted int value or -1 in case of error
int hex_to_val(char c);

template <typename T>
void remove_unordered(std::vector<T>& container, std::size_t index)
{
	if (index >= container.size() || container.empty())
		return;
	if (container.size() != index)
		container.back() = container[index];
	container.pop_back();
}

}

#endif // UTILS_HPP
