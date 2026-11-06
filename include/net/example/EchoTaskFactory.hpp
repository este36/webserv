#ifndef ECHO_TASK_FACTORY_HPP
#define ECHO_TASK_FACTORY_HPP

#include "net/ClientFactory.hpp"

class EchoTaskFactory : public net::ClientFactory
{
public:
	virtual net::PollTask *create(net::TCPClient *client) const;
};

#endif // ECHO_TASK_FACTORY_HPP
