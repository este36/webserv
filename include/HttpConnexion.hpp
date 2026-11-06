#ifndef HTTP_CONNEXION_HPP
#define HTTP_CONNEXION_HPP

#include "net/PollTask.hpp"
#include "net/TCPClient.hpp"
#include "http/Response.hpp"
#include "ARequestHandler.hpp"
#include "ServerConfig.hpp"
#include "RequestParser.hpp"

class HttpConnexion : public net::PollTask
{
	const ServerConfig&	_server_config;

	net::TCPClient 		*_client;

	RequestParser		_request_parser;
	http::Response		_response;
	std::string			_head_buffer;
	std::string			_body_buffer;

	enum State
	{
		PERFORM_READ,
		PERFORM_WRITE_HEAD,
		PERFORM_WRITE_BODY,
		WAIT_FOR_CHILD_RESPONSE,
	};

	State				_state;

	// `dispatch_request` act as the router of the server.
	// It takes a complete request, check it with the config, and returns the corresponding handler
	//
	// returns an rvalue reference to the request handler.
	// every handlers should be created and returned directly like `return X(...);` in the function body
	const ARequestHandler&	dispatch_request(const http::Request& request);

	void prepare_response();

public:
	explicit HttpConnexion(
				net::TCPClient *client,
				const ServerConfig& server_config);

	~HttpConnexion();

	net::PollTask::Result	resume(short revents);
	int fd() const;
	short events() const;

	void on_child_finished(net::PollTask*);
};

#endif // HTTP_CONNEXION_HPP
