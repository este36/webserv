#ifndef REQUEST_PARSER_HELPER_HPP
#define REQUEST_PARSER_HELPER_HPP

#include <cstddef>
#include <string>

namespace request_parser_helper
{

bool        is_valid_header_name(const std::string& name);
bool        is_valid_host_value(const std::string& value);
std::size_t parse_content_length(const std::string& value);
std::size_t parse_chunk_size(const std::string& line);

bool consume_body_bytes(
    std::string& body,
    const std::string& input_buffer,
    std::size_t& cursor,
    std::size_t& remaining);
}

#endif
