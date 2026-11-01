#include "ConfigParser.hpp"

// Validation du port (il doit ẽtre entre 1024 et 65535)
bool ConfigParser::_isValidPort(const std::string &port_str)
{
	if (port_str.empty())
		return false;
	
	for (size_t i = 0; i < port_str.size(); i++)
	{
		if (!std::isdigit(port_str[i]))
			return false;
	}
	
	int port = std::atoi(port_str.c_str());
	return (port >= 1024 && port <= 65535);
}

// Check si la directive est bien écrite et finit par un ';'.
bool ConfigParser::_parseDirective(const std::vector<std::string>& tokens, size_t& i)
{
	i++;

	if (i >= tokens.size() || tokens[i] == ";" || tokens[i] == "{" || tokens[i] == "}")
		return false;

	while (i < tokens.size() && tokens[i] != ";")
	{
		if (tokens[i] == "{" || tokens[i] == "}")
			return false;
		i++;
	}

	if (i >= tokens.size())
		return false;

	i++;
	return true;
}

// Check la structure du bloc "location /path { ... }".
bool ConfigParser::_parseLocationTokens(const std::vector<std::string> &tokens, size_t &i)
{
	i++;																	//comment: i pointe à path ...

	if (i >= tokens.size() || tokens[i] == "{" || tokens[i] == "}")
		return false; 														//comment: the path ne peut pas commencer par "{"	
	
	if (i + 1 >= tokens.size() || tokens[i + 1] != "{")
		return false;

	i++;																	//comment: sauter "{"

	while (i < tokens.size() && tokens[i] != "}")
	{
		if (tokens[i] == "location" || tokens[i] == "server")
			return false;

		if (!_parseDirective(tokens, i))
			return false;
	}

	if (i >= tokens.size())
		return false;

	i++;
	return true;
}

// Check la structure du bloc "server { ... }".
bool ConfigParser::_parseServerTokens(const std::vector<std::string> &tokens, size_t &i)
{
	i++;

	if (i >= tokens.size() || tokens[i] != "{")
		return false;

	i++;

	while (i < tokens.size() && tokens[i] != "}")
	{
		if (tokens[i] == "location")
		{
			if (!_parseLocationTokens(tokens, i))
				return false;
		}
		else if (tokens[i] == "server")
			return false;
		else if (tokens[i] == "listen")
		{
			i++; 												//comment: j'avance jusqu'à la valeur
			if (i >= tokens.size() || !_isValidPort(tokens[i])) //comment: validation
				return false;
			if (!_parseDirective(tokens, --i)) 					//comment: je reviens de 1, pouer controle de ";"
				return false;
		}
		else if (!_parseDirective(tokens, i))
			return false;
	}

	if (i >= tokens.size())
		return false;

	i++;
	return true;
}

// Check global : y'a au moins un serveur et tout est valide.
bool ConfigParser::_controlParserTokens(const std::vector<std::string> &tokens)
{
	size_t i = 0;
	size_t server_count = 0;

	while (i < tokens.size())
	{
		if (tokens[i] != "server")
			return false;

		if (!_parseServerTokens(tokens, i))
			return false;

		server_count++;
	}

	return (server_count > 0);
}