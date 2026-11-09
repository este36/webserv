#ifndef HTTP_CONNEXION_FACTORY
#define HTTP_CONNEXION_FACTORY

#include "net/ClientFactory.hpp"
#include "ServerConfig.hpp"

class HttpConnexionFactory : public net::ClientFactory
{
	const ServerConfig& _server_config;

public:
	HttpConnexionFactory(const ServerConfig& server_config);

	net::PollTask *create(net::TCPClient *client) const;
};

#endif // HTTP_CONNEXION_FACTORY
