#include <iostream>
#include <fstream>
#include <string>
#include "ConfigParser.hpp"

int main()
{
	try
	{
		ConfigParser parser;
		Config config = parser.parse("config/default.conf");

		std::cout << "========== CONFIG PARSED ==========" << std::endl;
		std::cout << "Total servers: " << config.servers.size() << std::endl;

		for (size_t i = 0; i < config.servers.size(); i++)
		{
			std::cout << "\n--- SERVER " << i << " ---" << std::endl;
			std::cout << "  port: " << config.servers[i].port << std::endl;
			std::cout << "  server_name: " << config.servers[i].server_name << std::endl;
			std::cout << "  client_max_body_size: " << config.servers[i].client_max_body_size << std::endl;

			std::cout << "  error_pages:" << std::endl;
			std::map<int, std::string>::const_iterator ep;
			for (ep = config.servers[i].error_pages.begin(); ep != config.servers[i].error_pages.end(); ++ep)
				std::cout << "    " << ep->first << " -> " << ep->second << std::endl;

			std::cout << "  locations: " << config.servers[i].location.size() << std::endl;

			for (size_t j = 0; j < config.servers[i].location.size(); j++)
			{
				const LocationConfig& loc = config.servers[i].location[j];
				std::cout << "\n    --- LOCATION " << j << " ---" << std::endl;
				std::cout << "      path: " << loc.path << std::endl;
				std::cout << "      root: " << loc.root << std::endl;
				std::cout << "      index: " << loc.index << std::endl;
				std::cout << "      upload_store: " << loc.upload_store << std::endl;
				std::cout << "      auto_index: " << loc.auto_index << std::endl;
				std::cout << "      redirect_code: " << loc.redirect_code << std::endl;
				std::cout << "      redirect_url: " << loc.redirect_url << std::endl;

				std::cout << "      accepts_methods: ";
				for (size_t k = 0; k < loc.accepts_methods.size(); k++)
					std::cout << loc.accepts_methods[k] << " ";
				std::cout << std::endl;

				std::cout << "      cgis:" << std::endl;
				std::map<std::string, std::string>::const_iterator cg;
				for (cg = loc.cgis.begin(); cg != loc.cgis.end(); ++cg)
					std::cout << "        " << cg->first << " -> " << cg->second << std::endl;
			}
		}
		std::cout << "\n========== PARSING DONE ==========" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}