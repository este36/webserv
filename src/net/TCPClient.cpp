#include "net/TCPClient.hpp"
#include <sys/socket.h>

using namespace net;

TCPClient::TCPClient(int fd) : _fd(fd) {}

ssize_t TCPClient::receive(std::string& out)
{
	unsigned char buf[4098];
	int n = recv(_fd.get(), buf, sizeof(buf), 0);
	if (n > 0)
		out.append(reinterpret_cast<char *>(buf), n);
	return n;
}

ssize_t TCPClient::send(const std::string& in)
{
	return ::send(_fd.get(), in.data(), in.size(), 0);
}

int TCPClient::fd() const { return _fd.get(); }
