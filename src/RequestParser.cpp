#include "RequestParser.hpp"

void	RequestParser::feed(const char *src, const size_t len)
{
	(void)src;
	(void)len;
	throw std::runtime_error("TODO");

}

bool	RequestParser::done()
{
	throw std::runtime_error("TODO");
}

void	RequestParser::reset()
{
	throw std::runtime_error("TODO");
}

const http::Request&	RequestParser::request()
{
	throw std::runtime_error("TODO");
}
