#ifndef PLAINBODY_HPP
#define PLAINBODY_HPP

#include "http/AStreamableBody.hpp"
#include <string>


namespace http
{

class PlainBody : public http::AStreamableBody
{
	private:
		std::string 	_content;
		size_t 			_position;

	public:
		PlainBody(const std::string &content);
		virtual ~PlainBody();

		ssize_t 	read_chunk(std::string &out, size_t max_size);		// Lit un chunk du body, retourne le nombre d'octets lus, ou 0 si la fin du body est atteinte
		size_t 		size() const;										// Retourne la taille totale du body
};

}


#endif //PLAINBODY_HPP