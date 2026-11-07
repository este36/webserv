#include "net/example/EchoTaskFactory.hpp"
#include "net/example/EchoTask.hpp"
#include <iostream>

net::PollTask *EchoTaskFactory::create(net::TCPClient *client)
{
	std::cout << "INFO: New client connected." << std::endl;
	return new EchoTask(client);
}
