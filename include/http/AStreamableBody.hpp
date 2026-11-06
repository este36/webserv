#ifndef HTTP_ASTREAMABLE_BODY
#define HTTP_ASTREAMABLE_BODY

#include <string>
#include <cstddef>

namespace http
{

class AStreamableBody
{

public:
	virtual ~AStreamableBody();

    // append data to `out` until `max_bytes`, returns read count, -1 if error, 0 if finished
	virtual ssize_t read_chunk(std::string& out, std::size_t max_bytes) = 0;

	// pointer to the first character of the underlying buffer
	// return NULL if the underlying buffer is not a string (example: files or pipes)
	virtual const char *data() const = 0;

	// mostly needed for content-length header in http responses
	virtual std::size_t size() const = 0;
};

}

#endif
