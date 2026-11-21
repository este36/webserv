#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "net/PollTask.hpp"
#include "net/Fd.hpp"
#include "ServerConfig.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include <string>
#include <sys/types.h>
#include <map>

class CGIHandler : public net::PollTask
{
	private:
		const 	ServerConfig 	&_server_config;
		const 	http::Request 	&_request;
				http::Response 	&_response;


	public:
		CGIHandler (const 	ServerConfig 	&server_config,
					const 	http::Request 	&request,
							http::Response 	&response);

		Result 		resume 	(short events);
		int 		fd		()				const;
		short		events	()				const;
};

#endif //CGIHANDLER_HPP
