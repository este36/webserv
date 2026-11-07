#ifndef REQUEST_PARSER
#define REQUEST_PARSER

#include "http/Request.hpp"

class RequestParser
{
	http::Request	pending_request;

public:
	void					feed(const char *src, const size_t len);
	bool					done();
	void					reset();

	const http::Request&	request();

};

#endif
