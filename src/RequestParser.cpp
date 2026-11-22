#include "RequestParser.hpp"
#include "RequestParserHelper.hpp"
#include "ft/utils.hpp"
#include <stdexcept>
#include <utility>
#include <vector>

RequestParser::RequestParser() :
  _cursor(0),
  _remaining(0),
  _state(REQUEST_LINE)
{}

//comment: Identifie les méthodes, versions et url puis les ajoute dans Request.headers
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

//comment: Parse les transfer_encodings pour en faire des listes si besoin.
static void append_transfer_codings(
    const std::string& field_value,
    std::vector<std::string>& codings)
{
    std::size_t start = 0;

    while (start <= field_value.size())
    {
        const std::size_t comma = field_value.find(',', start);
        std::size_t end =
            (comma == std::string::npos) ? field_value.size() : comma; //comment: Sert à trouver les header values successives et valides pour les réunir comme "Transfer_Encoding [gzip, chunked]"

        while (start < end
            && (field_value[start] == ' ' || field_value[start] == '\t'))
            ++start;

        while (end > start
            && (field_value[end - 1] == ' ' || field_value[end - 1] == '\t'))
            --end;

        const std::string coding = field_value.substr(start, end - start);
        if (!request_parser_helper::is_valid_header_name(coding))
            throw std::runtime_error("invalid Transfer-Encoding");

        codings.push_back(ft::to_lower(coding));

        if (comma == std::string::npos)
            break;

        start = comma + 1;
    }
}
//comment: Vérifie les règles pour les headers Content_Length et Transfer_Encoding, ensuite state passe à CHUNK_SIZE
void RequestParser::finish_headers()
{
    bool has_content_length = false;
    bool has_transfer_encoding = false;
    std::size_t content_length = 0;
    std::vector<std::string> codings;
    std::size_t host_count = 0;
    std::string host_value;

    for (http::Headers::const_iterator it =
            _pending_request.head.headers.begin();
         it != _pending_request.head.headers.end(); ++it)
    {
        const std::string name = ft::to_lower(it->first);

        if (name == "host")
        {
            ++host_count;
            if (host_count > 1)
                throw std::runtime_error("duplicate Host");
            host_value = it->second;
            if (!request_parser_helper::is_valid_host_value(host_value))
              throw std::runtime_error("invalid Host");
        }
        else if (name == "content-length")
        {
            if (has_content_length)
                throw std::runtime_error("duplicate Content-Length");

            content_length = request_parser_helper::parse_content_length(
                it->second);
            has_content_length = true;
        }
        else if (name == "transfer-encoding")
        {
            has_transfer_encoding = true;
            append_transfer_codings(it->second, codings);
        }
    }

    const bool requires_host =
        _pending_request.head.version_major == 1
        && _pending_request.head.version_minor == 1;
    if (requires_host && host_count == 0)
        throw std::runtime_error("HTTP/1.1 requires one Host");

    //comment: Ce conflit n'est connu qu'après avoir parcouru tous les headers.
    if (has_content_length && has_transfer_encoding)
        throw std::runtime_error(
            "Content-Length and Transfer-Encoding both found");

    if (has_transfer_encoding)
    {
        if (_pending_request.head.version_minor == 0)
            throw std::runtime_error("Transfer-Encoding requires HTTP/1.1");

        if (codings.empty() || codings.back() != "chunked")
            throw std::runtime_error("request Transfer-Encoding must end with chunked");

        bool has_chunked = false;

        for (std::size_t i = 0; i < codings.size(); ++i)
        {
            if (codings[i] == "chunked")
            {
                if (has_chunked)
                    throw std::runtime_error("chunked coding repeated");
                has_chunked = true;
            }
            else if (codings[i] == "gzip" || codings[i] == "x-gzip")
            {
                throw std::runtime_error("TODO: gzip decoding");
            }
            else
                throw std::runtime_error("unsupported transfer coding");
        }

        _remaining = 0;
        _state = CHUNK_SIZE;
        return;
    }

    if (has_content_length)
    {
        _remaining = content_length;
        _state = (_remaining == 0) ? COMPLETE : FIXED_BODY;
        return;
    }

    _state = COMPLETE;
}
//comment: Check des char dans headers.
void RequestParser::parse_header_line(
    const std::string& line,
    http::Headers& destination) const
{
    const std::size_t colon = line.find(':');
    if (colon == std::string::npos || colon == 0)
        throw std::runtime_error("invalid header line");

    const std::string name = line.substr(0, colon);
    if (!request_parser_helper::is_valid_header_name(name))
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
    destination.push_back(std::make_pair(name, value));
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
//comment: Boucle principale: envoie les octets reçus de façon incrémentale par recv et parse les noms et valeurs des headers, puis le body.
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
        parse_header_line(line, _pending_request.head.headers);
    }
    else if (_state == CHUNK_SIZE)
    {
      if (!read_line(line))
          break;
      _remaining = request_parser_helper::parse_chunk_size(line);
      if (_remaining == 0)
          _state = TRAILERS;
      else
          _state = CHUNK_DATA;
    }
    else if (_state == FIXED_BODY)
    {
        if (!request_parser_helper::consume_body_bytes(
                _pending_request.body, _input_buffer, _cursor, _remaining))
            break;
        _state = COMPLETE;
    }
    else if (_state == CHUNK_DATA)
    {
        if (!request_parser_helper::consume_body_bytes(
                _pending_request.body, _input_buffer, _cursor, _remaining))
            break;
        _state = CHUNK_DATA_CRLF;
    }
    else if (_state == CHUNK_DATA_CRLF)
    {
        const std::size_t available = _input_buffer.size() - _cursor;
        if (available < 2)
            break;
        if (_input_buffer[_cursor] != '\r' || _input_buffer[_cursor + 1] != '\n')
            throw std::runtime_error("invalid chunk data terminator");
        _cursor += 2;
        _state = CHUNK_SIZE;
    }
    else if (_state == TRAILERS)
    {
        if (!read_line(line))
            break;
        if (line.empty())
            _state = COMPLETE;
        else
            parse_header_line(line, _pending_request.trailers);
    }
    else
    {
      break;
    }
  }
}

bool	RequestParser::done()
{
	return _state == COMPLETE;
}

void RequestParser::reset()
{
    if (!done())
        throw std::logic_error("incomplete request cannot reset");

    _input_buffer.erase(0, _cursor);
    _cursor = 0;
    _remaining = 0;
    _pending_request = http::Request();
    _state = REQUEST_LINE;
}

const http::Request&	RequestParser::request()
{
	if (!done())
		throw std::logic_error("request is not complete");
	return _pending_request;
}
