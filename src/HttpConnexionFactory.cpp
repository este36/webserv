#include "HttpConnexionFactory.hpp"
#include "HttpConnexion.hpp"

#include <iostream>

HttpConnexionFactory::HttpConnexionFactory(const ServerConfig& server_config)
	: _server_config(server_config)
{}

net::PollTask *HttpConnexionFactory::create(net::TCPClient *client) const
{
	std::cout << "INFO: accepted new client connection" << std::endl;
	return new HttpConnexion(client, _server_config);
}
