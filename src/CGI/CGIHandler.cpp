#include "CGI/CGIHandler.hpp"
#include "HttpConnexion.hpp"
#include "LocationConfig.hpp"
#include "RequestHandler.hpp"
#include "ft/utils.hpp"
#include <iostream>


// Retourne la LocationConfig la plus spécifique (préfixe le plus long) qui correspond à l'URI.
// Retourne NULL si aucune location ne correspond.
const LocationConfig *_find_location(const ServerConfig &serverConfig, const std::string &uri_path)
{
	const LocationConfig 	*match 			= NULL;
	size_t 					longest_match 	= 0;

	for (size_t i = 0; i < serverConfig.location.size(); i++)
	{
		const LocationConfig	&current_loc 	= serverConfig.location[i];
		const std::string 		&loc_path 		= current_loc.path;


		if (uri_path.empty())
			continue;

		if (uri_path.length() >= loc_path.length() && 
			uri_path.substr(0, loc_path.length()) == loc_path) 	//comment: Vérifie si l'URI commence par le chemin de la location
		{
			if (loc_path.length() > longest_match)				//Si le path est encore plus specifique (plus long...), encore mieux (comme GNIX).
			{
				match 		= &current_loc;
				longest_match 	= loc_path.length();
			}
			
		}
	}

	return match;
}

/* 
	CGIHandler est un PollTask qui gère l'exécution d'un script CGI.
	Il crée un processus fils pour exécuter le script, redirige les entrées/sorties via pipes.
*/
CGIHandler::CGIHandler(	const ServerConfig& server_config,
                       	const http::Request& request,
                       	http::Response& response,
						const std::string &script_path,
						const std::string &interpreter)
    : _server_config(server_config)
    , _request(request)
    , _response(response)
	, _pipe_in()
	, _pipe_out()
	, _child_pid(-1)
	, _child_finished(false)
	, _body_offset(0)
{
	_execute_cgi(script_path, interpreter);
}


//Resume() est appelé par le Poller pour gérer les événements sur le pipe de sortie du CGI. 
//Il lit les données du pipe et les stocke dans _output_buffer. Quand le pipe est fermé 
//(le fils a fini d'écrire), il parse la sortie du CGI et construit la réponse HTTP.
net::PollTask::Result CGIHandler::resume(short revents)
{
	(void)revents; // Supprimer l'avertissement de variable non utilisée

	if (revents & POLLOUT && _pipe_in.get() != -1)
	{
		ssize_t bytes_written = write(_pipe_in.get(), _request.body.c_str() + _body_offset, _request.body.size() - _body_offset);
    
		if (bytes_written > 0)
			_body_offset += bytes_written;

		if (_body_offset >= _request.body.size())
		    _pipe_in.reset();
        
    return Result(net::PollTask::WAIT);
	}

	if (_child_finished)
		return net::PollTask::FINISHED;

	char 		buffer[4096];
	ssize_t 	bytes_read = read(_pipe_out.get(), buffer, sizeof(buffer)); //comment: lire le pipe de sortie du CGI (le fils écrit dans ce pipe, le père lit)

	if (bytes_read < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)				//comment: Si le pipe est vide, on attend la prochaine fois que le pipe sera prêt à être lu
			return Result(net::PollTask::WAIT);
		
		_response.head().status_code 	= 500;
		_child_finished 				= true;

		return Result(net::PollTask::FINISHED);
	}
	else if (bytes_read == 0)										//comment: Si le pipe est fermé (le fils a fini d'écrire), on parse la sortie du CGI et on construit la réponse HTTP
	{
		_child_finished = true;
		waitpid(_child_pid, NULL, 0);

		size_t header_end = _output_buffer.find("\r\n\r\n");
		if (header_end == std::string::npos)						//comment: Si la sortie du CGI ne contient pas de headers HTTP valides, on retourne une erreur 500
			_response.head().status_code = 500;
		else
		{
			std::string headers_raw = _output_buffer.substr(0, header_end);
			std::string 	body = _output_buffer.substr(header_end + 4);

			size_t colon = headers_raw.find(':');
			if (colon != std::string::npos)
			{
				std::string header_name = headers_raw.substr(0, colon);
				std::string header_value = headers_raw.substr(colon + 1);
				
				_response.head().headers.push_back(std::make_pair(header_name, header_value)); //comment: On ajoute le header retourné par le CGI à la réponse HTTP
			}

			_response.head().status_code = 200;
			_response.set_body(new http::PlainBody(body));
		}

		return Result(net::PollTask::FINISHED);
	}
	else															//comment: Si on a lu des données, on les ajoute à _output_buffer et on continue à attendre la prochaine fois que le pipe sera prêt à être lu
	{
		_output_buffer.append(buffer, bytes_read);

		return Result(net::PollTask::WAIT);
	}
}

//Retourne le descripteur de fichier du pipe de sortie du CGI. 
//Le Poller utilise ce descripteur pour surveiller les événements sur le pipe.
int CGIHandler::fd() const
{
	if (_child_finished)
    	return -1;

	if (_pipe_in.get() != -1)			// Si le pipe d'entrée est ouvert, on retourne son descripteur pour surveiller les événements d'écriture
		return _pipe_in.get();

	return _pipe_out.get();
}

