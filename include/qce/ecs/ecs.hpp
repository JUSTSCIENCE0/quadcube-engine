// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ecs/component.hpp>
#include <qce/ancillary/error_codes.hpp>

#include <cu/template-utils.hpp>

#include <cassert>
#include <algorithm>
#include <bitset>
#include <unordered_map>
#include <list>
#include <optional>

#ifndef FRIEND_TEST
#define  FRIEND_TEST(test_case_name, test_name)
#endif

namespace QCE {
    using entity_id_t = CU::id_t;

    static constexpr size_t DISABLE_ENTITY_QUERY_CACHE = 0;
    static constexpr auto   INFINITE_ENTITY_QUERY_CACHE = std::numeric_limits<size_t>::max();

    template <size_t CacheLimit, typename... Components>
    class EntityQueriesCache {
    public:
        template <typename... Query>
        std::optional<std::set<entity_id_t>> Get() const noexcept {
            if constexpr (CacheLimit == DISABLE_ENTITY_QUERY_CACHE) {
                return std::nullopt;
            }

            auto mask = GetQueryMask<Query...>();
            auto it = m_results.find(mask);
            if (it == m_results.end()) {
                return std::nullopt;
            }

            if constexpr (CacheLimit != INFINITE_ENTITY_QUERY_CACHE) {
                auto usage_it = std::find(m_usage_order.begin(), m_usage_order.end(), mask);
                assert(m_usage_order.end() != usage_it);
                m_usage_order.splice(m_usage_order.begin(), m_usage_order, usage_it);
            }

            return it->second;
        }

        template <typename... Query>
        void Put(std::set<entity_id_t> result) noexcept {
            if constexpr (CacheLimit == DISABLE_ENTITY_QUERY_CACHE) {
                return;
            }
            auto mask = GetQueryMask<Query...>();

            assert(m_results.find(mask) == m_results.end());
            assert(m_usage_order.end() == std::find(
                m_usage_order.begin(), m_usage_order.end(), mask));

            if constexpr (CacheLimit != INFINITE_ENTITY_QUERY_CACHE) {
                assert(m_results.size() <= CacheLimit);

                if (m_results.size() == CacheLimit) {
                    auto lru_mask = m_usage_order.back();
                    m_usage_order.pop_back();
                    m_results.erase(lru_mask);
                }
                m_usage_order.push_front(mask);
            }

            m_results.insert(mask, std::move(result));
        }

        // TODO: Invalidate cache entries on component addition/removal



    private:
        FRIEND_TEST(EntityQueriesCacheTest, QueryMask);

        static constexpr size_t ComponentsCount = sizeof...(Components);

        using QueryMask = std::bitset<ComponentsCount>;

        template <typename... Query>
        static QueryMask GetQueryMask() noexcept {
            constexpr bool flags[ComponentsCount] = {
                CU::contains_v<Components, Query...>...
            };

            QueryMask mask{};
            for (size_t i = 0; i < ComponentsCount; ++i) {
                if (flags[i]) {
                    mask.set(i);
                }
            }
            return mask;
        }

        std::unordered_map<QueryMask, std::set<entity_id_t>> m_results{};
        std::list<QueryMask> m_usage_order{};
    };

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

                // TODO: cache result for next queries
                return result;
            }
        }

    private:
        CU::IdPool m_id_pool;
        std::tuple<ComponentStorage<Components>...> m_components;
    };
}
