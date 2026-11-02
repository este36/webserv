#ifndef POLL_TASK_HPP
#define POLL_TASK_HPP

#include "net/PollScheduler.hpp"
#include <cstdlib>

namespace net
{

class PollTask
{
public:
	enum Status
	{
		WAIT,
		FINISHED
	};

	struct Result
	{
		Status 		status;
		PollTask	*child;

		Result(Status stat = WAIT, PollTask *c = NULL);
	};

	virtual ~PollTask();

	virtual Result	resume(short revents) = 0;
	virtual int fd() const = 0;
	virtual short events() const = 0;
	virtual void on_child_finished(PollTask*);
};

}

#endif // POLL_TASK_HPP
