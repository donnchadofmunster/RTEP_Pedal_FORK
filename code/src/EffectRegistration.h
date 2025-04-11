#pragma once
#include "EffectFactory.h"
#include <memory>

/**
 * Registers an effect at static init time and exposes a unique linker symbol
 * so it can be included automatically by ForceEffects().
 */
#define REGISTER_EFFECT_AUTO(CLASS_NAME) \
    namespace { \
        static void ensure_##CLASS_NAME##_registered() { \
            static bool registered = [] { \
                EffectFactory::instance().registerEffect(#CLASS_NAME, [] { \
                    return std::make_shared<CLASS_NAME>(); \
                }); \
                return true; \
            }(); \
            (void)registered; \
        } \
        struct CLASS_NAME##_EffectRegistrar { \
            CLASS_NAME##_EffectRegistrar() { ensure_##CLASS_NAME##_registered(); } \
        }; \
        static CLASS_NAME##_EffectRegistrar CLASS_NAME##_registrar_instance; \
        extern "C" void ForceLink_##CLASS_NAME##_Effect() {} \
    }
