#ifndef URL_HPP
#define URL_HPP

#include <string>
#include <utility>

namespace http
{

struct URL
{
	std::string host;
	std::string path;
	std::string query;

	static std::string encode(const std::string& raw);
	static std::pair<std::string, bool> decode(const std::string& s);
	static std::pair<std::string, bool> normalize(const std::string& s);

	static std::pair<URL, bool> create_from_request_target(const std::string& src);

	// for every pairs, result.second incicates a parsing error if it equals to false
};

std::ostream& operator<<(std::ostream& out, const URL& url);

}

#endif // URL_HPP
