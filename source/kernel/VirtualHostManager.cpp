#include "VirtualHostManager.hpp"
#include "Logger.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"

namespace webkernel
{

VirtualHostManager::VirtualHostManager() {}

VirtualHostManager::~VirtualHostManager()
{
    for (ListenMap::iterator it = _vhost_hash.begin(); it != _vhost_hash.end();
         ++it) {
        delete it->second;
    }
}

void VirtualHostManager::add_server(const std::string& ipaddr,
                                    webconfig::ConfigServerBlock* server_config)
{
    std::string ipaddr_host = utils::join(ipaddr, server_config->server_name());

    if (_vhost_hash.find(ipaddr_host) == _vhost_hash.end()) {
        _vhost_hash[ipaddr_host] = new VirtualHost(ipaddr, server_config);
    }
}

void VirtualHostManager::add_listen(const std::string& ipaddr,
                                    webconfig::ConfigServerBlock* server_config)
{
    if (_vhost_hash.find(ipaddr) == _vhost_hash.end()) {
        _vhost_hash[ipaddr] = new VirtualHost(ipaddr, server_config);
    }
}

bool VirtualHostManager::has_server(
    const std::string& ipaddr,
    webconfig::ConfigServerBlock* server_config) const
{
    std::string ipaddr_host = utils::join(ipaddr, server_config->server_name());
    return (_vhost_hash.find(ipaddr_host) != _vhost_hash.end());
}

bool VirtualHostManager::is_ipaddr_listen(const std::string& ipaddr) const
{
    return (_vhost_hash.find(ipaddr) != _vhost_hash.end());
}

webconfig::ConfigServerBlock*
VirtualHostManager::find_server(const std::string& ipaddr,
                                const std::string& host) const
{
    std::map<std::string, VirtualHost*>::const_iterator it;
    const std::string ipaddr_host = utils::join(ipaddr, host);

    LOG(weblog::DEBUG, "Looking for server block: " + ipaddr_host);
    for (it = _vhost_hash.begin(); it != _vhost_hash.end(); it++) {
        LOG(weblog::DEBUG, "try to match ipaddr_host: " + it->first);
        if (it->first == ipaddr_host) {
            LOG(weblog::DEBUG, "found server block: " + it->first);
            return (it->second->config());
        }
    }
    return (NULL);
}

webconfig::ConfigServerBlock* VirtualHostManager::find_default(int fd) const
{
    std::string ipaddr = get_socket_address(fd);
    std::map<std::string, VirtualHost*>::const_iterator it;

    LOG(weblog::DEBUG,
        "Looking for default server block for ipaddr: " + ipaddr);
    for (it = _vhost_hash.begin(); it != _vhost_hash.end(); it++) {
        LOG(weblog::DEBUG, "try to match ipaddr: " + it->first);
        if (it->first == ipaddr) {
            return (it->second->config());
        }
    }
    return (NULL);
}

} // namespace webkernel
