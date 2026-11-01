#ifndef POLL_TASK_HPP
#define POLL_TASK_HPP

#include "net/PollScheduler.hpp"

namespace net
{

class PollTask
{
public:
	PollTask *resume(struct pollfd& pfd) = 0;
}

}

#endif // POLL_TASK_HPP
