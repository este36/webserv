#include "net/example/EchoTaskFactory.hpp"
#include "net/example/EchoTask.hpp"

net::PollTask *EchoTaskFactory::create(net::TCPClient *client)
{
	return new EchoTask(client);
}
