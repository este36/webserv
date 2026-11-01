#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>
#include <map>

class LocationConfig
{
	public:									//comment: for the moment I make all public, later we can make getters and setters
		LocationConfig	();
		~LocationConfig	();

		std::string							path;
		std::string							root;
		std::string							index;
		std::string							upload_store;
		std::string							redirect_url;
		int									redirect_code;
		bool								auto_index;

		std::vector<std::string> 			accepts_methods;
		std::map<std::string, std::string>	cgis;
};


#endif //LOCATIONCONFIG_HPP