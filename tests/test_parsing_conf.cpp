#include <fstream>
#include <iostream>
#include <string>
#include "ConfigParser.hpp"

int main()
{
	try
	{
		ConfigParser 	parser;
		Config			config = parser.parse("config/default.conf");

		size_t i = 0;
		while (i < config.servers.size())
		{
			std::cout << "i =" << i << std::endl;
			std::cout << "Server			: " << config.servers[i].server_name << std::endl;
			std::cout << "Port			: " << config.servers[i].port << std::endl;
			std::cout << "Location		: " << config.servers[i].location[0].root << std::endl;
			std::cout << "Auto index status	: " << config.servers[i].location[0].auto_index << std::endl;
			i++;
		}
		std::cout << "Parsing done!" << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		return 1;
	}
	
	return 0;
}
