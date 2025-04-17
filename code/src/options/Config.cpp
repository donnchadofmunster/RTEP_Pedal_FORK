#include "Config.h"
#include <sstream>
#include <iostream>
#include <csignal>
#include <typeindex>
#include <any>
#include <mutex>

std::atomic<Config *> Config::instance{nullptr};

Config::Config()
{
    updated.store(false);
    instance.store(this);
}

Config &Config::getInstance()
{
    static Config singleton;
    return singleton;
}

void Config::signalHandler(int sig)
{
    if (sig == SIGUSR1)
    {
        auto config = instance.load();
        if (config)
        {
            std::cerr << "[Config] Caught SIGUSR1\n";
            config->updated.store(true);
        }
    }
}

void Config::registerSignalHandler()
{
    std::signal(SIGUSR1, Config::signalHandler);
}

void Config::set(const std::string &key, bool on, const std::any &value)
{
    std::unique_lock lock(mutex);
    if (on)
    {
        data[key] = Entry{true, value};
    }
    else
    {
        data.erase(key);
    }
    updated.store(true);
}

bool Config::contains(const std::string &key) const
{
    std::shared_lock lock(mutex);
    auto it = data.find(key);
    return it != data.end() && it->second.enabled;
}

bool Config::hasUpdate() const
{
    return updated.load();
}

void Config::clearUpdate()
{
    updated.store(false);
}

template <typename T>
T Config::get(const std::string &key, const T &defaultValue) const
{
    std::shared_lock lock(mutex);
    auto it = data.find(key);
    if (it == data.end() || !it->second.enabled)
        return defaultValue;
    try
    {
        return std::any_cast<T>(it->second.value);
    }
    catch (...)
    {
        return defaultValue;
    }
}

// Explicit template instantiations
template int Config::get<int>(const std::string &, const int &) const;
template float Config::get<float>(const std::string &, const float &) const;
template bool Config::get<bool>(const std::string &, const bool &) const;
template std::string Config::get<std::string>(const std::string &, const std::string &) const;
