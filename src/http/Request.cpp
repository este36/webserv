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

}
