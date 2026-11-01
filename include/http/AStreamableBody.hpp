#ifndef HTTP_ASTREAMABLE_BODY
#define HTTP_ASTREAMABLE_BODY

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

	virtual bool has_bytes() const;
	virtual std::size_t size() const;

	// false indicates an error
	virtual bool feed(std::string src);

	virtual ReadResult read(std::string destination, std::size_t maxBytes) const = 0;
};

}

#endif
