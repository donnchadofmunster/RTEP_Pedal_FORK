#pragma once
#include "EffectFactory.h"

#define REGISTER_EFFECT(CLASS_NAME) \
    namespace { \
        const bool registered_##CLASS_NAME = []() { \
            EffectFactory::instance().registerEffect(#CLASS_NAME, []() { \
                return std::make_shared<CLASS_NAME>(); \
            }); \
            return true; \
        }(); \
    }
