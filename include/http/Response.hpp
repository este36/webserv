#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "http/Message.hpp"
#include "http/AStreamableBody.hpp"

namespace http
{

class Response : public Message
{
	// Owns the StreamableBody instance
	// NULL means no body
	AStreamableBody	*_body;

 	// immutable since we are unable to copy the body accordingly
	Response& operator=(const Response& other);
	Response(const Response& other);

public:
	int				status_code;

	// non-explicit which means `Response resp = 200;` is ok.
	Response(int status_code = 200);
	~Response();

	void set_body(AStreamableBody *sb);
	const AStreamableBody *get_body() const;

	std::string serialize();
};

std::string reason_phrase(int status_code);

std::ostream& operator<<(std::ostream& out, const Response& response);

}

#endif // HTTP_RESPONSE_HPP
