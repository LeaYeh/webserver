#pragma once
#include <cstddef>

namespace utils
{

class Timer
{
public:
    void start();
    void stop();
    void reset();
    double elapsed();
    bool timeout();

public:
    Timer();
    Timer(size_t timeout);
    Timer& operator=(const Timer& other);
    Timer(const Timer& other);
    ~Timer();

private:
    bool _running;
    double _start_time;
    double _end_time;
    double _elapsed;
    size_t _timeout;

private:
};

} // namespace utils
