#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <fstream>
#include <vector>
#include <cctype>
#include <iostream>
#include <stdlib.h>								//comment: c'est pour atoi
#include "Config.hpp"

class ConfigParser
{
	private:
		std::vector<std::string> 	_tokenizer			(const std::string &content);
		void						_parseServer		(size_t &i, std::vector<std::string> &token, Config 		&config);
		void						_parseLocation		(size_t &i, std::vector<std::string> &token, ServerConfig 	&server);
		
		//tokens control
		static bool 				_controlParserTokens(const std::vector<std::string> &tokens);
		static bool 				_parseDirective		(const std::vector<std::string> &tokens, size_t &i);
		static bool 				_parseServerTokens	(const std::vector<std::string> &tokens, size_t &i);
		static bool 				_parseLocationTokens(const std::vector<std::string> &tokens, size_t &i);
		static bool 				_isValidPort		(const std::string &port_str);


	public:
		ConfigParser	();
		~ConfigParser	();

		Config parse(const std::string &file_name);
};

#endif //CONFIGPARSER_HPP
