#ifndef POLL_TASK_HPP
#define POLL_TASK_HPP

#include <cstdlib>
#include <poll.h>

namespace net
{

class PollTask
{
public:
	enum StatusEnum
	{
		WAIT,
		FINISHED
	};

	struct Result
	{
		StatusEnum	status;
		PollTask	*child;

		Result(StatusEnum stat = WAIT, PollTask *c = NULL);
	};

	virtual ~PollTask();

	virtual Result	resume(short revents) = 0;
	virtual int fd() const = 0;
	virtual short events() const = 0;
	virtual void on_child_finished(PollTask*);
};

}

#endif // POLL_TASK_HPP
