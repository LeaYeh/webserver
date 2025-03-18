#include "VirtualHost.hpp"

namespace webkernel
{

VirtualHost::VirtualHost(const std::string& ipaddr) : _ipaaddr(ipaddr) {}

VirtualHost::VirtualHost(const std::string& ipaddr,
                         webconfig::ConfigServerBlock* config) :
    _ipaaddr(ipaddr), _name(config->server_name()), _config(config)
{
}

VirtualHost::VirtualHost(const VirtualHost& other) :
    _ipaaddr(other._ipaaddr), _name(other._name), _config(other._config)
{
}

VirtualHost& VirtualHost::operator=(const VirtualHost& other)
{
    if (this != &other) {
        _ipaaddr = other._ipaaddr;
        _name = other._name;
        _config = other._config;
    }
    return (*this);
}

VirtualHost::~VirtualHost() {}

const std::string& VirtualHost::ipaddr(void) const
{
    return (_ipaaddr);
}

const std::string& VirtualHost::name(void) const
{
    return (_name);
}

webconfig::ConfigServerBlock* VirtualHost::config(void) const
{
    return (_config);
}

} // namespace webkernel
