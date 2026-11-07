#include "http/MessageHead.hpp"

namespace http
{

MessageHead::MessageHead() :
	version_major(1),
	version_minor(1),
	headers()
{}

void serialize_headers(std::ostream& out, const MessageHead& head)
{
	for (Headers::const_iterator it = head.headers.begin(); it != head.headers.end(); ++it)
		out << it->first << ": " << it->second << CRLF;
}

void serialize_version(std::ostream& out, const MessageHead& head)
{
	out << "HTTP/" << head.version_major << "." << head.version_minor;
}

}
