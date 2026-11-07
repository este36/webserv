#include "net/Fd.hpp"
#include <fcntl.h>
#include <unistd.h>

using namespace net;

Fd::Fd(int fd) : _fd(fd) {}

Fd::~Fd()
{
	if (_fd != -1)
		close(_fd);
}

int Fd::get() const { return _fd; }

void Fd::reset(int fd)
{
	if (_fd != -1)
		close(fd);
	_fd = fd;
}

int Fd::release()
{
	int tmp = _fd;
	_fd = -1;
	return tmp;
}

bool Fd::set_non_blocking()
{
	int flags = fcntl(_fd, F_GETFL, 0);

	if (flags == -1)
		return false;

	if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return false;

	return true;
}
