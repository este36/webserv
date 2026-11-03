#ifndef TCP_LISTENER_HPP
#define TCP_LISTENER_HPP

#include "net/PollTask.hpp"
#include "net/TCPClient.hpp"
#include "net/ClientFactory.hpp"
#include <string>
#include <poll.h>

namespace net
{

class TCPListener : public PollTask
{
	Fd				_fd;
	ClientFactory&	_factory;

	// immutable
	TCPListener(const TCPListener&);
	TCPListener& operator=(const TCPListener&);

	TCPClient*	try_accept_client();

public:
	TCPListener(const std::string& host, const std::string& port, ClientFactory& factory);
	virtual ~TCPListener();

	Result resume(short revents);
	int    fd() const;
	short  events() const;
};

}

#endif // TCP_LISTENER_HPP
