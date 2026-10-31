#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <fstream>
#include "Config.hpp"

class ConfigParser
{
	private:
		std::vector<std::string> 	_tokenizer		(const std::string &content);
		void						_parseServer	(size_t &i, std::vector<std::string> &token, Config 		&config);
		void						_parseLocation	(size_t &i, std::vector<std::string> &token, ServerConfig 	&server);

	public:
		ConfigParser	();
		~ConfigParser	();

		Config parse(const std::string &file_name);
};

#endif //CONFIGPARSER_HPP
