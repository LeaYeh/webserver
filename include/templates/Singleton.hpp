#pragma once
#include <stdexcept>

namespace templates
{

template <typename TType, typename TParam = void> class Singleton
{
public:
    static TType* instance()
    {
        if (!_instance) {
            throw std::runtime_error(
                "Instance not yet created. Call instantiate() first.");
        }
        return _instance;
    }

    static void instantiate()
    {
        if (!_instance) {
            _instance = TType::create_instance();
        }
    }

    static void instantiate(const TParam& param)
    {
        if (!_instance) {
            _instance = TType::create_instance(param);
        }
    }

    static void destroy()
    {
        if (_instance) {
            delete _instance;
            _instance = NULL;
        }
    }

protected:
    Singleton(){};
    ~Singleton(){};
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);

private:
    static TType* _instance;
};

template <typename TType> class Singleton<TType, void>
{
public:
    static TType* instance()
    {
        if (!_instance) {
            throw std::runtime_error(
                "Instance not yet created. Call instantiate() first.");
        }
        return _instance;
    }

    static void instantiate()
    {
        if (!_instance) {
            _instance = TType::create_instance();
        }
    }

    static void destroy()
    {
        if (_instance) {
            delete _instance;
            _instance = NULL;
        }
    }

protected:
    Singleton(){};
    ~Singleton(){};
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);

private:
    static TType* _instance;
};

template <typename TType, typename TParam>
TType* Singleton<TType, TParam>::_instance = NULL;
template <typename TType> TType* Singleton<TType, void>::_instance = NULL;

} // namespace templates
