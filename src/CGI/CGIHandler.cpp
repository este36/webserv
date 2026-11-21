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
    return -1;
}

short CGIHandler::events() const
{
    return 0;
}

net::PollTask *handle_cgi(const ServerConfig& server_config,
                          const http::Request& request,
                          http::Response& response)
{
    return new CGIHandler(server_config, request, response);
}

void CGIHandler::_execute_cgi(const std::string &script_path, const std::string &interpreter)
{
	(void)script_path;
	(void)interpreter;
}