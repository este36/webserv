#include "net/TCPListener.hpp"
#include "ft/utils.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <stdexcept>

using namespace net;

TCPListener::~TCPListener() {}

TCPClient	*TCPListener::try_accept_client()
{
	int client_fd = accept(_fd.get(), NULL, NULL);

	if (client_fd == -1)
		return NULL;

	return new TCPClient(client_fd);
}

TCPListener::TCPListener(
    const std::string host,
    const int port,
    ClientFactory& factory
) : _factory(factory)
{
	struct addrinfo hints;
	struct addrinfo* res = NULL;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	const std::string port_str = ft::to_string(port);

	if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res) != 0)
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
		throw std::runtime_error("listen() failed");
	}

	freeaddrinfo(res);
	if (!_fd.set_non_blocking())
		throw std::runtime_error("set_non_blocking() failed");
}

PollTask::Result TCPListener::resume(short)
{
	TCPClient *client = try_accept_client();

	// basically, the task will never finish and always return WAIT
    if (client == NULL)
        return PollTask::Result(WAIT, NULL);
    return PollTask::Result(WAIT, _factory.create(client));
}

int    TCPListener::fd() const { return _fd.get(); }
short  TCPListener::events() const { return POLLIN; }
