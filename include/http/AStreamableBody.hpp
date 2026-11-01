#ifndef HTTP_ASTREAMABLE_BODY
#define HTTP_ASTREAMABLE_BODY

#include <string>
#include <cstddef>

namespace http
{

class AStreamableBody
{

public:
	enum ReadResult
	{
		UNSET,
		WAIT,
		DONE,
		ERROR,
	};

	virtual ~AStreamableBody();

	virtual bool has_bytes() const = 0;
	virtual std::size_t size() const = 0;

	// false indicates an error
	virtual bool feed(const std::string& src) = 0;

	virtual ReadResult read(std::string& destination, std::size_t maxBytes) const = 0;
};

}

#endif
