#include "net/TCPListener.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

using namespace net;

TCPListener::~TCPListener() {}

TCPClient*	TCPListener::accept_client()
{
	// TODO
	return NULL;
}

TCPListener::TCPListener(
    const std::string& host,
    const std::string& port,
    ClientFactory& factory
) : _factory(factory)
{
	struct addrinfo hints;
	struct addrinfo* res = NULL;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
		throw std::runtime_error("getaddrinfo() failed");

	_fd.reset(socket(res->ai_family, res->ai_socktype, res->ai_protocol));
	if (_fd.get() == -1)
	{
		freeaddrinfo(res);
		throw std::runtime_error("socket() failed");
	}

	int one = 1;
	setsockopt(_fd.get(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

	if (bind(_fd.get(), res->ai_addr, res->ai_addrlen) == -1)
    {
		freeaddrinfo(res);
		throw std::runtime_error("bind() failed");
	}

	if (listen(_fd.get(), SOMAXCONN) == -1)
    {
		freeaddrinfo(res);
		throw std::runtime_error("/listen() failed");
	}

	freeaddrinfo(res);
	if (!_fd.set_non_blocking())
		throw std::runtime_error("set_non_blocking() failed");
}

PollTask::Result TCPListener::resume(short)
{
    int client_fd = accept(_fd.get(), NULL, NULL);
    if (client_fd < 0)
        return PollTask::Result(WAIT, NULL);
    return PollTask::Result(
		WAIT,
		_factory.create(new TCPClient(client_fd))
	);
}

int    TCPListener::fd() const { return _fd.get(); }
short  TCPListener::events() const { return POLLIN; }
