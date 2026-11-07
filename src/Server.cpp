#include "Server.hpp"
#include "HttpConnexionFactory.hpp"
#include "net/TCPListener.hpp"

Server::Server(const Config& config) : _config(config) {}

void Server::run()
{
	for (std::size_t i = 0; i < _config.servers.size(); ++i)
	{
		const ServerConfig& server_config  = _config.servers[i];
		_scheduler.add(
			new net::TCPListener(
					server_config.server_name,
					server_config.port,
					HttpConnexionFactory(server_config)
				)
		);
	}

	_scheduler.run();
}
