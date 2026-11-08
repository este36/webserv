#define DEFAULT_SERVER 8080

#include "ConfigParser.hpp"
#include <stdexcept>							//comment: only for rutine_error, for the moment, mybe I will change later

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

//On controle si le string est vide, et si il a des caratères alphanumeriques.
//Om accepte . - et _ des chifres et lettres  
bool ConfigParser::_controlSpaceAndAlphanumeric(const std::string &name)
{
    if (name.empty())
        return true;

    for (size_t i = 0; i < name.size(); i++) 					//comment: On accepte "isalnum" et seulement "." "_" et "-"
    {
		if (!std::isalnum(static_cast<unsigned char>(name[i])) 
			&& name[i] != '.' && name[i] != '-' && name[i] != '_')
			return (true);
	}
    return false;
}

//pasage d'information vers les servers (vector avec tous les servers)
void ConfigParser::_parseServer(size_t &i, std::vector<std::string> &token, Config &config)
{
	size_t 			listen_control 		= 0;
	size_t			server_name_control = 0;
	ServerConfig 	server;
	i += 2;														//comment: sauter le mot "server" et "{"

	while(i < token.size() && token[i] != "}")
	{
		if (token[i] == "listen")
		{
			listen_control++;
			server.port = atoi(token[++i].c_str());
			i++;												//comment: sauter ";"
		}
		else if (token[i] == "server_name")
		{
			server_name_control++;
			server.server_name = token[++i];
			if (ConfigParser::_controlSpaceAndAlphanumeric(token[i]))
				throw std::runtime_error("Server name not accepted");
			i++;												//comment: sauter ";"
		}
		else if (token[i] == "location")
			_parseLocation(i, token, server);
		else if (token[i] == "error_page")
			control_error_page(token, i, server.error_pages);	//comment: control de "error_pages", pour tout le serveur
		else
			i++;
	}

	if (listen_control > 1)
		throw std::runtime_error("Only one port per server");
	else if (listen_control == 0)
		server.port = DEFAULT_SERVER;

	if (server_name_control > 1)
		throw std::runtime_error("Only one server name is accepted");

	i++;														//comment: sauter "}"	
	config.servers.push_back(server);
}

//On controle le format du root.
//Il doit commencer forcement soit / (absolut path) soit ./ (relative path) ../ et avoir quelque chose après.
bool ConfigParser::_rootFormatControl(const std::string &root)
{
	if (root.empty())
		return true;

	size_t i = 0;
	
	if (root[i] == '.')
	{
		i++;

		if (root[i] == '.')
			if (root[++i] == '/' && root[++i] != '\0')
				return false;
			else
				return true;
		else if (root[i] == '/' && root[++i] != '\0')
			return false;

	}
	else if (root[i] == '/' && root[++i] != '\0')
		return false;

	return true;
}

//Controler si les methods se répetent:
//Seulement une directive error exemple:  GET GET POST.
//Au minimum une directive, au maximum trois.
//Les seules directives acceptées sont: GET, POST et DELETE.
static void control_of_repeated_methods(std::vector<std::string> &methods)
{
	size_t control_GET = 0;
	size_t control_POST = 0;
	size_t control_DELETE = 0;

	size_t i = 0;
	while (i < methods.size())
	{
		if (methods[i] == "GET")
			control_GET++;
		else if (methods[i] == "POST")
			control_POST++;
		else if (methods[i] == "DELETE")
			control_DELETE++;
		else
			throw std::runtime_error("Invalid HTTP method");
		
		i++;
	}
	if (control_GET > 1 || control_POST > 1 || control_DELETE > 1)
    	throw std::runtime_error("Duplicate HTTP method");

	if (control_GET + control_POST + control_DELETE < 1)
    	throw std::runtime_error("At least one HTTP method is required");

}

//Controler sans atoi pour eviter debordement ou error et avoir comme retour "0" de la part d'atoi
static bool isValidErrorCode(const std::string &code)
{
    int value = 0;

    if (code.empty())
        return false;
    for (size_t i = 0; i < code.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(code[i])))
            return false;
        value = value * 10 + (code[i] - '0');
        if (value > 599)
            return false;
    }
    return (value >= 300);
}

