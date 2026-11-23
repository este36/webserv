#ifndef REQUEST_PARSER
#define REQUEST_PARSER

#include "http/Request.hpp"
#include <iostream>
#include <cstddef>
#include <string>

class RequestParser
{
	http::Request	_pending_request;
	std::string		_input_buffer;
	std::size_t		_max_body_size;
	std::size_t		_cursor;
	std::size_t		_remaining;

	enum State
	{
		REQUEST_LINE,
		HEADERS,
		FIXED_BODY,
		CHUNK_SIZE,
		CHUNK_DATA,
		CHUNK_DATA_CRLF,
		TRAILERS,
		COMPLETE
	};
	State _state;

	bool			read_line(std::string& line);
	void			parse_request_line(const std::string& line);
	void			parse_header_line(const std::string& line, http::Headers& destination) const;
  void      finish_headers();
public:
  explicit  RequestParser(std::size_t max_body_size);
	void			feed(const char *src, const size_t len);
	bool			done();
	void			reset();

	const http::Request&	request();

};

#endif
