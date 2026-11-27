#include "http/PlainBody.hpp"

namespace http
{
PlainBody::PlainBody(const std::string &content)
	: _content(content)
	, _position(0)
{}

PlainBody::~PlainBody()
{}

ssize_t PlainBody::read_chunk(std::string &out, size_t max_size)
{
	if (_position >= _content.size())
		return 0; 																// Fin du body

	size_t bytes_to_read = std::min(max_size, _content.size() - _position);		// Calculer le nombre d'octets à lire (le minimum entre max_size et le reste du body)
	out.append(_content, _position, bytes_to_read);
	_position += bytes_to_read;

	return bytes_to_read;
}


size_t PlainBody::size() const
{
	return _content.size();

}

}