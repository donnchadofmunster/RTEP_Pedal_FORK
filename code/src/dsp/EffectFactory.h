#pragma once

#include <string>
#include <functional>
#include <map>
#include <memory>
#include "Effect.h"

/**
 * @class EffectFactory
 * @brief Central factory for dynamically creating DSP effect instances by name.
 *
 * This singleton class maintains a registry of effect constructors. Each effect registers
 * itself via the `REGISTER_EFFECT_AUTO()` macro, which associates a string name with a factory function.
 * 
 * The factory enables runtime creation of effects based on configuration files, UI selections, etc.
 */
class EffectFactory {
public:
    /**
     * @brief Type alias for effect constructor functions.
     * These functions return a new instance of an effect via a shared pointer.
     */
    using Creator = std::function<std::shared_ptr<Effect>()>;

    /**
     * @brief Returns the singleton instance of the factory.
     * @return A reference to the global EffectFactory instance.
     */
    static EffectFactory& instance();

    /**
     * @brief Registers a new effect under a given name.
     * @param name The name used to identify the effect (usually the class name).
     * @param creator A factory function that creates and returns a shared_ptr to the effect.
     */
    void registerEffect(const std::string& name, Creator creator);

    /**
     * @brief Creates an effect instance by name.
     * @param name The string identifier of the effect (as registered).
     * @return A shared_ptr to the created Effect instance, or nullptr if not found.
     */
    std::shared_ptr<Effect> createEffect(const std::string& name) const;

private:
    std::map<std::string, Creator> registry; ///< Internal mapping of effect names to constructor functions
};
