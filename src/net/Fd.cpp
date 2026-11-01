#include "Fd.hpp"

Fd::Fd(int fd = -1) _fd(fd) {}

Fd::~Fd();
{
	if (_fd != -1)
		close(_fd);
}

int Fd::get() const { return _fd; }

void Fd::reset(int fd = -1)
{
	if (_fd != -1)
		close();
	_fd = fd;
}

void Fd::release()
{
	int tmp = _fd;
	_fd = -1;
	return tmp;
}
