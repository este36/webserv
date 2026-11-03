#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "http/MessageHead.hpp"
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

struct RequestHead : public MessageHead
{
	MethodEnum	method;
	URL			uri;

	RequestHead();
};

struct Request
{
	RequestHead	head;
	std::string	body;
};

std::ostream& operator<<(std::ostream& out, const Request& request);

}

#endif
