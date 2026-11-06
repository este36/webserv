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

	// for every pairs of the returned value of the function below,
	// result.second indicates a parsing error if it equals to false

	static std::string encode(const std::string& raw);
	static std::pair<std::string, bool> decode(const std::string& s);
	static std::pair<std::string, bool> normalize(const std::string& s);

	static std::pair<URL, bool> create_from_raw_target(const std::string& src);

};

std::ostream& operator<<(std::ostream& out, const URL& url);

}

#endif // URL_HPP
