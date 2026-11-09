#ifndef ECHO_TASK_HPP
#define ECHO_TASK_HPP

#include "net/PollTask.hpp"
#include "net/TCPClient.hpp"
#include <string>

class EchoTask : public net::PollTask
{
	net::TCPClient *_client;
	short			_events;
	std::string		_request_data;

	// immutable
	EchoTask(const EchoTask&);
	EchoTask& operator=(const EchoTask&);

public:
	explicit EchoTask(net::TCPClient *client);
	~EchoTask();

	net::PollTask::Result	resume(short revents);
	int fd() const;
	short events() const;
};

#endif
