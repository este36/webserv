#include "ServerConfig.hpp"

ServerConfig::ServerConfig() :
	port(0),
	client_max_body_size(1048576)		//comment: c'est 1Mb
{}

ServerConfig::~ServerConfig()
{}