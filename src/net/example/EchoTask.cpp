#include "net/example/EchoTask.hpp"

EchoTask::EchoTask(net::TCPClient *client) : _client(client)
{
	if (_client == NULL)
		throw std::runtime_error("_client cannot be NULL");
	_events = POLLIN;
}

EchoTask::~EchoTask()
{
	delete _client;
}

net::PollTask::Result	EchoTask::resume(short revents)
{
	if (revents & POLLIN)
	{
		ssize_t ret = _client->receive(_request_data);
		if (ret < 0) // error case
			return net::PollTask::Result(FINISHED, NULL);
		else if (ret > 0)
			return net::PollTask::Result(WAIT, NULL);
		else if (ret == 0)
			_events = POLLOUT;
	}
	else if (revents & POLLOUT)
	{
		ssize_t ret = _client->send(_request_data);
		if (ret < 0) // error case
			return net::PollTask::Result(FINISHED, NULL);
		else if (ret > 0)
		{
			_request_data.erase(0, ret);
			return net::PollTask::Result(WAIT, NULL);
		}
		else if (ret == 0)
			_events = POLLIN;
	}
	return net::PollTask::Result(WAIT, NULL);
}

int EchoTask::fd() const { return _client->fd(); }
short EchoTask::events() const { return _events; }
