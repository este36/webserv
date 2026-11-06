#ifndef CLIENT_FACTORY_HPP
#define CLIENT_FACTORY_HPP

#include "net/PollTask.hpp"
#include "net/TCPClient.hpp"

namespace net
{

class ClientFactory
{
public:

	// the returned task should own the client
	virtual PollTask *create(TCPClient *client) const = 0;
};

}

#endif // CLIENT_FACTORY_HPP
