#pragma once
#include "ConfigServerBlock.hpp"
#include <map>

namespace webkernel
{
class VirtualHost
{
public:
    const std::string& ipaddr(void) const;
    const std::string& name(void) const;
    webconfig::ConfigServerBlock* config(void) const;

public:
    VirtualHost(const std::string& ipaddr,
                webconfig::ConfigServerBlock* config);
    VirtualHost(const std::string& ipaddr);

    VirtualHost(const VirtualHost& other);
    VirtualHost& operator=(const VirtualHost& other);
    ~VirtualHost();

private:
    std::string _ipaaddr;
    std::string _name;
    webconfig::ConfigServerBlock* _config;

private:
    VirtualHost();
};

} // namespace webkernel
