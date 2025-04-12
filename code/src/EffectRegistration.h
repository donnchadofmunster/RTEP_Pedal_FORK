#pragma once
#include "EffectFactory.h"
#include <memory>

/**
 * @brief Automatically registers an Effect subclass with the EffectFactory at static initialization time.
 *
 * This macro should be placed in the implementation file (.cpp) of an `Effect` subclass.
 * It ensures:
 *   - The effect is registered with the factory under its class name as a string.
 *   - A unique static initializer runs at program startup.
 *   - A visible linker symbol (`ForceLink_<CLASS_NAME>_Effect`) exists, allowing
 *     `ForceEffects()` to reference and include all available effects in the binary.
 *
 * Example usage:
 * @code
 * class Gain : public Effect {
 * public:
 *     float process(float sample, float setting) override;
 * };
 *
 * REGISTER_EFFECT_AUTO(Gain);
 * @endcode
 *
 * This makes `EffectFactory::createEffect("Gain")` valid at runtime.
 *
 * @param CLASS_NAME The name of the effect class to register.
 */
#define REGISTER_EFFECT_AUTO(CLASS_NAME)                                      \
    namespace {                                                               \
        static void ensure_##CLASS_NAME##_registered() {                      \
            static bool registered = [] {                                     \
                EffectFactory::instance().registerEffect(#CLASS_NAME, [] {    \
                    return std::make_shared<CLASS_NAME>();                    \
                });                                                           \
                return true;                                                  \
            }();                                                              \
            (void)registered;                                                 \
        }                                                                     \
        struct CLASS_NAME##_EffectRegistrar {                                 \
            CLASS_NAME##_EffectRegistrar() { ensure_##CLASS_NAME##_registered(); } \
        };                                                                    \
        static CLASS_NAME##_EffectRegistrar CLASS_NAME##_registrar_instance; \
                                                                              \
        /**                                                                  \
         * @brief Linkable symbol to ensure this effect is included in the binary. \
         * Call ForceLink_<CLASS_NAME>_Effect() in ForceEffects.cpp to force inclusion. \
         */                                                                   \
        extern "C" void ForceLink_##CLASS_NAME##_Effect() {}                  \
    }
