#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "http/Request.hpp"
#include "ServerConfig.hpp"
#include "http/Response.hpp"
#include "net/PollTask.hpp"

// The http::Response in the 3rd parameter should be used as the response of the handler

typedef net::PollTask *(*RequestHandler)(
				const ServerConfig& server_config,
				const http::Request& request,
				http::Response& response);

net::PollTask	*handle_cgi(
					const ServerConfig& server_config,
					const http::Request& request,
					http::Response& response);

net::PollTask	*handle_delete(
					const ServerConfig& server_config,
					const http::Request& request,
					http::Response& response);

net::PollTask	*handle_upload(
					const ServerConfig& server_config,
					const http::Request& request,
					http::Response& response);

net::PollTask	*handle_get(
					const ServerConfig& server_config,
					const http::Request& request,
					http::Response& response);

#endif // REQUESTHANDLER_HPP
