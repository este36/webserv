#include <iostream>
#include <exception>
#include "ConfigParser.hpp"
#include "Server.hpp"

//debug: main test pour le serveur. Il parse le fichier de configuration, crée un serveur et le lance.
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
        return 1;
    }

    try
    {
		ConfigParser 	parser;
		Config 			config = parser.parse(argv[1]);
        
		Server server(config);
		server.run(); 								// ESTO LEVANTA EL SERVIDOR
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}