#include "net/example/EchoTask.hpp"
#include <stdexcept>
#include <iostream>

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
		{
			if (!_request_data.empty() && *_request_data.rbegin() == '\n')
				_request_data.erase(_request_data.size()-1, 1); // remove newline character
			std::cout << "reveived data : \"" <<  _request_data << "\"" << std::endl;
			_request_data = std::string("<h1>") + _request_data + "</h1>\r\n";
			_events = POLLOUT;
		}
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

