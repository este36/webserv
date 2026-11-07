#ifndef POLL_SCHEDULER_HPP
#define POLL_SCHEDULER_HPP

#include <poll.h>
#define POLL_TIMEOUT 1000

#include "net/PollTask.hpp"
#include <vector>
#include <csignal>
#include <list>
#include "ft/unordered_map.hpp"

namespace net
{

// In our project, the instance is used as a singleton.
// Wich means there is only one instance for the entire program.
// after run(), the only thing that can stop the scheduler is a fatal error or CTRL-C

class PollScheduler
{
	struct TaskEntry
	{
		PollTask				*task;
		TaskEntry				*parent;
		std::vector<TaskEntry*> children;
		bool					pending_removal;
	
		bool operator==(const TaskEntry& rhs);
		TaskEntry(PollTask *task);
	};

	std::list<TaskEntry>				_tasks;
	ft::unordered_map<int, TaskEntry*>	_tasks_by_fd;
	std::vector<TaskEntry*>				_root_tasks;
	std::vector<pollfd>					_pollfds;

	// tasks management
	std::vector<TaskEntry *>							_dead_tasks;
	std::vector< std::pair<PollTask *, TaskEntry *> >	_new_tasks; // task and parent

	// immutable
	PollScheduler(const PollScheduler&);
	PollScheduler& operator=(const PollScheduler&);

	TaskEntry	*task_add(PollTask *task);
	void		task_stop(TaskEntry *entry);
	void 		delete_entry(TaskEntry* task);

public:
	void setup_signal_handlers();
	void add(net::PollTask *task);
	void run(); 

	PollScheduler();
	~PollScheduler();

	static void signal_handler(int);
	static volatile sig_atomic_t running;
};

}

#endif // POLL_SCHEDULER_HPP
