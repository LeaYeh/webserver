#include "IPAddress.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>
#include <string>

namespace utils
{

bool IPAddress::is_ipv4(const std::string& ip)
{
    std::string::size_type pos = 0;
    int count = 0;
    int num = 0;

    while (pos < ip.size()) {
        if (std::isdigit(ip[pos])) {
            num = num * 10 + (ip[pos] - '0');
        }
        else if (ip[pos] == '.') {
            if (num > 255) {
                return (false);
            }
            num = 0;
            count++;
        }
        else {
            return (false);
        }
        pos++;
    }
    if (count != 3 || num > 255) {
        return (false);
    }
    return (true);
}

bool IPAddress::is_ipv6(const std::string& ip)
{
    if (ip.empty()) {
        return (false);
    }
    if (ip.length() < 2 || ip.length() > 39) {
        return (false);
    }
    std::string segment;
    int segment_count = 0;
    int double_colon_count = 0;
    bool has_double_colon = false;

    for (size_t i = 0; i < ip.length(); i++) {
        char c = ip[i];
        if (c == ':') {
            if (segment.empty()) {
                if (i == 0 || i == ip.length() - 1 || ip[i + 1] == ':') {
                    return (false);
                }
                if (has_double_colon) {
                    double_colon_count++;
                    if (double_colon_count > 1) {
                        return (false);
                    }
                }
                has_double_colon = true;
            }
            else {
                if (segment_count > 7) {
                    return (false);
                }
                segment.clear();
                segment_count++;
            }
        }
        else if (std::isxdigit(c)) {
            segment.push_back(c);
            if (segment.length() > 4) {
                return (false);
            }
        }
        else {
            return (false);
        }
    }
    int total_segments = segment_count + 1;
    return (total_segments >= 3 && total_segments <= 8);
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

    if (is_ipv6(ip)) {
        return (ip);
    }
    else {
        if (!parse_ipv4(ip, octets)) {
            throw std::invalid_argument("Invalid IPv4 address");
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

std::string IPAddress::to_ipv4(const std::string& ip)
{
    unsigned int octets[4];

    if (is_ipv4(ip)) {
        return (ip);
    }
    else {
        if (!is_ipv6(ip)) {
            throw std::invalid_argument("Invalid IPv6 address");
        }
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
        std::string ipv6 = IPAddress::to_ipv6(addresses[i]);
        std::cout << "Original: " << addresses[i] << " -> IPv6: " << ipv6
                  << std::endl;
    }
}

} // namespace utils
