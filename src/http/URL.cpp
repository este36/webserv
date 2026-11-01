#include "http/URL.hpp"
#include "ft/string_view.hpp"
#include "ft/utils.hpp"

#include <sstream>
#include <cctype>
#include <algorithm>
#include <vector>
#include <exception>
#include <iostream>

namespace http
{

std::pair<URL, bool> URL::create_from_request_target(const std::string& src)
{
	URL url;
	ft::string_view s(src);

	if (s.find('#') != ft::string_view::npos)
		return std::make_pair(url, false);

	ft::string_view::const_iterator start = s.begin();
	ft::string_view::const_iterator path_start = start;

	if (s.substr(0, 4) == "http")
	{
		ft::string_view::const_iterator it = start + 4;
		if (it != s.end() && *it == 's')
			++it;
		if (s.end() - it < 3 || s.substr(it - start, 3) != "://")
			return std::make_pair(url, false);
		it += 3;
		ft::string_view::const_iterator auth_end = std::find(it, s.end(), '/');
		url.host = s.substr(it - start, auth_end - it).str();
		path_start = auth_end;
	}
	else if (s.empty() || s[0] != '/')
		return std::make_pair(url, false);

	ft::string_view::const_iterator path_end = std::find(path_start, s.end(), '?');

	if (path_end != s.end())
		url.query = s.substr(path_end - start + 1, s.end() - path_end - 1).str();

	std::string raw_path = s.substr(path_start - start, path_end - path_start).str();
	if (raw_path.empty())
		raw_path = "/";

	std::pair<std::string, bool> decoded = URL::decode(raw_path);
	if (!decoded.second)
		return std::make_pair(url, false);

	std::pair<std::string, bool> normalized = URL::normalize(decoded.first);
	if (!normalized.second)
		return std::make_pair(url, false);

	url.path = normalized.first;

	return std::make_pair(url, true);
}

std::string URL::encode(const std::string& s)
{
	static const char hex_base[] = "0123456789ABCDEF";

	std::string res;
	res.reserve(s.length() * 3);

	char	buf[4] = {0};
	unsigned char c;

	for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
	{
		c = *it;
		if (std::isalnum(c) || c == '-' || c == '_'
			|| c == '.' || c == '~') {
			buf[0] = c;
			buf[1] = '\0';
		} else {
			buf[0] = '%';
			buf[1] = hex_base[c >> 4];
			buf[2] = hex_base[c & 0x0F];
		}
		res += buf;
	}
	return res;
}

std::pair<std::string, bool> URL::decode(const std::string& s)
{
	std::string res;
	res.reserve(s.length());

	int percent_code;
	int tmp;
	unsigned char c;

	for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
	{
		c = *it;
		if (std::isspace(c))
			return std::make_pair("", false);
		if (c == '%')
		{
			if (++it == s.end())
				return std::make_pair("", false);
			tmp = ft::hex_to_val(*it);
			if (tmp == -1)
				return std::make_pair("", false);
			percent_code = tmp << 4;
			if (++it == s.end())
				return std::make_pair("", false);
			tmp = ft::hex_to_val(*it);
			if (tmp == -1)
				return std::make_pair("", false);
			percent_code |= tmp;

			c = static_cast<char>(percent_code);
		}
		res += c;
	}
	return std::make_pair(res, true);
}

std::pair<std::string, bool> URL::normalize(const std::string& s)
{
	if (s.find("..") == std::string::npos)
		return std::make_pair(s, true);

	std::vector<std::string> segments;

	{
		std::size_t pos = 0;
		std::size_t end = 0;

		while (true)
		{
			if (s[pos] == '/')
				pos++;
			end = s.find("/", pos);
			if (pos != end) 
				segments.push_back(s.substr(pos, end - pos));
			if (end == std::string::npos)
				break;
			pos = end;
		}

	}

	{
		std::vector<std::string>::iterator it = segments.begin();
		while (it != segments.end())
		{
			if (*it == "." || *it == "")
				it = segments.erase(it);
			else
				++it;
		}
	}

	if (segments.empty())
		return std::make_pair("/", true);

	{
		std::vector<std::string>::iterator it = segments.begin();
		while (it != segments.end())
		{
			if (*it == "..")
			{
				if (it == segments.begin())
					return std::make_pair("", false);
				it = segments.erase(it);
				if (it == segments.begin())
					return std::make_pair("", false);
				--it;
				it = segments.erase(it);
			}
			else
				++it;
		}
	}

	if (segments.empty())
		return std::make_pair("/", true);

	std::string res;
	res.reserve(s.length());

	for (std::vector<std::string>::iterator it = segments.begin(); it != segments.end(); ++it)
		res += "/" + *it;

	return std::make_pair(res, true);
}

std::ostream& operator<<(std::ostream& out, const URL& url)
{
	out << url.host << url.path << url.query;
	return out;
}

}
