#include "Timer.hpp"
#include <ctime>

namespace utils
{

Timer::Timer() :
    _running(false), _start_time(0), _end_time(0), _elapsed(0), _timeout(60.0)
{
}

Timer::Timer(size_t timeout) :
    _running(false),
    _start_time(0),
    _end_time(0),
    _elapsed(0),
    _timeout(timeout)
{
}

Timer::Timer(const Timer& other) :
    _running(other._running),
    _start_time(other._start_time),
    _end_time(other._end_time),
    _elapsed(other._elapsed),
    _timeout(other._timeout)
{
}

Timer& Timer::operator=(const Timer& other)
{
    if (this != &other) {
        _running = other._running;
        _start_time = other._start_time;
        _end_time = other._end_time;
        _elapsed = other._elapsed;
        _timeout = other._timeout;
    }
    return (*this);
}

Timer::~Timer() {}

void Timer::start()
{
    if (!_running) {
        _start_time = std::time(0);
        _running = true;
    }
}

void Timer::stop()
{
    if (_running) {
        _end_time = std::time(0);
        _running = false;
        _elapsed = _end_time - _start_time;
    }
}

void Timer::reset()
{
    _running = false;
    _start_time = 0;
    _end_time = 0;
    _elapsed = 0;
}

double Timer::elapsed()
{
    _end_time = std::time(0);
    _elapsed = _end_time - _start_time;
    return (_elapsed);
}

bool Timer::timeout()
{
    return (this->elapsed() >= _timeout);
}

} // namespace utils