//Retourne les événements que le Poller doit surveiller sur le pipe de sortie du CGI.
short CGIHandler::events() const
{
	if (_child_finished)
    	return 0;

	if (_pipe_in.get() != -1)
		return POLLOUT; // Si le pipe d'entrée est ouvert, on veut écrire dedans (pour envoyer le body de la requête au CGI)
	else
		return POLLIN;
}

// Cette fonction est appelée par le RequestHandler pour créer un 
// CGIHandler pour gérer l'exécution d'un script CGI.
net::PollTask *handle_cgi(const ServerConfig& server_config,
                          const http::Request& request,
                          http::Response& response)
{
	std::string extention = request.get_extention();
	if (extention.empty())
		return NULL;

	const LocationConfig *location = _find_location(server_config, request.head.uri.path);
	if (!location || location->cgis.empty())
		return NULL;

	std::map<std::string, std::string>::const_iterator it = location->cgis.find(extention);
	if (it == location->cgis.end())
		return NULL;

	std::string script_path = location->root + request.head.uri.path;
	std::string interpreter = it->second;

    return new CGIHandler(server_config, request, response, script_path, interpreter);
}

// Cette fonction vérifie si l'URI de la requête correspond à une location configurée pour le CGI. 
// Si c'est le cas, elle retourne true, sinon false. Le RequestHandler utilise cette fonction pour
// déterminer si la requête doit être traitée par le CGIHandler.
void CGIHandler::_execute_cgi(const std::string &script_path, const std::string &interpreter)
{
	int		pipe_in[2];
	int 	pipe_out[2];

	if (pipe(pipe_in) < 0 || pipe(pipe_out) <0)
	{
		_response.head().status_code 	= 500;
		_child_finished 				= true;
		return;
	}

	_child_pid = fork();											//comment: Ici le fils

	if (_child_pid < 0)
	{
		close(pipe_in[0]);
		close(pipe_in[1]);

		close(pipe_out[0]);
		close(pipe_out[1]);

		_response.head().status_code 	= 500;
		_child_finished 				= true;
		return;
	}

	if (_child_pid == 0)
	{
		dup2(pipe_in[0], STDIN_FILENO);								//comment: Redirige stdin vers le pipe d'entrée pour que le CGI lise les données de la requête
		dup2(pipe_out[1], STDOUT_FILENO);							//comment: Redirige stdout vers le pipe de sortie pour que le CGI écrive sa sortie dans le pipe	

		dup2(pipe_out[1], STDERR_FILENO); 							//comment: Redirige stderr vers le pipe de sortie pour capturer les erreurs du CGI

		close(pipe_in[0]);
		close(pipe_in[1]);

		close(pipe_out[0]);
		close(pipe_out[1]);

		std::string env_path = "PATH=/usr/local/bin:/usr/bin:/bin";
		
		std::string env_method_str = "REQUEST_METHOD=";
		if (_request.head.method == http::METHOD_GET) 
			env_method_str += "GET";
		else if (_request.head.method == http::METHOD_POST)
			env_method_str += "POST";
		else if (_request.head.method == http::METHOD_DELETE)
			env_method_str += "DELETE";

		std::string env_query = "QUERY_STRING=" + _request.head.uri.query;
		std::string env_length = "CONTENT_LENGTH=" + ft::to_string(_request.body.size()); // Si es POST
		std::string env_gateway = "GATEWAY_INTERFACE=CGI/1.1";
		std::string env_script = "SCRIPT_FILENAME=" + script_path;

		char *envp[] = {
			const_cast<char*>(env_path.c_str()),
			const_cast<char*>(env_method_str.c_str()),
			const_cast<char*>(env_query.c_str()),
			const_cast<char*>(env_length.c_str()),
			const_cast<char*>(env_gateway.c_str()),
			const_cast<char*>(env_script.c_str()),
			NULL
		};

        char *argv[] = {											//comment: ça c'est le "interpreter", "route (path)"
            const_cast<char*>(interpreter.c_str()),
            const_cast<char*>(script_path.c_str()),
            NULL
        };

		execve(interpreter.c_str(), argv, envp);					//comment: execution! (exceve), le fils fini ici, sauf erreur

		exit(1);													//comment: le processus a eu un erreur
	}

	close(pipe_in[0]);  											//comment: fermer les pipes inutiles. extreme de lecture du pipe d'entrée (le père n'en a pas besoin)
	close(pipe_out[1]);												//comment: Cerramos el extremo de escritura del pipe de entrada (el hijo lo usa)

																	
	if (_request.head.method == http::METHOD_GET || _request.head.method == http::METHOD_DELETE) //comment: Si es GET, cerramos el pipe_in inmediatamente (el hijo verá EOF en stdin)
	    close(pipe_in[1]); 											//comment: cerramos el extremo de escritura directamente
	else
	    _pipe_in.reset(pipe_in[1]); // Pour POST, on garde le pipe ouvert pour écrire le body de la requête dans le pipe d'entrée du CGI, en resume().

	_pipe_out.reset(pipe_out[0]);
	_pipe_out.set_non_blocking();
}

