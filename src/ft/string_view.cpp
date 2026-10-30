#include "ft/string_view.hpp"

#include <cstring>

namespace ft
{

string_view::string_view() : _buf(NULL), _len(0) {}
string_view::~string_view() {}
string_view::string_view(const char* str) : _buf(str), _len(std::strlen(str)) {}
string_view::string_view(const char* str, string_view::size_type len) : _buf(str), _len(len)  {}
string_view::string_view(const std::string& str) : _buf(str.c_str()), _len(str.length()) {}
string_view::string_view(const string_view& other) : _buf(other._buf), _len(other._len) {}
string_view& string_view::operator=(const string_view& other)
{
	this->_buf = other._buf;
	this->_len = other._len;
	return *this;
}

string_view::const_reference string_view::operator[](string_view::size_type pos) const
{
	return _buf[pos];
}

string_view::const_reference string_view::at(string_view::size_type pos) const
{
	if (pos >= _len)
		throw std::out_of_range("string_view::at");
	return _buf[pos];
}

string_view::const_pointer string_view::data() const { return _buf; }
string_view::const_pointer string_view::c_str() const { return _buf; }
string_view::size_type string_view::size() const { return _len; }
string_view::size_type string_view::length() const { return _len; }
string_view::const_reference string_view::back() const { return _buf[_len - 1]; }
string_view::const_reference string_view::front() const { return _buf[0]; }
bool string_view::empty() const { return _len == 0; }

string_view::const_iterator string_view::begin() const { return _buf; }
string_view::const_iterator string_view::end() const { return (_len ? _buf + _len : _buf); }

void string_view::remove_prefix(string_view::size_type n)
{
	if (n > _len)
		n = _len;
	_buf += n;
	_len -= n;
}

void string_view::remove_suffix(string_view::size_type n)
{
	if (n > _len)
		n = _len;
	_len -= n;
}

void string_view::destroy()
{
	delete[] _buf;
}

string_view string_view::substr(string_view::size_type pos, string_view::size_type count) const
{
	if (pos > _len)
		throw std::out_of_range("string_view::substr");
	if (count > _len - pos)
		count = _len - pos;
	return string_view(_buf + pos, count);
}

std::string string_view::str() const { return std::string(_buf, _len); }

// https://en.cppreference.com/cpp/string/basic_string_view/find
// All *find* member fonctions share the same overloads.
// Here is a macro that define automatically all corresponding overloads,
// actually just to reduce the amount of lines of code in this file.
#define STRINGVIEW_OVERLOADS(fn)\
string_view::size_type string_view::fn(char c, string_view::size_type pos) const\
{\
	return fn(string_view(&c, 1), pos);\
}\
\
string_view::size_type string_view::fn(const char* s, string_view::size_type pos, string_view::size_type count) const\
{\
	return fn(string_view(s, count), pos);\
}\
\
string_view::size_type string_view::fn(const char* s, string_view::size_type pos) const\
{\
	return fn(string_view(s), pos);\
}\

string_view::size_type string_view::find(const string_view& v, string_view::size_type pos) const
{
	if (pos >= _len)
		return string_view::npos;
	for (std::size_t i = pos; i < _len && (_len - i) >= v._len; ++i)
	{
		std::size_t j = 0;
		for (; j < v._len; ++j)
		{
			if (_buf[i + j] != v._buf[j])
				break;
		}
		if (j == v._len)
			return i;
	}
	return string_view::npos;
}

STRINGVIEW_OVERLOADS(find)

string_view::size_type string_view::rfind(const string_view& v, string_view::size_type pos) const
{
	if (v._len > _len)
		return string_view::npos;

	if (pos >= _len)
		pos = (_len != 0 ? _len - 1 : 0);

	if (v._len > _len - pos)
		pos = _len - v._len;

	for (difference_type i = static_cast<difference_type>(pos); i >= 0; --i)
	{
		std::size_t j = 0;
		for (; j < v._len; ++j)
		{
			if (_buf[i + j] != v._buf[j])
				break;
		}
		if (j == v._len)
			return i;
	}
	return string_view::npos;
}

STRINGVIEW_OVERLOADS(rfind)

string_view::size_type string_view::find_first_of(const string_view& v, string_view::size_type pos) const
{
	if (pos >= _len)
		return string_view::npos;
	for (std::size_t i = pos; i < _len; ++i)
	{
		for (std::size_t j = 0; j < v._len; ++j)
		{
			if (_buf[i] == v._buf[j])
				return i;
		}
	}
	return string_view::npos;
}

STRINGVIEW_OVERLOADS(find_first_of)

string_view::size_type string_view::find_last_of(const string_view& v, string_view::size_type pos) const
{
	if (pos >= _len)
		pos = (_len != 0 ? _len - 1 : 0);
	for (difference_type i = static_cast<difference_type>(pos); i >= 0; --i)
	{
		for (std::size_t j = 0; j < v._len; ++j)
		{
			if (_buf[i] == v._buf[j])
				return i;
		}
	}
	return string_view::npos;
}

STRINGVIEW_OVERLOADS(find_last_of)

string_view::size_type string_view::find_first_not_of(const string_view& v, string_view::size_type pos) const
{
	if (pos >= _len)
		return string_view::npos;
	for (std::size_t i = pos; i < _len; ++i)
	{
		std::size_t j = 0;
		for (; j < v._len && _buf[i] != v._buf[j]; ++j)
			;
		if (j == v._len)
			return i;
	}
	return string_view::npos;
}

STRINGVIEW_OVERLOADS(find_first_not_of)

string_view::size_type string_view::find_last_not_of(const string_view& v, string_view::size_type pos) const
{
	if (pos >= _len)
		pos = (_len != 0 ? _len - 1 : 0);
	for (difference_type i = static_cast<difference_type>(pos); i >= 0; --i)
	{
		std::size_t j = 0;
		for (; j < v._len && _buf[i] != v._buf[j]; ++j)
			;
		if (j == v._len)
			return i;
	}
	return string_view::npos;
}

STRINGVIEW_OVERLOADS(find_last_not_of)

int string_view::compare(const string_view& v) const
{
    std::size_t i = 0;
    while (i < _len && i < v._len && _buf[i] == v._buf[i])
        i++;
    if (i == _len && i == v._len)
        return 0;
    if (i == _len)
        return -1;
    if (i == v._len)
        return 1;
    return static_cast<unsigned char>(_buf[i]) - static_cast<unsigned char>(v._buf[i]);
}

#undef STRINGVIEW_OVERLOADS

bool operator==(const string_view& lhs, const string_view& rhs)
{
	if (lhs.length() != rhs.length())
		return false;
	if (lhs.empty())
		return true;
	return std::memcmp(lhs.data(), rhs.data(), lhs.length()) == 0;
}

bool operator!=(const string_view& lhs, const string_view& rhs)
{
	return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const string_view& sv)
{
	os.write(sv.data(), static_cast<std::streamsize>(sv.size()));
	return os;
}

}
