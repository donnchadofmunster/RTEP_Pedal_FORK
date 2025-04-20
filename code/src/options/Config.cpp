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
}

Config &Config::getInstance()
{
    static Config singleton;
    return singleton;
    instance.store(&singleton);
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

bool Config::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "[Config] Failed to open file: " << filename << "\n";
        return false;
    }

    std::cerr << "[Config] Loading initial config from: " << filename << "\n";

    std::string line;
    while (std::getline(file, line))
    {
        // Remove comments and leading/trailing whitespace
        auto comment = line.find('#');
        if (comment != std::string::npos)
            line = line.substr(0, comment);

        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string key, onStr, value;

        if (!std::getline(ss, key, ','))
            continue;
        if (!std::getline(ss, onStr, ','))
            continue;
        if (!std::getline(ss, value))
            continue;

        // Trim fields
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        onStr.erase(0, onStr.find_first_not_of(" \t"));
        onStr.erase(onStr.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Parse activation flag
        bool enabled = (onStr == "true" || onStr == "1");

        // Infer value type
        std::any typedValue;
        try
        {
            if (value.find(' ') != std::string::npos)
            {
                typedValue = value; // likely a list
            }
            else if (value == "true" || value == "false")
            {
                typedValue = (value == "true");
            }
            else if (value.find('.') != std::string::npos)
            {
                typedValue = std::stof(value);
            }
            else
            {
                typedValue = std::stoi(value);
            }
        }
        catch (...)
        {
            typedValue = value; // fallback to raw string
        }
        std::cout << "[Config] Setting " << key << ": " << enabled << "\n";
        set(key, enabled, typedValue);
    }

    return true;
}

// Explicit template instantiations
template int Config::get<int>(const std::string &, const int &) const;
template float Config::get<float>(const std::string &, const float &) const;
template bool Config::get<bool>(const std::string &, const bool &) const;
template std::string Config::get<std::string>(const std::string &, const std::string &) const;