//Controler le std::map avec son key and value
//controler au minimum 2 argument, qui ne se repetent pas.
//ça doit commencer par "/" et avoir quelque chose après
//le code d'error doit etre entre 300 et 599
static void control_error_page(const std::vector<std::string> &tokens, size_t &i, std::map<int, std::string> &error_pages)
{
	std::vector<std::string>		args;
	std::map<int, std::string>		pending;

	++i;																//comment: je saute "error_page".
	
	while (i < tokens.size() && tokens[i] != ";")
	{
		args.push_back(tokens[i]);
		++i;
	}
	
	if (args.size() < 2 || i == tokens.size())							//comment: au minimum 2 arguments.
		throw std::runtime_error("Invalid error_page arguments");	
	
	const std::string &destination = args.back();
	
	if (destination[0] != '/' || destination[1] == '\0')				//comment: la route doit commencer par "/" et il doit avoir quelque chose après.
		throw std::runtime_error("Invalid error_page destination");	
	
	for (size_t j = 0; j < args.size() - 1; ++j)
	{
	    if (!isValidErrorCode(args[j]))
	    	throw std::runtime_error("Invalid HTTP error code");		//comment: control de code error entre 300 et 599, et si c'est une chiffre valide.
			
		int	code = std::atoi(args[j].c_str());
	    
		if (error_pages.count(code) || pending.count(code))				//comment: control de repetition dans la même ligne ou dans une autre "error_page" suivant.
	    	throw std::runtime_error("Duplicate error_page code");

		pending[code] = destination;
	}

	error_pages.insert(pending.begin(), pending.end());
	i++; 																//comment: sauter dernière ";"
}

void ConfigParser::_parseLocation(size_t &i, std::vector<std::string> &token, ServerConfig &server)
{
	LocationConfig 		location;
	size_t				root_control = 			0;
	size_t				index_control = 		0;
	size_t				auto_index_control =	0;
	size_t				allow_method_control = 	0;

	i += 2;														//comment: saluter "\" et "{"

	while(i < token.size() && token[i] != "}")
	{
		if (token[i] == "root")
		{
			root_control++;
			location.root = token[++i];
			if (ConfigParser::_rootFormatControl(token [i]))
				throw std::runtime_error("location isn't valid");
			i++;												//comment: sauter ";"
		}
		else if (token[i] == "index")
		{
			index_control++;
			location.index = token[++i];
			i++;												//comment: sauter ";"
		}
		else if (token[i] == "autoindex")
		{
			auto_index_control++;

			if (token[++i] == "on")
				location.auto_index = true;
			else if (token[i] == "off")
				location.auto_index = false;
			else
				throw std::runtime_error("Autoindex only accepts the on or off options");
	
			i++;
		}
		else if (token[i] == "allow_methods")
		{
		    allow_method_control++;
		
			if (allow_method_control > 1)
				throw std::runtime_error("Only one allow_methods directive accepted");
		
			i++;
		
			while (i < token.size() && token[i] != ";")
			{
				location.accepts_methods.push_back(token[i]);
				i++;
			}
		
			if (location.accepts_methods.empty())
				throw std::runtime_error("At least one method is required");

			control_of_repeated_methods(location.accepts_methods);

			i++;
		}
		else if (token[i] == "error_page")
			control_error_page(token, i, location.error_pages);
		else
			i++;
	}
	i++;														//comment: sauter dernière "}"
	
	if (root_control > 1)
		throw std::runtime_error("Only one root is accepted");
	else if (index_control > 1)
		throw std::runtime_error("Only one index is accepted");
	else if (auto_index_control > 1)
		throw std::runtime_error("Only one auto_index is accepted");

	server.location.push_back(location);
}


//comment: -----------------------------------------------------					PUBLIC METOHDS

//parse obtien "path/name.conf" pour commencer avec la parsing
Config ConfigParser::parse(const std::string &file_name)
{
	std::ifstream file(file_name.c_str());

	if (!file.is_open())												//comment: opening file
		throw std::runtime_error("Can't open config file");

	std::string content;
	std::string line;

	while (std::getline(file, line))									//comment: saving file
		content+= line + '\n';
		
	file.close();
		
	Config config;

	std::vector<std::string> tokens = _tokenizer(content);				//comment: tokenizing data
	
	if (!ConfigParser::_controlParserTokens(tokens))
		throw std::runtime_error("Invalid configuration structure");	//comment: validation data

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
