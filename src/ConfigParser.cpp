#include "ConfigParser.hpp"
#include <stdexcept>							//comment: only for rutine_error, for the moment, mybe I will change later
#include <cctype>
#include <iostream>
#include <stdlib.h>								//comment: c'est pour atoi

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

//pasage d'information vers les servers (vector avec tous les servers)
void ConfigParser::_parseServer(size_t &i, std::vector<std::string> &token, Config &config)
{
	ServerConfig server;
	i += 2;														//comment: sauter le mot "server" et "{"

	while(i < token.size() && token[i] != "}")
	{
		if (token[i] == "listen")
		{
			server.port = atoi(token[++i].c_str());
			i++;												//comment: sauter ";"
		}
		else if (token[i] == "server_name")
		{
			server.server_name = token[++i];
			i++;												//comment: sauter ";"
		}
		else if (token[i] == "location")
		{
			_parseLocation(i, token, server);
		}
		else
			i++;
	}

	if (token[i] == "}")							//comment: sauter dernière "}"
		i++;
	else
		throw std::runtime_error("Error: SERVER configuration isn't close");

	config.servers.push_back(server);
}

void ConfigParser::_parseLocation(size_t &i, std::vector<std::string> &token, ServerConfig &server)
{
	LocationConfig location;

	i += 2;														//comment: saluter "\" et "{"

	while(i < token.size() && token[i] != "}")
	{
		if (token[i] == "root")
		{
			location.root = token[++i];
			i++;												//comment: sauter ";"
		}
		else if (token[i] == "index")
		{
			location.index = token[++i];
			i++;												//comment: sauter ";"
		}
		else if (token[i] == "allow_methods")
		{
			location.accepts_methods.push_back(token[++i]);
			i++;												//comment: sauter ";"
		}
		else
			i++;
	}
	if (token[i] == "}")										//comment: sauter dernière "}"
		i++;
	else
		throw std::runtime_error("Error: LOCATION configuration isn't close");	

	server.location.push_back(location);
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

	size_t i = 0;
	while (i < tokens.size())
	{
		if (tokens[i] == "server")
			_parseServer(i, tokens, config);
		else
			i++;
	}

	return config;
}
