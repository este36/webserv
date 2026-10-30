#ifndef PROFILER_HPP
#define PROFILER_HPP
#include <ctime>

namespace ft
{

class Profiler
{
	const char *_name;
	std::clock_t _start;

	// immutable
	Profiler(const Profiler& other);
	Profiler& operator=(const Profiler& other);

public:
	Profiler(const char *name);
	~Profiler();
};

}

#endif // PROFILER_HPP
