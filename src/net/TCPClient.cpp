#include "net/TCPClient.hpp"
#include <sys/socket.h>

using namespace net;

char TCPClient::recv_temp_buffer[TCPClient::recv_temp_buffer_capacity] = {0};
std::size_t	TCPClient::recv_temp_buffer_length = 0;

TCPClient::TCPClient(int fd) : _fd(fd)
{
	_fd.set_non_blocking();
}

ssize_t TCPClient::receive()
{
	ssize_t n = recv(_fd.get(), recv_temp_buffer, recv_temp_buffer_capacity, 0);
	recv_temp_buffer_length = n;
	return (n < recv_temp_buffer_capacity ? 0 : n ); // return 0 if there is not anymore data to receive
}

ssize_t TCPClient::send(const std::string& in)
{
	return ::send(_fd.get(), in.data(), in.size(), 0);
}

ssize_t TCPClient::send(const unsigned char *data, const std::size_t size)
{
	return ::send(_fd.get(), data, size, 0);
}

const char	*TCPClient::received_data() const { return recv_temp_buffer; }
std::size_t		TCPClient::received_length() { return recv_temp_buffer_length; }

int TCPClient::fd() const { return _fd.get(); }
