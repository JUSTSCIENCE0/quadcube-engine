// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/ecs.hpp>

#include <qce/ancillary/error_codes.hpp>

#include <concepts>

namespace QCE {
    template<typename T>
    concept SystemT =
        requires(T s) {
            { s.Update() } -> std::same_as<ErrorCode>;
        };

    template<SystemT... Systems>
    class SystemsHub {
    public:
        explicit SystemsHub(Entities& entities) :
            m_systems( SystemStorage<Systems>{ System{entities}, true }... )
        {}

        SystemsHub(const SystemsHub&) = delete;
        SystemsHub(SystemsHub&&) = delete;
        SystemsHub& operator=(const SystemsHub&) = delete;
        SystemsHub& operator=(SystemsHub&&) = delete;

        template<SystemT System>
        System& Get() {
            return std::get<SystemStorage<System>>(m_systems).system;
        }

        template<SystemT System>
        void SetActive(bool is_active) {
            std::get<SystemStorage<System>>(m_systems).is_active = is_active;
        }

        template<SystemT System>
        bool IsActive() const {
            return std::get<SystemStorage<System>>(m_systems).is_active;
        }

        ErrorCode UpdateAll() {
            auto ret_code = ErrorCode::SUCCESS;
            ([&]() {
                if (is_fail(ret_code)) {
                    return;
                }
                auto& storage = std::get<SystemStorage<Systems>>(m_systems);
                if (storage.is_active) {
                    ret_code = storage.system.Update();
                }
            }(), ...);
            return ret_code;
        }

    private:
        template<SystemT System>
        struct SystemStorage {
            System system;
            bool is_active = true;
        };

        std::tuple<SystemStorage<Systems>...> m_systems;
    };
}