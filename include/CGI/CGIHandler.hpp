#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "net/PollTask.hpp"
#include "net/Fd.hpp"
#include "ServerConfig.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include <string>
#include <sys/types.h>


class CGIHandler : public net::PollTask
{
	private:
		const 	ServerConfig 	&_server_config;
		const 	http::Request 	&_request;
				http::Response 	&_response;

		net::Fd 				_pipe_in;
		net::Fd 				_pipe_out;
		pid_t 					_child_pid;
		std::string 			_output_buffer;
		bool 					_child_finished;

	void _execute_cgi(const std::string &script_path, const std::string &interpreter);	
		

	public:
		CGIHandler (const 	ServerConfig 	&server_config,
					const 	http::Request 	&request,
							http::Response 	&response);

		Result 		resume 	(short events);
		int 		fd		()				const;
		short		events	()				const;
};

#endif //CGIHANDLER_HPP
