#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "LocationConfig.hpp"
#include <string>
#include <vector>
#include <map>

struct ServerConfig
{
	ServerConfig	();
	~ServerConfig	();

	int 							port;
	size_t							client_max_body_size;
	std::string 					server_name;
	std::vector<LocationConfig>		location;
	std::map<int, std::string>		error_pages;			//comment: "error pages" contemplé pour tout le serveur
};

#endif //SERVERCONFIG_HPP
