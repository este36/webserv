#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "http/Message.hpp"
#include "http/URL.hpp"

namespace http
{

enum MethodEnum
{
	METHOD_UNSET,
	METHOD_GET,
	METHOD_POST,
	METHOD_DELETE,
	METHOD_MAX
};

std::ostream& operator<<(std::ostream& out, const MethodEnum method);

struct Request : public Message
{
	MethodEnum	method;
	URL			uri;
	std::string	body;

	// to ensure 0 initialized
	Request();
};

std::ostream& operator<<(std::ostream& out, const Request& request);

}

#endif
