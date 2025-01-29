#pragma once
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

namespace utils
{

class IPAddress
{
public:
    static bool is_ipv4(const std::string& ip);
    static bool is_ipv4(const struct sockaddr_storage& addr);
    static bool is_ipv6(const std::string& ip);
    static bool is_ipv6(const struct sockaddr_storage& addr);
    static bool is_valid_ip(const std::string& ip);
    static bool is_valid_port(const std::string& port);
    static std::string to_ipv6(const std::string& ip);
    static std::string to_ipv6(const struct in6_addr& addr);
    static std::string to_ipv4(const std::string& ip);
    static std::string to_ipv4(const struct in_addr& addr);
    static bool parse_ipv4(const std::string& ip, unsigned int octets[4]);
    static void demo(void);

private:
    IPAddress();
    IPAddress(const IPAddress& other);
    IPAddress& operator=(const IPAddress& other);
    ~IPAddress();
};

} // namespace utils
