#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <fstream>
#include "Config.hpp"

class ConfigParser
{
	private:
		std::vector<std::string> _tokenizer(const std::string &content);

	public:
		ConfigParser	();
		~ConfigParser	();

		Config parse(const std::string &file_name);
};

#endif //CONFIGPARSER_HPP
