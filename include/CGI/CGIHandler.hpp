#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "net/PollTask.hpp"
#include "net/Fd.hpp"
#include "ServerConfig.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/PlainBody.hpp" 		//pour lire le body de la requête et l'envoyer au pipe d'entrée du CGI
#include <string>					//pour stocker la sortie du CGI dans un buffer avant de l'envoyer au client
#include <sys/types.h>				//pid_t
#include <unistd.h>  				//fork, pipe, dup2, execve
#include <sys/wait.h>				 //waitpid
#include <fcntl.h>   				//fcntl (set_non_blocking)
#include <cstdlib>   				//exit
#include <cerrno>					//errno


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
		size_t 					_body_offset; 					// pour suivre combien de données du body de la requête ont été écrites dans le pipe d'entrée du CGI

	void _execute_cgi(const std::string &script_path, const std::string &interpreter);	
		

	public:
		CGIHandler (const 	ServerConfig 	&server_config,
					const 	http::Request 	&request,
							http::Response 	&response,
					const 	std::string 	&script_path,
					const 	std::string 	&interpreter);

		Result 		resume 	(short events);
		int 		fd		()				const;
		short		events	()				const;
};

#endif //CGIHANDLER_HPP
