// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/component.hpp>
#include <qce/ecs/query_cache.hpp>
#include <qce/ancillary/error_codes.hpp>

namespace QCE {
    template <size_t CacheLimit, typename... Components>
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

                if constexpr (CacheLimit != DISABLE_ENTITY_QUERY_CACHE) {
                    if (storage.HasEntity(entity_id)) {
                        m_query_cache.RemoveComponent<Components>();
                    }
                    else
                        return;
                }

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

            if constexpr (CacheLimit != DISABLE_ENTITY_QUERY_CACHE) {
                m_query_cache.RemoveComponent<Component>();
            }

            return ErrorCode::SUCCESS;
        }

        template <typename Component>
        ErrorCode RemoveComponent(entity_id_t entity_id) {
            if (!m_id_pool.CheckId(entity_id)) {
                return ErrorCode::E_ENG_ENTITY_NOT_FOUND;
            }

            auto& storage = std::get<ComponentStorage<Component>>(m_components);
            if (storage.HasEntity(entity_id)) {
                if constexpr (CacheLimit != DISABLE_ENTITY_QUERY_CACHE) {
                    m_query_cache.RemoveComponent<Component>();
                }
                storage.RemoveEntity(entity_id);
            }

            return ErrorCode::SUCCESS;
        }

        template <typename... Query>
        std::set<entity_id_t> QueryEntities() const {
            std::set<entity_id_t> result;
            if constexpr (sizeof...(Query) == 0) {
                return result;
            }
            else if constexpr (sizeof...(Query) == 1) {
                using first_type_t = std::tuple_element_t<0, std::tuple<Query...>>;
                const auto& storage = std::get<ComponentStorage<first_type_t>>(m_components);
                return storage.GetActualEntities();
            }
            else {
                if constexpr (CacheLimit != DISABLE_ENTITY_QUERY_CACHE) {
                    auto cached_result = m_query_cache.Get<Query...>();
                    if (cached_result.has_value()) {
                        return cached_result.value();
                    }
                }

                std::vector<size_t> entity_counts{
                    std::get<ComponentStorage<Query>>(m_components).Size()...
                };

                auto it = std::min_element(entity_counts.begin(), entity_counts.end());
                if (0 == *it) {
                    return result;
                }

                auto min_index = std::distance(entity_counts.begin(), it);
                ptrdiff_t index = 0;

                ([&]() {
                    if (index == min_index) {
                        const auto& storage = std::get<ComponentStorage<Query>>(m_components);
                        result = storage.GetActualEntities();
                    }
                    index++;
                    }(), ...);

                ([&]() {
                    if (index != min_index) {
                        const auto& storage = std::get<ComponentStorage<Query>>(m_components);
                        std::erase_if(result,
                            [&storage](entity_id_t id) { return !storage.HasEntity(id); });
                    }
                    index++;
                    }(), ...);

                if constexpr (CacheLimit != DISABLE_ENTITY_QUERY_CACHE) {
                    m_query_cache.Put<Query...>(result);
                }

                return result;
            }
        }

        template <typename Component>
        Component& GetComponent(entity_id_t entity_id) {
            auto& storage = std::get<ComponentStorage<Component>>(m_components);
            return storage[entity_id];
        }

        template <typename Component>
        const Component& GetComponent(entity_id_t entity_id) const {
            auto& storage = std::get<ComponentStorage<Component>>(m_components);
            return storage[entity_id];
        }

    private:
        CU::IdPool m_id_pool;
        std::tuple<ComponentStorage<Components>...> m_components;

        mutable EntityQueriesCache<CacheLimit, Components...> m_query_cache{};
    };
}