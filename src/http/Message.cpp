#include "http/Message.hpp"

namespace http
{

Message::Message() :
	version_major(1),
	version_minor(1),
	headers()
{}

void serialize_headers(std::ostream& out, const Headers& headers)
{
	for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
		out << it->first << ": " << it->second << CRLF;
}

void serialize_version(std::ostream& out, const Message& message)
{
	out << "HTTP/" << message.version_major << "." << message.version_minor;
}

}
