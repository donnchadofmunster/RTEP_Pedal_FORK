#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <atomic>
#include <csignal>
#include <any>

class Config
{
public:
    // Get singleton instance
    static Config &getInstance();

    // Sets a configuration value (marks updated)
    void set(const std::string &key, bool on, const std::any &value);

    // Gets a configuration value with default fallback
    template <typename T>
    T get(const std::string &key, const T &defaultValue) const;

    // Returns true if key exists
    bool contains(const std::string &key) const;

    // Whether configuration has been updated since last check
    bool hasUpdate() const;

    // Clears the update flag
    void clearUpdate();

    // Register signal handler
    static void registerSignalHandler();

private:
    Config(); // Private constructor
    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;

    static void signalHandler(int signal);

    static std::atomic<Config *> instance;

    struct Entry
    {
        bool enabled = false;
        std::any value;
    };

    mutable std::shared_mutex mutex;
    std::unordered_map<std::string, Entry> data;
    std::atomic<bool> updated;
};

#endif // CONFIG_H
