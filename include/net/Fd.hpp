#ifndef NET_FD
#define NET_FD

namespace net

class Fd
{
	// immutable
	Fd(const Fd&);
	Fd& operator=(const Fd&);

	int _fd;

public:
	explicit Fd(int fd = -1);
	~Fd();

	int get() const;
	void reset(int fd = -1);
	int release();
};

}

#endif
