#include "http/Request.hpp"

namespace http
{

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
	out << request.method << " " << request.uri << " ";
	serialize_version(out, request);
	out << CRLF;
	serialize_headers(out, request.headers);
	out << request.body;
	return out;
}

}
