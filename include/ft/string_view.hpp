#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

// this aim to be a 1:1 rewrite of std::string_view
// https://en.cppreference.com/cpp/header/string_view

#include <cstddef>
#include <string>
#include <ostream>

#include "ft/HashFn.hpp"

namespace ft
{

class string_view
{
	const char	*_buf;
	std::size_t	_len;

public:
	typedef char value_type;
	typedef const char* const_pointer;
	typedef const char& const_reference;
	typedef const char* iterator;
	typedef const char* const_iterator;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	static const size_type npos = static_cast<size_type>(-1);

	string_view();

	template <std::size_t N>
	string_view(const char (&str)[N]) : _buf(str), _len(N - 1) {};

	string_view(const char* str);
	string_view(const char* str, size_type len);
	string_view(const std::string& str);
	string_view(const string_view& other);

	string_view& operator=(const string_view& other);
	~string_view();

	const_reference operator[](size_type pos) const;
	const_reference at(size_type pos) const;
	const_pointer data() const;
	const_pointer c_str() const;
	const_reference front() const;
	const_reference back() const;
	size_type size() const;
	size_type length() const;
	bool empty() const;

	std::string str() const;

	const_iterator begin() const;
	const_iterator end() const;

	void remove_prefix(size_type n);
	void remove_suffix(size_type n);

	void destroy(); // if user knows that the instance owns the _buf memory

	string_view substr(size_type pos = 0, size_type count = npos) const;

	size_type find(const string_view& v, size_type pos = 0) const;
	size_type find(char c, size_type pos = 0) const;
	size_type find(const char* s, size_type pos, size_type count) const;
	size_type find(const char* s, size_type pos = 0) const;

	size_type rfind(const string_view& v, size_type pos = npos) const;
	size_type rfind(char c, size_type pos = npos) const;
	size_type rfind(const char* s, size_type pos, size_type count) const;
	size_type rfind(const char* s, size_type pos = npos) const;

	size_type find_first_of(const string_view& v, size_type pos = 0) const;
	size_type find_first_of(char c, size_type pos = 0) const;
	size_type find_first_of(const char* s, size_type pos, size_type count) const;
	size_type find_first_of(const char* s, size_type pos = 0) const;

	size_type find_last_of(const string_view& v, size_type pos = npos) const;
	size_type find_last_of(char c, size_type pos = npos) const;
	size_type find_last_of(const char* s, size_type pos, size_type count) const;
	size_type find_last_of(const char* s, size_type pos = npos) const;

	size_type find_first_not_of(const string_view& v, size_type pos = 0) const;
	size_type find_first_not_of(char c, size_type pos = 0) const;
	size_type find_first_not_of(const char* s, size_type pos, size_type count) const;
	size_type find_first_not_of(const char* s, size_type pos = 0) const;

	size_type find_last_not_of(const string_view& v, size_type pos = npos) const;
	size_type find_last_not_of(char c, size_type pos = npos) const;
	size_type find_last_not_of(const char* s, size_type pos, size_type count) const;
	size_type find_last_not_of(const char* s, size_type pos = npos) const;

	int compare(const string_view& v) const;
};

bool operator==(const string_view& lhs, const string_view& rhs);
bool operator!=(const string_view& lhs, const string_view& rhs);

std::ostream& operator<<(std::ostream& os, const string_view& sv);

template <>
struct Hash<string_view>
{
	size_t operator()(const string_view& val) const
	{
		return HashFn::djb2(val.c_str(), val.length());
	}
};

}

#endif // STRING_VIEW_HPP
