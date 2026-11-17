#include "RequestParser.hpp"
#include <stdexcept>
#include <utility>

RequestParser::RequestParser() :
  _cursor(0),
  _remaining(0),
  _state(REQUEST_LINE)
{}

void RequestParser::parse_request_line(const std::string& line)
{
    const std::size_t first_space = line.find(' ');
    if (first_space == std::string::npos || first_space == 0)
        throw std::runtime_error("invalid request line");

    const std::string method = line.substr(0, first_space);
    if (method == "GET")
        _pending_request.head.method = http::METHOD_GET;
    else if (method == "POST")
        _pending_request.head.method = http::METHOD_POST;
    else if (method == "DELETE")
        _pending_request.head.method = http::METHOD_DELETE;
    else
        throw std::runtime_error("invalid method");

    const std::size_t second_space = line.find(' ', first_space + 1);
    if (second_space == std::string::npos
        || second_space == first_space + 1
        || line.find(' ', second_space + 1) != std::string::npos)
        throw std::runtime_error("invalid request line");

    const std::string raw_target =
        line.substr(first_space + 1, second_space - first_space - 1);

    const std::string version = line.substr(second_space + 1);

    if (version == "HTTP/1.0")
    {
        _pending_request.head.version_major = 1;
        _pending_request.head.version_minor = 0;
    }
    else if (version == "HTTP/1.1")
    {
        _pending_request.head.version_major = 1;
        _pending_request.head.version_minor = 1;
    }
    else
        throw std::runtime_error("invalid HTTP version");

    const std::pair<http::URL, bool> parsed_target =
        http::URL::create_from_raw_target(raw_target);
    if (!parsed_target.second) // comment: check valid target url
        throw std::runtime_error("invalid request target");

    _pending_request.head.uri = parsed_target.first;
}

static bool is_valid_header_name(const std::string& name)
{
    if (name.empty())
        return false;

    static const std::string punctuation = "!#$%&'*+-.^_`|~";

    for (std::string::const_iterator it = name.begin();
        it != name.end(); ++it)
    {
        const unsigned char c = static_cast<unsigned char>(*it);
        const bool is_alphanumeric =
            (c >= '0' && c <= '9')
            || (c >= 'A' && c <= 'Z')
            || (c >= 'a' && c <= 'z');
        if (!is_alphanumeric
            && punctuation.find(static_cast<char>(c)) == std::string::npos)
            return false;
    }
    return true;
}

void RequestParser::parse_header_line(const std::string& line)
{
    const std::size_t colon = line.find(':');
    if (colon == std::string::npos || colon == 0)
        throw std::runtime_error("invalid header line");

    const std::string name = line.substr(0, colon);
    if (!is_valid_header_name(name))
        throw std::runtime_error("invalid header name");

    std::size_t value_start = colon + 1;
    std::size_t value_end = line.size();

    while (value_start < value_end
        && (line[value_start] == ' ' || line[value_start] == '\t'))
        ++value_start;

    while (value_end > value_start
        && (line[value_end - 1] == ' ' || line[value_end - 1] == '\t'))
        --value_end;

    const std::string value =
        line.substr(value_start, value_end - value_start);

    for (std::size_t i = 0; i < value.size(); ++i)
    {
        const unsigned char c = static_cast<unsigned char>(value[i]);
        if ((c < 32 && c != '\t') || c == 127)
            throw std::runtime_error("invalid header value");
    }

    _pending_request.head.headers.push_back(std::make_pair(name, value));
}

bool RequestParser::read_line(std::string& line)
{
  const std::size_t end = _input_buffer.find("\r\n", _cursor);
  if (end == std::string::npos)
    return false;

  line.assign(_input_buffer, _cursor, end - _cursor);
  _cursor = end + 2;
  return true;
}

void	RequestParser::feed(const char *src, const size_t len)
{
  if (len > 0)
    _input_buffer.append(src, len);

  while (_state != COMPLETE)
  {
    std::string line;

    if (_state == REQUEST_LINE)
    {
      if (!read_line(line))
        break;

        parse_request_line(line);
        _state = HEADERS;
    }
    else if (_state == HEADERS)
    {
      if (!read_line(line))
        break;
      if (line.empty())
        finish_headers();
      else
        parse_header_line(line);
    }
    else
    {
      break;
    }
  }

}

bool	RequestParser::done()
{
	throw std::runtime_error("TODO");
}

void	RequestParser::reset()
{
	throw std::runtime_error("TODO");
}

const http::Request&	RequestParser::request()
{
	throw std::runtime_error("TODO");
}
