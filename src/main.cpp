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

		std::cout << "Server: " 	<< config.servers[0].server_name << std::endl;
		std::cout << "Port: " 		<< config.servers[0].port << std::endl;
		std::cout << "Location: " 	<< config.servers[0].location.size() << std::endl;

		std::cout << "Parsing done!" << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		return 1;
	}
	
	return 0;
}
