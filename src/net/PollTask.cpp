#include "net/PollTask.hpp"
#include <stdexcept>

namespace net
{

PollTask::Result::Result(PollTask::Status stat, PollTask *c) : status(stat), child(c)
{
	if (status == FINISHED && child != NULL)
		throw std::runtime_error("bad usage");
}

PollTask::~PollTask() {}

void PollTask::on_child_finished(PollTask*) {}

}
