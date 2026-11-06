#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "http/MessageHead.hpp"
#include "http/AStreamableBody.hpp"

namespace http
{

struct ResponseHead : public MessageHead
{
	int	status_code;

	ResponseHead();
};

class Response
{
	ResponseHead _head;
	// Owns the StreamableBody instance
	// NULL means no body
	AStreamableBody	*_body;

 	// immutable since we are unable to copy the body accordingly
	Response& operator=(const Response& other);
	Response(const Response& other);

public:

	// non-explicit which means `Response resp = 200;` is ok.
	Response(int status_code = 200);
	~Response();

	const ResponseHead& head() const;
	ResponseHead& head();

	// takes ownership of the new body
	void set_body(AStreamableBody *sb);

	AStreamableBody *body();
	const AStreamableBody *body() const;

	std::string serialize();
};

std::string reason_phrase(int status_code);

std::ostream& operator<<(std::ostream& out, const Response& response);

}

#endif // HTTP_RESPONSE_HPP
