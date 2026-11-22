#include "RequestParserHelper.hpp"
#include "ft/utils.hpp"

#include <limits>
#include <stdexcept>
#include <netdb.h>
#include <sys/socket.h>

namespace request_parser_helper
{

static bool is_ascii_alphanumeric(unsigned char c)
{
    return (c >= '0' && c <= '9')
        || (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z');
}

bool is_valid_header_name(const std::string& name)
{
    if (name.empty())
        return false;

    static const std::string punctuation = "!#$%&'*+-.^_`|~";

    for (std::string::const_iterator it = name.begin();
        it != name.end(); ++it)
    {
        const unsigned char c = static_cast<unsigned char>(*it);
        if (!is_ascii_alphanumeric(c)
            && punctuation.find(static_cast<char>(c)) == std::string::npos)
            return false;
    }
    return true;
}

//comment: Utilise les fonctions addr pour vérifier validité de l'ipv6
static bool is_valid_ipv6_address(const std::string& address)
{
    struct addrinfo hints = {
        AI_NUMERICHOST,
        AF_INET6,
        0,
        0,
        0,
        NULL,
        NULL,
        NULL
    };
    struct addrinfo* result = NULL;

    if (getaddrinfo(address.c_str(), NULL, &hints, &result) != 0)
        return false;

    freeaddrinfo(result);
    return true;
}

//comment: Vérifie les char et les crochets pour host ipv6
static bool is_valid_host_name(const std::string& host)
{
    if (host.empty())
        return false;

    if (host[0] == '[')
    {
        if (host.size() < 3 || host[host.size() - 1] != ']')
            return false;

        const std::string address = host.substr(1, host.size() - 2);
        return is_valid_ipv6_address(address);
    }

    static const std::string sub_delimiters = "!$&'()*+,;=";

    for (std::size_t i = 0; i < host.size(); ++i)
    {
        const unsigned char c = static_cast<unsigned char>(host[i]);

        if (is_ascii_alphanumeric(c)
            || c == '-' || c == '.' || c == '_' || c == '~'
            || sub_delimiters.find(static_cast<char>(c))
                != std::string::npos)
            continue;

        if (c == '%')
        {
            if (i + 2 >= host.size()
                || ft::hex_to_val(host[i + 1]) < 0
                || ft::hex_to_val(host[i + 2]) < 0)
                return false;

            i += 2;
            continue;
        }

        return false;
    }

    return true;
}
//comment: Valide le port (vide aussi), selon grammaire URI
static bool is_valid_port(const std::string& port)
{
    for (std::size_t i = 0; i < port.size(); ++i)
    {
        if (port[i] < '0' || port[i] > '9')
            return false;
    }
    return true;
}

//comment: découpage nom/value, et vérification de la forme
bool is_valid_host_value(const std::string& value)
{
    if (value.empty()
        || value.find_first_of(" \t/?#@") != std::string::npos)
        return false;

    std::size_t host_end = value.size();
    std::size_t port_start = std::string::npos;

    if (value[0] == '[')
    {
        const std::size_t closing_bracket = value.find(']');
        if (closing_bracket == std::string::npos || closing_bracket == 1)
            return false;

        host_end = closing_bracket + 1;

        if (host_end < value.size())
        {
            if (value[host_end] != ':')
                return false;
            port_start = host_end + 1;
        }
    }
    else
    {
        const std::size_t colon = value.find(':');
        if (colon != std::string::npos)
        {
            if (value.find(':', colon + 1) != std::string::npos)
                return false;

            host_end = colon;
            port_start = colon + 1;
        }
    }

    if (host_end == 0)
        return false;

    const std::string host = value.substr(0, host_end);
    if (!is_valid_host_name(host))
        return false;

    if (port_start != std::string::npos)
    {
        const std::string port = value.substr(port_start);
        if (!is_valid_port(port))
            return false;
    }

    return true;
}

std::size_t parse_content_length(const std::string& value)
{
    if (value.empty())
        throw std::runtime_error("empty Content-Length");

    std::size_t result = 0;
    const std::size_t max_value =
        std::numeric_limits<std::size_t>::max();

    for (std::size_t i = 0; i < value.size(); ++i)
    {
        const unsigned char c = static_cast<unsigned char>(value[i]);
        if (c < '0' || c > '9')
            throw std::runtime_error("invalid Content-Length");

        const std::size_t digit = c - '0';
        if (result > (max_value - digit) / 10)
            throw std::runtime_error("Content-Length too big");

        result = result * 10 + digit;
    }
    return result;
}

std::size_t parse_chunk_size(const std::string& line)
{
    const std::size_t extension_start = line.find(';');
    std::size_t hex_end = (extension_start == std::string::npos) ? line.size() : extension_start;

    while (hex_end > 0
        && (line[hex_end - 1] == ' ' || line[hex_end - 1] == '\t'))
        --hex_end;

    if (hex_end == 0)
        throw std::runtime_error("empty chunk size");

    std::size_t result = 0;
    const std::size_t max_value =
        std::numeric_limits<std::size_t>::max();

    for (std::size_t i = 0; i < hex_end; ++i)
    {
        const int digit = ft::hex_to_val(line[i]);
        if (digit < 0)
            throw std::runtime_error("invalid chunk size");

        const std::size_t hex_digit = static_cast<std::size_t>(digit);
        if (result > (max_value - hex_digit) / 16)
            throw std::runtime_error("chunk size too large");

        result = result * 16 + hex_digit;
    }
    return result;
}

bool consume_body_bytes(
    std::string& body,
    const std::string& input_buffer,
    std::size_t& cursor,
    std::size_t& remaining)
{
    if (remaining == 0)
        return true;

    const std::size_t available = input_buffer.size() - cursor;
    if (available == 0)
        return false;

    const std::size_t take =
        (available < remaining) ? available : remaining;

    body.append(input_buffer, cursor, take);
    cursor += take;
    remaining -= take;
    return remaining == 0;
}

}