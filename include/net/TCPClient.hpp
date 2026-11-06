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

	// since we are only using 1 thread, we can share the read buffer between clients
	static const ssize_t recv_temp_buffer_capacity = 4098;
	static char recv_temp_buffer[recv_temp_buffer_capacity];
	static std::size_t	recv_temp_buffer_length;

	Fd _fd;

public:
	explicit TCPClient(int fd);

	const char		*received_data() const;
	std::size_t		received_length();

	// return value is the result from recv(), or 0 if no pending bytes to receive
	// The data is stored in the recv_temp_buffer
	ssize_t receive();

	// return value is the result from send()
	// send at most data.size()
	ssize_t send(const std::string& data);

	// return value is the result from send()
	// (overload) send at most size bytes
	ssize_t send(const unsigned char *data, const std::size_t size);

	int fd() const;
};

}

#endif // TCPCLIENT_HPP
