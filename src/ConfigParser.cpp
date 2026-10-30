#include "ConfigParser.hpp"
#include <stdexcept>							//comment: only for rutine_error, for the moment, mybe I will change later
#include <cctype>
#include <iostream>

ConfigParser::ConfigParser()
{}

ConfigParser::~ConfigParser()
{}

static bool isDelimiter(char c)
{
    return (std::isspace(static_cast<unsigned char>(c))
        	|| c == '#' || c == '{' || c == '}' || c == ';');

}


//Internal tool to split text into words
std::vector<std::string> ConfigParser::_tokenizer(const std::string &content)
{

	std::vector<std::string> 	token;
	size_t 						i = 0;


	while (i < content.length())
	{

		if (std::isspace(static_cast<unsigned char>(content[i]))) 			//comment: ignore spaces
			i++;

		else if (content[i] == '#')
		{
			while (i < content.length() && content[i] != '\n')
				i++;														//comment: I jump the comment line.
		}
		else if (content[i] == '{' || content[i] == '}' || content[i] == ';')
		{

			token.push_back(std::string(1, content[i]));

			i++;

		}
		else
		{
			size_t start = i;

			while (i < content.length() && !isDelimiter(content[i]))
				i++;

			token.push_back(content.substr(start, i - start));
		}
	}

	return token;
}

//comment: PUBLIC METOHDS

Config ConfigParser::parse(const std::string &file_name)
{
	std::ifstream file(file_name.c_str());

	if (!file.is_open())										//comment: opening file
		throw std::runtime_error("Can't open config file");

	std::string content;
	std::string line;

	while (std::getline(file, line))							//comment: saving file
		content+= line + '\n';
		
	file.close();
		
	Config config;

	std::vector<std::string> tokens = _tokenizer(content);		//comment: tokenizing data

	for(size_t i = 0; i < tokens.size(); i++)
		std::cout << "[" << tokens[i] << "]" << std::endl;

	//comment: parsing logic will go here. A server config with: port, client_body_machins... et server_name

	return config;
}
