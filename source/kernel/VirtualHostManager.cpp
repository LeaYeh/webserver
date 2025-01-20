#include "VirtualHostManager.hpp"
#include "kernelUtils.hpp"

namespace webkernel
{

VirtualHostManager::VirtualHostManager() {}

VirtualHostManager::~VirtualHostManager()
{
    for (ListenMap::iterator it = _listen_hash.begin();
         it != _listen_hash.end();
         ++it) {
        delete it->second;
    }
}

void VirtualHostManager::add_server(const std::string& ipaddr,
                                    webconfig::ConfigServerBlock* server_config)
{
    if (_listen_hash.find(ipaddr) == _listen_hash.end()) {
        _listen_hash[ipaddr] = new VirtualHost(ipaddr, server_config);
    }
}

bool VirtualHostManager::has_server(const std::string& ipaddr) const
{
    return (_listen_hash.find(ipaddr) != _listen_hash.end());
}

webconfig::ConfigServerBlock*
VirtualHostManager::find_server(const std::string& ipaddr,
                                const std::string& host) const
{
    std::map<std::string, VirtualHost*>::const_iterator it;

    for (it = _listen_hash.begin(); it != _listen_hash.end(); it++) {
        if (host.empty() && it->first == ipaddr) {
            return (it->second->config());
        }
        if (it->first == ipaddr && it->second->name() == host) {
            return (it->second->config());
        }
    }
    return (NULL);
}

webconfig::ConfigServerBlock* VirtualHostManager::find_default(int fd) const
{
    std::string ipaddr = get_socket_address(fd);

    webconfig::ConfigServerBlock* server_config = this->find_server(ipaddr, "");
    return (server_config);
}

} // namespace webkernel
