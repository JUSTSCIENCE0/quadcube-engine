// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#ifndef QCE_UID_TYPE
#  define QCE_UID_TYPE uint64_t
#endif // !QCE_UID_TYPE

#include <qce/error_codes.hpp>

#include <stdint.h>
#include <concepts>
#include <mutex>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>

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
    class UidGeneratorT :
        private PrivateImplementation::UidGenSingletonTemplate {
    public:
        using UID = T;

        static UID Get() {
            static UidGeneratorT gen;

            std::scoped_lock lock(gen.m_counter_mutex);
            if (gen.m_counter == std::numeric_limits<UID>::max())
                throw ErrorCodeException(ErrorCode::E_ENG_TOO_MANY_UNIQUE_OBJECTS);
            return gen.m_counter++;
        }

    private:
        UidGeneratorT() :
            UidGenSingletonTemplate() {}

        std::mutex m_counter_mutex;
        UID m_counter = 0;
    };

    using UidGenerator = UidGeneratorT<QCE_UID_TYPE>;
    using uid_t = UidGenerator::UID;
    constexpr inline auto UID_STR_WIDTH = sizeof(uid_t) * 2;

    static inline uid_t get_uid() {
        return UidGenerator::Get();
    }

    static inline std::string get_unique_name(const std::string& name) {
        std::stringstream ss;
        ss << name << " {" 
           << std::hex
           << std::setw(UID_STR_WIDTH)
           << std::setfill('0')
           << get_uid()
           << "}";
        return ss.str();
    }

    static inline std::string extract_name(std::string unique_name) {
        assert(unique_name.length() > UID_STR_WIDTH + 3); /* {UID_STR_WIDTH}*/
        assert(unique_name[unique_name.length() - 1] == '}');

        auto postfix_offset = unique_name.rfind(" {");
        assert(postfix_offset != std::string::npos);
        return unique_name.erase(postfix_offset);
    }
}
