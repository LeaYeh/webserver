#pragma once
#include "VirtualHost.hpp"
#include <map>

namespace webkernel
{
class VirtualHostManager
{
public:
    void add_server(const std::string& ipaddr,
                    webconfig::ConfigServerBlock* server_config);
    bool has_server(const std::string& ipaddr) const;
    webconfig::ConfigServerBlock* find_server(const std::string& ipaddr,
                                              const std::string& host) const;
    webconfig::ConfigServerBlock* find_default(int fd) const;

public:
    VirtualHostManager();
    VirtualHostManager(const VirtualHostManager& other);
    VirtualHostManager& operator=(const VirtualHostManager& other);
    ~VirtualHostManager();

private:
    typedef std::map<std::string, VirtualHost*> ListenMap;
    ListenMap _listen_hash; // listen ip address -> VirtualHost
};

} // namespace webkernel
