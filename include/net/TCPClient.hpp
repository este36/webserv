#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP

#include "net/PollTask.hpp"
#include "net/Fd.hpp"
#include <sys/types.h>
#include <string>

namespace net
{

class TCPClient
{
	// immutable
	TCPClient(const TCPClient&);
	TCPClient& operator=(const TCPClient&);

	Fd _fd;

public:
	explicit TCPClient(int fd);

	// return value is the result from recv()
	// append the received data to out.
	ssize_t receive(std::string& out);

	// return value is the result from send()
	// send at most data.size()
	ssize_t send(const std::string& data);

	int fd() const;
};

}

#endif // TCPCLIENT_HPP
