#include "CGI/CGIHandler.hpp"
#include "HttpConnexion.hpp"
#include "LocationConfig.hpp"
#include "RequestHandler.hpp"


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
			uri_path.substr(0, loc_path.length()) == loc_path) 	// Vérifie si l'URI commence par le chemin de la location
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


CGIHandler::CGIHandler(const ServerConfig& server_config,
                       const http::Request& request,
                       http::Response& response)
    : _server_config(server_config)
    , _request(request)
    , _response(response)
	, _pipe_in()
	, _pipe_out()
	, _child_pid(-1)
	, _child_finished(false)
{
	const LocationConfig *loc = _find_location(_server_config, _request.head.uri.path);

	if (loc == NULL)
	{
		_response.head().status_code = 404;
		_child_finished = true;
		return ;
	}

	std::string path 	= _request.head.uri.path;
	size_t 		dot_pos = path.find_last_of('.');

	if (dot_pos == std::string::npos)
	{
		_response.head().status_code = 404;
		_child_finished = true;
		return ;
	}

	std::string extension = path.substr(dot_pos);

	std::map<std::string, std::string>::const_iterator it = loc->cgis.find(extension);

	if (it == loc->cgis.end())
	{
		_response.head().status_code = 404;
		_child_finished = true;
		return ;
	}

	std::string script_path = loc->root + path;		//comment: concatenation, route absolute qui a besoin exceve

	_execute_cgi(script_path, it->second);
}


net::PollTask::Result CGIHandler::resume(short revents)
{
    (void)revents;
    return Result(net::PollTask::WAIT);
}


int CGIHandler::fd() const
{
	if (_child_finished)
    	return -1;

	return _pipe_out.get();
}


short CGIHandler::events() const
{
	if (_child_finished)
    	return 0;

	return POLLIN;
}


net::PollTask *handle_cgi(const ServerConfig& server_config,
                          const http::Request& request,
                          http::Response& response)
{
    return new CGIHandler(server_config, request, response);
}


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

	_child_pid = fork();											//Ici le fils

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
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);

		close(pipe_in[0]);
		close(pipe_in[1]);

		close(pipe_out[0]);
		close(pipe_out[1]);

		// TODO: Armar variables de entorno (lo haré mañana) ... en una funcion a parte
        char *envp[] = { NULL };

        char *argv[] = {											//ça c'est le "interpreter", "route (path)"
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


	// TODO: Por ahora asumimos que es GET. Si es POST, hay que escribir el body en _pipe_in.
    // Como es GET, cerramos el pipe_in inmediatamente.
}
