// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/error_codes.hpp>

#include <stdint.h>
#include <concepts>
#include <mutex>
#include <limits>

namespace QCE {
    namespace PrivateImplementation {
        struct UidGenSingletonTemplate {
            UidGenSingletonTemplate(const UidGenSingletonTemplate&) = delete;
            UidGenSingletonTemplate(UidGenSingletonTemplate&&) = delete;
            UidGenSingletonTemplate& operator=(const UidGenSingletonTemplate&) = delete;
            UidGenSingletonTemplate& operator=(UidGenSingletonTemplate&&) = delete;

        protected:
            UidGenSingletonTemplate() {
                if (instantiated) {
                    throw ErrorCodeException(ErrorCode::E_ENG_UIDGEN_MULTIPLE_INIT);
                }
                instantiated = true;
            }

        private:
            inline static bool instantiated = false;
        };
    }

    template<std::unsigned_integral T>
    class UidGenerator :
        private PrivateImplementation::UidGenSingletonTemplate {
    public:
        using UID = T;

        static UID Get() {
            static UidGenerator gen;

            std::scoped_lock lock(gen.m_counter_mutex);
            if (gen.m_counter == std::numeric_limits<UID>::max())
                throw ErrorCodeException(ErrorCode::E_ENG_TOO_MANY_UNIQUE_OBJECTS);
            return gen.m_counter++;
        }

    private:
        UidGenerator() :
            UidGenSingletonTemplate() {}

        std::mutex m_counter_mutex;
        UID m_counter = 0;
    };
}
