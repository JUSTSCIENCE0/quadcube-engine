// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/component.hpp>
#include <qce/ancillary/error_codes.hpp>

namespace QCE {
    using entity_id_t = CU::id_t;

    template <typename... Components>
    class EntityManager {
    public:
        entity_id_t AddEntity() {
            return m_id_pool.LockId();
        }

        ErrorCode RemoveEntity(entity_id_t entity_id) {
            if (!m_id_pool.CheckId(entity_id)) {
                return ErrorCode::E_ENG_ENTITY_NOT_FOUND;
            }

            m_id_pool.FreeId(entity_id);
            ([&]() {
                auto& storage = std::get<ComponentStorage<Components>>(m_components);
                storage.RemoveEntity(entity_id);
            }(), ...);
            return ErrorCode::SUCCESS;
        }

        template <typename Component>
        ErrorCode AddComponent(entity_id_t entity_id, Component component) {
            if (!m_id_pool.CheckId(entity_id)) {
                return ErrorCode::E_ENG_ENTITY_NOT_FOUND;
            }
            auto& storage = std::get<ComponentStorage<Component>>(m_components);
            storage.SetEntity(entity_id, std::move(component));
            return ErrorCode::SUCCESS;
        }

        template <typename... Request>
        std::set<entity_id_t> QueryEntities() const {
            std::set<entity_id_t> result;
            if constexpr (sizeof...(Request) == 0) {
                return result;
            }
            else if constexpr (sizeof...(Request) == 1) {
                using first_type_t = std::tuple_element_t<0, std::tuple<Request...>>;
                const auto& storage = std::get<ComponentStorage<first_type_t>>(m_components);
                return storage.GetActualEntities();
            }
            else {
                std::vector<size_t> entity_counts{
                    std::get<ComponentStorage<Request>>(m_components).Size()...
                };

                auto it = std::min_element(entity_counts.begin(), entity_counts.end());
                if (0 == *it) {
                    return result;
                }

                auto min_index = std::distance(entity_counts.begin(), it);

                // TODO: get result from the minimal storage and check other storages for each entity
                (void)min_index;

                return result;
            }
        }

    private:
        CU::IdPool m_id_pool;
        std::tuple<ComponentStorage<Components>...> m_components;
    };
}
