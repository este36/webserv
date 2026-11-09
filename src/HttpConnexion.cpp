#include "HttpConnexion.hpp"
#include "ft/utils.hpp"

HttpConnexion::HttpConnexion(
			net::TCPClient *client, 
			const ServerConfig& server_config) :
	_server_config(server_config),
	_client(client)
{}

HttpConnexion::~HttpConnexion()
{
	delete _client;
}

RequestHandler	HttpConnexion::dispatch_request(const http::Request& request)
{
	(void)request;
	throw std::runtime_error("TODO");
}

void HttpConnexion::prepare_response()
{
	_head_buffer.clear();

	if (_response.body() != NULL)
	{
		_response.head().headers.push_back(
			std::make_pair("Content-Length", ft::to_string(_response.body()->size()))
		);
	}

	_head_buffer += ft::to_string(_response.head());
}

net::PollTask::Result	HttpConnexion::resume(short revents)
{
	if (_state == PERFORM_READ && revents & POLLIN)
	{
		ssize_t ret = _client->receive();

		if (ret < 0) // fatal error, we cancel the task
			return net::PollTask::Result(net::PollTask::FINISHED, NULL);

		_request_parser.feed(_client->received_data(), _client->received_length());

		if (_request_parser.done())
		{
			_state = PERFORM_WRITE_HEAD;

			const http::Request& request = _request_parser.request();

			RequestHandler handler = dispatch_request(request);

			net::PollTask *child = handler(_server_config, request, _response);

			if (child != NULL)
			{
				_state = WAIT_FOR_CHILD_RESPONSE;
				return net::PollTask::Result(net::PollTask::WAIT, child);
			}

			prepare_response();
		}
	}
	else if (_state == PERFORM_WRITE_HEAD && revents & POLLOUT)
	{
		ssize_t ret = _client->send(_head_buffer.data(), _head_buffer.length());

		if (ret < 0) // fatal error, we cancel the task
			return net::PollTask::Result(net::PollTask::FINISHED, NULL);
		if (ret == 0)
		{
			if (_response.body() != NULL)
				_state = PERFORM_WRITE_BODY;
			else
			{
				_state = PERFORM_READ; // no more data to send, back to reading
				_request_parser.reset();
			}
		}

		_head_buffer.erase(0, ret);
	}
	else if (_state == PERFORM_WRITE_BODY && revents & POLLOUT)
	{
		ssize_t chunk_ret = _response.body()->read_chunk(_body_buffer, 4098); // arbitrary chunk size

		if (chunk_ret < 0) // fatal error, we cancel the task
			return net::PollTask::Result(net::PollTask::FINISHED, NULL);

		ssize_t send_ret = _client->send(_body_buffer.data(), _body_buffer.length());

		if (send_ret < 0) // fatal error, we cancel the task
			return net::PollTask::Result(net::PollTask::FINISHED, NULL);

		if (send_ret == 0 && chunk_ret == 0) // no more data to process in the body, and everything is now sent to client.
		{
			_state = PERFORM_READ;
			_request_parser.reset();
		}
		else
		{
			_body_buffer.erase(0, send_ret);
		}
	}
	return net::PollTask::Result(net::PollTask::WAIT, NULL);
}

void HttpConnexion::on_child_finished(net::PollTask*)
{
	// the child task completed his work, now we need to send the response
	_state = PERFORM_WRITE_HEAD;
	prepare_response();
}

int HttpConnexion::fd() const { return _client->fd(); }

short HttpConnexion::events() const
{
	switch (_state)
	{
		case PERFORM_WRITE_HEAD: return POLLOUT;
		case PERFORM_WRITE_BODY: return POLLOUT;
		case PERFORM_READ: return POLLIN;
		case WAIT_FOR_CHILD_RESPONSE: return 0;
		default: throw std::runtime_error("unreachable");
	}
}
