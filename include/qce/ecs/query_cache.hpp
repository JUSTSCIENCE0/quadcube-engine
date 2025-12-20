// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <cu/id-utils.hpp>
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

            m_results.emplace(mask, std::move(result));
        }

        template <typename Component>
        void RemoveComponent() {
            constexpr auto index = GetComponentIndex<Component>();

            std::erase_if(m_results, [&](const auto& v) { return v.first.test(index); });
            std::erase_if(m_usage_order, [&](const auto& v) { return v.test(index); });
        }

    private:
        FRIEND_TEST(EntityQueriesCacheTest, QueryMask);
        FRIEND_TEST(EntityQueriesCacheTest, ComponentIndex);

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

        template <typename Component>
        static constexpr size_t GetComponentIndex() noexcept {
            return CU::type_index_v<Component, Components...>;
        }

        std::unordered_map<QueryMask, std::set<entity_id_t>> m_results{};
        mutable std::list<QueryMask> m_usage_order{};
    };
}