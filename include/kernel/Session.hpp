#pragma once
#include <ctime>
#include <map>
#include <string>

namespace webkernel
{
class Session
{
public:
    Session(const std::string& id, int expire_seconds);
    Session(const Session& other);
    Session& operator=(const Session& other);
    ~Session();

private:
    std::string _id;
    int _expire_seconds;
    time_t _last_access_time;
    std::map<std::string, std::string> _data;

private:
    Session();
};

} // namespace webkernel
