#include "net/PollScheduler.hpp"
#include "ft/utils.hpp"
#include <iterator>
#include <algorithm>

using namespace net;

volatile sig_atomic_t PollScheduler::running = 0;

PollScheduler::PollScheduler() {}

PollScheduler::~PollScheduler()
{
	typedef std::vector<TaskEntry*>::iterator entry_iterator;

	for (entry_iterator it = _root_tasks.begin(); it != _root_tasks.end(); ++it)
	{
		for (entry_iterator child = (*it)->children.begin(); child != (*it)->children.end(); ++child)
			delete (*child)->task;

		delete (*it)->task;
	}
}

void PollScheduler::signal_handler(int)
{
	PollScheduler::running = 1;
}

void PollScheduler::setup_signal_handlers()
{
	signal(SIGINT, PollScheduler::signal_handler);
	signal(SIGTERM, PollScheduler::signal_handler);
}

PollScheduler::TaskEntry *PollScheduler::add_task(PollTask *task)
{
	if (task == NULL || task->fd() == -1)
		return NULL;
	_tasks.push_back((TaskEntry){ .task = task });

	TaskEntry& entry = _tasks.back();

	if (_tasks_by_fd.find(task->fd()) == _tasks_by_fd.end())
	{
		_tasks_by_fd.insert(std::make_pair(task->fd(), &entry));
		_pollfds.push_back((pollfd){
			.fd = task->fd(),
			.events = task->events(),
		});
	}

	return &entry;
}

bool PollScheduler::TaskEntry::operator==(const TaskEntry& rhs)
{
	return task->fd() == rhs.task->fd();
}

void PollScheduler::delete_entry(TaskEntry *entry)
{
	if (entry == NULL)
		return;
	_tasks_by_fd.erase(entry->task->fd());
	// find and delete _tasks entry
	{
		std::list<TaskEntry>::iterator element = std::find(_tasks.begin(), _tasks.end(), *entry);
		_tasks.erase(element);
	}
	// find and delete _root_tasks entry
	if (entry->parent == NULL)
	{
		std::vector<TaskEntry*>::iterator element = std::find(_root_tasks.begin(), _root_tasks.end(), entry);
		_root_tasks.erase(element);
	}
	// find and delete _pollfds entry
	{
		for (std::size_t i = 0; i < _pollfds.size(); ++i)
		{
			if (_pollfds[i].fd == entry->task->fd())
				_pollfds.erase(_pollfds.begin() + i);
		}
	}
	// delete the last reference to the task
	delete entry->task;
}

void PollScheduler::add(PollTask *task)
{
	TaskEntry *entry = add_task(task);

	if (entry == NULL)
		return;

	_root_tasks.push_back(entry);
}

void PollScheduler::run()
{
	std::vector<TaskEntry *> dead_tasks;
	dead_tasks.reserve(256);
	std::vector< std::pair<PollTask *, TaskEntry *> > new_tasks; // task and parent
	dead_tasks.reserve(256);

	running = 1;

	if (_pollfds.size() == 0)
		return;

	while (running)
	{
		int ready_count = poll(&_pollfds.front(), _pollfds.size(), POLL_TIMEOUT);

		if (!running) // check again if the signal has been handled
			return;
		if (ready_count == -1)
			throw std::runtime_error("poll() failed");
	
		for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
		{
			if (it->revents == 0)
				continue;

			TaskEntry *entry = _tasks_by_fd[it->fd];

			if (entry->pending_removal)
				continue;

			PollTask::Result res = entry->task->resume(it->revents);

			if (res.status == PollTask::FINISHED)
			{
				// should cancel subtasks
				std::vector<TaskEntry *> stack;
				stack.push_back(entry);
				while (!stack.empty())
				{
					TaskEntry *head = stack.back();
					stack.pop_back();

					for (std::vector<TaskEntry*>::iterator it = head->children.begin(); it != head->children.end(); ++it)
						stack.push_back(*it);

					head->pending_removal = true;
					dead_tasks.push_back(head);
				}
			}
			else if (res.child != NULL)
				new_tasks.push_back(std::make_pair(res.child, entry));

			it->events = entry->task->events();
			it->fd = entry->task->fd();
		}

		for (std::size_t i = 0; i < dead_tasks.size(); ++i)
		{
			TaskEntry *entry = dead_tasks[i];
			// if the task is a child task, we need to know if the parent is alive or not
			// if the parent is also dead, then it does not matter anymore
			if (entry->parent != NULL
				&& std::find(dead_tasks.begin(), dead_tasks.end(), entry->parent) == dead_tasks.end())
			{
				std::vector<TaskEntry*>::iterator element = std::find(
					entry->parent->children.begin(),
					entry->parent->children.end(),
					entry
				);
				std::size_t parent_index = std::distance(entry->parent->children.begin(), element);
				ft::remove_unordered(entry->parent->children, parent_index);
				entry->parent->task->on_child_finished(entry->task);
			}
			delete_entry(dead_tasks[i]);
		}

		for (std::size_t i = 0; i < new_tasks.size(); ++i)
		{
			std::pair< PollTask*, TaskEntry* > *it = &new_tasks[i];
			TaskEntry *child = add_task(it->first);
			child->parent = it->second;
			child->parent->children.push_back(child);
		}

		dead_tasks.clear();
		new_tasks.clear();
	}
}
