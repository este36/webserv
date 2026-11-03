#include "http/Response.hpp"
#include <sstream>

namespace http
{

ResponseHead::ResponseHead()
	: status_code(0)
{}

Response::Response(int status_code)
	: _body(NULL)
{
	_head.status_code = status_code;
}

Response::~Response()
{
	if (_body != NULL)
		delete _body;
}

const ResponseHead& Response::head() const { return _head; }
ResponseHead& Response::head() { return _head; }

void Response::set_body(AStreamableBody *sb)
{
	if (_body != NULL && sb != _body)
		throw std::runtime_error("body already set");
	_body = sb;
}
const AStreamableBody *Response::body() const { return _body; }

std::ostream& operator<<(std::ostream& out, const Response& response)
{
	const AStreamableBody *body = response.body();

	serialize_version(out, response.head());
	out << " " << response.head().status_code << " " << reason_phrase(response.head().status_code) << CRLF;
	serialize_headers(out, response.head());
	out << CRLF;
	if (body != NULL && body->has_bytes())
	{
		std::string body_out;
		body->read(body_out, body->size());
		out << body_out;
	}
	return out;
}

std::string Response::serialize()
{
	std::ostringstream ss;
	ss << *this;
	return ss.str();
}

// Useful documentation:
// https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status
std::string reason_phrase(int status_code)
{
	switch (status_code)
	{
		case 100: return "Continue";
		case 101: return "Switching Protocols";
		case 103: return "Early Hints";
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 207: return "Multi-Status";
		case 208: return "Already Reported";
		case 226: return "IM Used";
		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 307: return "Temporary Redirect";
		case 308: return "Permanent Redirect";
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Content Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 416: return "Range Not Satisfiable";
		case 417: return "Expectation Failed";
		case 418: return "I'm a teapot";
		case 421: return "Misdirected Request";
		case 422: return "Unprocessable Content";
		case 423: return "Locked";
		case 424: return "Failed Dependency";
		case 425: return "Too Early";
		case 426: return "Upgrade Required";
		case 428: return "Precondition Required";
		case 429: return "Too Many Requests";
		case 431: return "Request Header Fields Too Large";
		case 451: return "Unavailable For Legal Reasons";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";
		case 506: return "Variant Also Negotiates";
		case 507: return "Insufficient Storage";
		case 508: return "Loop Detected";
		case 510: return "Not Extended";
		case 511: return "Network Authentication Required";
		default: throw std::runtime_error("Unreachable: Unknown Status Code");
	}
}

}
