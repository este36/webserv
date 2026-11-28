#include "HttpConnexion.hpp"
#include "ft/utils.hpp"

HttpConnexion::HttpConnexion(
			net::TCPClient *client, 
			const ServerConfig& server_config) :
	_server_config(server_config),
	_client(client),
	_state(PERFORM_READ)										//comment(antuel): initialitation pour n'avoir pas d'information poubelle.
{}


HttpConnexion::~HttpConnexion()
{
	delete _client;
}


RequestHandler	HttpConnexion::dispatch_request(const http::Request &request)
{	
	std::string Extention = request.get_extention();

	if (Extention.empty())
		return NULL;											//CHOSIR QUOI FAIRE (quand il n'existe pas d'extention)
	
	if (Extention == ".py" || Extention == ".php")
			return handle_cgi;

	
	throw std::runtime_error("TODO");						//CHOISIR QUOI FAIRE !!
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

//debug: correction faite pour tester CGI
net::PollTask::Result HttpConnexion::resume(short revents)
{
	/*if (_state == PERFORM_READ && revents & POLLIN)
	{
		ssize_t ret = _client->receive();

		if (ret < 0) // Erreur fatale, on annule la tâche
			return net::PollTask::Result(net::PollTask::FINISHED, NULL);

		// On passe les données réelles au parser
		_request_parser.feed(_client->received_data(), _client->received_length());

		// Si le parser a terminé de lire la requête complète
		if (_request_parser.done())
		{
			_state = PERFORM_WRITE_HEAD;
			
			// On récupère la requête réelle (ce n'est plus un fake)
			const http::Request& request = _request_parser.request();

			// On cherche le handler (CGI ou fichier statique)
			RequestHandler handler = dispatch_request(request);

			// S'il n'y a pas de handler, on renvoie une 404
			if (handler == NULL)
			{
				_response.head().status_code = 404;
				prepare_response();
				return net::PollTask::Result(net::PollTask::WAIT, NULL);
			}

			// On exécute le handler
			net::PollTask *child = handler(_server_config, request, _response);

			if (child != NULL)
			{
				_state = WAIT_FOR_CHILD_RESPONSE;
				return net::PollTask::Result(net::PollTask::WAIT, child);
			}

			// S'il n'y a pas de child (handler synchrone, ex: fichier statique), on prépare la réponse
			prepare_response();
		}
	}*/
	if (_state == PERFORM_READ && revents & POLLIN)
	{
		ssize_t ret = _client->receive();

		if (ret < 0) // fatal error, we cancel the task
			return net::PollTask::Result(net::PollTask::FINISHED, NULL);

		// =========================================================
		// PARCHE POUR TESTER CGI SANS PARSER LA REQUETE HTTP
		// =========================================================
		http::Request fake_request;
		fake_request.head.method = http::METHOD_GET;
		fake_request.head.uri.path = "/cgi-bin/test.py";
		
		_state = PERFORM_WRITE_HEAD;
		const http::Request& request = fake_request;
		
		RequestHandler handler = dispatch_request(request);
		
		if (handler == NULL) 
		{
			_state = PERFORM_WRITE_HEAD;
			_response.head().status_code = 404;
			prepare_response();
			return net::PollTask::Result(net::PollTask::WAIT, NULL);
		}

		net::PollTask *child = handler(_server_config, request, _response);

		if (child != NULL)
		{
			_state = WAIT_FOR_CHILD_RESPONSE;
			return net::PollTask::Result(net::PollTask::WAIT, child);
		}
		prepare_response();
		// =========================================================
	}
	else if (_state == PERFORM_WRITE_HEAD && revents & POLLOUT)			//cette partie est identique comme tu l'as fait auparavant.
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
			//_request_parser.reset(); debug: seulement pour tester le CGI, sinon on doit reset le parser pour la prochaine requête.
		}
		else
		{
			_body_buffer.erase(0, send_ret);
		}
	}
	return net::PollTask::Result(net::PollTask::WAIT, NULL);
}

/*net::PollTask::Result	HttpConnexion::resume(short revents)
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
}*/

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
		case PERFORM_WRITE_HEAD		: return POLLOUT;
		case PERFORM_WRITE_BODY		: return POLLOUT;
		case PERFORM_READ			: return POLLIN;
		case WAIT_FOR_CHILD_RESPONSE: return POLLIN; //debug: On doit garder le socket "vivant" pour le POLLOUT
		default: throw std::runtime_error("unreachable");
	}
}

