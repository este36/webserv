#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "ServerConfig.hpp"

class Config
{
	public:
		Config	();
		~Config	();

		std::vector<ServerConfig> servers;
};

#endif //CONFIG_HPP
