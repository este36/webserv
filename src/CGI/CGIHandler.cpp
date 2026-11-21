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
			if (loc_path.length() > longest_match)				//Si le path est encore plus specifique (plus long...), encore mieux.
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
{
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

