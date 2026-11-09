#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
#include "net/PollScheduler.hpp"

class Server
{
	const Config&		_config;
	net::PollScheduler	_scheduler;

public:
	explicit Server(const Config& config);

	void run();
};

#endif // SERVER_HPP
