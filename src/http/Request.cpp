#include "http/Request.hpp"

namespace http
{

RequestHead::RequestHead() : method(METHOD_UNSET) {}

std::ostream& operator<<(std::ostream& out, const MethodEnum method)
{
	switch(method)
	{
		case METHOD_GET:	out << "GET"; break;
		case METHOD_POST:	out << "POST"; break;
		case METHOD_DELETE:	out << "DELETE"; break;
		default:			out << "UNKNOWN"; break;
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const Request& request)
{
	out << request.head.method << " " << request.head.uri << " ";
	serialize_version(out, request.head);
	out << CRLF;
	serialize_headers(out, request.head);
	out << request.body;
	return out;
}

std::string Request::get_extention() const
{
	const std::string &path = this->head.uri.path;

	if (path.empty() || path[path.size() - 1] == '/')
		return "";

	size_t dot = path.rfind('.');

	if (dot == std::string::npos)
		return "";

	std::string extension = path.substr(dot);

	if (extension.find('/') != std::string::npos)
		return "";

	size_t i = 1;
	while (i < extension.size())
	{
		if (!std::isalnum(static_cast<unsigned char>(extension[i++])))
			return "";
	}

	return extension;
}

}
