#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>

#define CRLF "\r\n"

namespace http
{

// Having an std::map is not a good solution because you can
// have multiple headers with the same name.
// There is the solution of having std::multimap, but you end up with a lot of heap
// allocations for nothing
// Having a simple dynamic array is better in this situation
// Using std::string also simplifies ownership and allows headers to be modified
typedef std::vector< std::pair<std::string, std::string> > Headers;

// we need to have a separate method because typedefs are only aliases. not actual type
void serialize_headers(std::ostream& out, const Headers& headers);

struct Message
{
	int			version_major;
	int			version_minor;
	Headers		headers;
	std::string	body;
};

void serialize_version(std::ostream& out, const Message& message);

enum MethodEnum
{
	METHOD_UNSET,
	METHOD_GET,
	METHOD_POST,
	METHOD_DELETE,
	METHOD_MAX
};

std::ostream& operator<<(std::ostream& out, const MethodEnum& method);

struct Request : public Message
{
	MethodEnum	method;
	URL			uri;
};

std::ostream& operator<<(std::ostream& out, const Request& request);

struct Response : public Message
{
	int status_code;

	std::string serialize();
};

std::string reason_phrase(int status_code);

std::ostream& operator<<(std::ostream& out, const Response& response);

}

#endif // MESSAGE_HPP
