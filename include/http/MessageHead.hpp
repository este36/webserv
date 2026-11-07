#ifndef MESSAGE_HEAD_HPP
#define MESSAGE_HEAD_HPP

#include <string>
#include <vector>
#include <ostream>

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

struct MessageHead
{
	int			version_major;
	int			version_minor;
	Headers		headers;

	MessageHead();
};

// we need to have a separate method because typedefs are only aliases. not actual type
void serialize_headers(std::ostream& out, const MessageHead& head);

void serialize_version(std::ostream& out, const MessageHead& head);

}

#endif
