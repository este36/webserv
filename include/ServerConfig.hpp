#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "LocationConfig.hpp"
#include <string>
#include <vector>
#include <map>

class ServerConfig
{
	public:
		ServerConfig	();
		~ServerConfig	();

		int 				port;
		size_t				client_name_body_max;
		std::string 		server_name;
		
};

#endif //SERVERCONFIG_HPP
