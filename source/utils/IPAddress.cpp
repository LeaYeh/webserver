#include "IPAddress.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

namespace utils
{

bool IPAddress::is_ipv4(const std::string& ip)
{
    size_t count = 0;
    size_t segment_size = 0;
    size_t pos = 0;
    size_t num = 0;
    bool segment_start = true;

    while (pos < ip.size()) {
        if (std::isdigit(ip[pos])) {
            if (segment_start && ip[pos] == '0' && (pos + 1) < ip.length()
                && std::isdigit(ip[pos + 1])) {
                return (false);
            }
            segment_start = false;
            num = num * 10 + (ip[pos] - '0');
        }
        else if (ip[pos] == '.') {
            if (num > 255 || segment_size > 3 || segment_size == 0) {
                return (false);
            }
            num = 0;
            count++;
            segment_start = true;
            segment_size = -1;
        }
        else {
            return (false);
        }
        pos++;
        segment_size++;
    }
    if (count != 3 || num > 255) {
        return (false);
    }
    return (true);
}

bool IPAddress::is_ipv4(const struct sockaddr_storage& addr)
{
    if (addr.ss_family == AF_INET) {
        return (true);
    }
    return (false);
}

bool IPAddress::is_ipv6(const struct sockaddr_storage& addr)
{
    if (addr.ss_family == AF_INET6) {
        return (true);
    }
    return (false);
}

bool IPAddress::is_ipv6(const std::string& ip)
{
    if (ip.empty() || ip.length() < 2 || ip.length() > 39) {
        return (false);
    }
    std::string segment;
    int segment_count = 0;
    bool has_double_colon = false;
    int segments_before_dc = 0;
    int segments_after_dc = 0;

    for (size_t i = 0; i < ip.length() - 2; i++) {
        if (ip[i] == ':' && ip[i + 1] == ':' && ip[i + 2] == ':') {
            return (false);
        }
    }

    for (size_t i = 0; i < ip.length(); i++) {
        char c = ip[i];

        if (c == ':') {
            if (i + 1 < ip.length() && ip[i + 1] == ':') {
                if (has_double_colon) {
                    return (false); // Only one :: allowed
                }
                has_double_colon = true;
                i++; // Skip next colon
                if (!segment.empty()) {
                    segments_before_dc = segment_count + 1;
                }
                else {
                    segments_before_dc = segment_count;
                }
                segment.clear();
                continue;
            }

            if (!segment.empty() || (!has_double_colon && i > 0)) {
                segment_count++;
            }
            segment.clear();
            continue;
        }

        if (!std::isxdigit(c)) {
            return (false);
        }

        segment += c;
        if (segment.length() > 4) {
            return (false);
        }
    }

    // Count the last segment if exists
    if (!segment.empty()) {
        segment_count++;
    }

    // If we had a ::, calculate segments after it
    if (has_double_colon) {
        segments_after_dc = segment_count - segments_before_dc;
        int compressed_segments = 8 - segments_before_dc - segments_after_dc;
        if (compressed_segments < 1) {
            return (false);
        }
    }
    else {
        // Without ::, we need exactly 8 segments
        if (segment_count != 8) {
            return (false);
        }
    }

    // Check for invalid cases like "2001:db8:::1"
    if (has_double_colon && (segments_before_dc + segments_after_dc) >= 8) {
        return (false);
    }

    return (true);
}

bool IPAddress::is_valid_ip(const std::string& ip)
{
    return (is_ipv4(ip) || is_ipv6(ip));
}

bool IPAddress::is_valid_port(const std::string& port)
{
    std::string::size_type pos = 0;

    while (pos < port.size()) {
        if (!std::isdigit(port[pos])) {
            return (false);
        }
        pos++;
    }
    if (utils::stoi(port) > 65535) {
        return (false);
    }
    return (true);
}

std::string IPAddress::to_ipv6(const std::string& ip)
{
    unsigned int octets[4];

    if (!is_valid_ip(ip)) {
        throw std::invalid_argument("Invalid IP address: " + ip);
    }
    if (is_ipv6(ip)) {
        return (ip);
    }
    else {
        if (!is_ipv4(ip) || !parse_ipv4(ip, octets)) {
            throw std::invalid_argument("Invalid IPv4 address: " + ip);
        }
        std::ostringstream oss;
        oss << "::ffff:";
        oss << std::hex;
        oss << ((octets[0] << 8) | octets[1]);
        oss << ":";
        oss << ((octets[2] << 8) | octets[3]);
        return oss.str();
    }
}

std::string IPAddress::to_ipv6(const struct in6_addr& addr)
{
    const uint16_t* words = reinterpret_cast<const uint16_t*>(&addr);
    std::ostringstream oss;

    for (int i = 0; i < 8; i++) {
        oss << std::hex << words[i];
        if (i > 0 && i < 7) {
            oss << ":";
        }
    }
    return to_ipv6(oss.str());
}

std::string IPAddress::to_ipv4(const struct in_addr& addr)
{
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&addr);
    std::ostringstream oss;

    oss << static_cast<int>(bytes[0]) << "." << static_cast<int>(bytes[1])
        << "." << static_cast<int>(bytes[2]) << "."
        << static_cast<int>(bytes[3]);
    return to_ipv4(oss.str());
}

std::string IPAddress::to_ipv4(const std::string& ip)
{
    unsigned int octets[4];

    if (!is_valid_ip(ip)) {
        throw std::invalid_argument("Invalid IP address: " + ip);
    }
    if (is_ipv4(ip)) {
        return (ip);
    }
    else {
        std::string::size_type pos = ip.find("::ffff:");
        if (pos == std::string::npos) {
            throw std::invalid_argument("Invalid IPv6 address");
        }
        std::string ipv4 = ip.substr(pos + 7);
        std::istringstream iss(ipv4);
        unsigned int num;
        char dot;

        iss >> std::hex >> num >> dot;
        octets[0] = num >> 8;
        octets[1] = num & 0xff;
        iss >> std::hex >> num >> dot;
        octets[2] = num >> 8;
        octets[3] = num & 0xff;
        std::ostringstream oss;
        oss << octets[0] << "." << octets[1] << "." << octets[2] << "."
            << octets[3];
        return oss.str();
    }
}

bool IPAddress::parse_ipv4(const std::string& ip, unsigned int octets[4])
{
    std::istringstream iss(ip);
    char dot;

    for (int i = 0; i < 4; ++i) {
        if (!(iss >> octets[i]) || octets[i] > 255) {
            return (false);
        }
        if ((i < 3 && !(iss >> dot)) || dot != '.') {
            return (false);
        }
    }
    return (iss.eof());
}

void IPAddress::demo(void)
{
    // Test cases
    std::string addresses[] = {
        "192.168.1.1",  // IPv4
        "10.0.0.1",     // IPv4
        "2001:db8::1",  // IPv6
        "::1",          // IPv6
        "fe80::1",      // IPv6
        "256.1.2.3",    // Invalid IPv4
        "2001:db8:::1", // Invalid IPv6
        "192.168.001.1" // Invalid IPv4
    };

    for (int i = 0; i < 8; ++i) {
        try {
            std::string ipv6 = IPAddress::to_ipv6(addresses[i]);
            std::cout << "Original: " << addresses[i] << " -> IPv6: " << ipv6
                      << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            continue;
        }
    }
}

} // namespace utils
