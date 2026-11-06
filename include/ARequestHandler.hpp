#ifndef AREQUESTHANDLER_HPP
#define AREQUESTHANDLER_HPP

#include "http/Request.hpp"
#include "ServerConfig.hpp"
#include "http/Response.hpp"

class ARequestHandler
{
	// every implementations should have only constant member variable or not any at all
public:
	virtual net::PollTask *handle(
					const ServerConfig& server_config,
					const http::Request& request,
					http::Response& response) const = 0;
};

#endif // AREQUESTHANDLER_HPP
