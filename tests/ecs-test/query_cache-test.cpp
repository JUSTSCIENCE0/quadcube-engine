// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <gtest/gtest.h>

#include <qce/ecs/query_cache.hpp>

namespace QCE {
    struct Component1 { int val = 0; };
    struct Component2 { int val = 0; };
    struct Component3 { int val = 0; };
    struct Component4 { int val = 0; };
    struct Component5 { int val = 0; };

    /* entity  | 1 | 2 | 3 |
    -----------|---|---|---|
    Component1 | 1 | 2 | 3 |
    Component2 |   | 4 | 5 |
    Component3 |   | 6 |   |
    Component4 | 7 | 8 |   |
    Component5 |   | 9 |   | */

    class EntityQueriesCacheTest :
        public testing::Test {
    protected:
        using EntityQueriesCacheT =
            EntityQueriesCache<
            3,
            Component1,
            Component2,
            Component3,
            Component4,
            Component5
            >;

        EntityQueriesCacheT m_test_cache{};
    };

    TEST_F(EntityQueriesCacheTest, QueryMask) {
        auto mask = EntityQueriesCacheT::GetQueryMask<Component1, Component2, Component5>();
        std::bitset<5> expected_mask(0b10011ull);
        EXPECT_EQ(mask, expected_mask);

        mask = EntityQueriesCacheT::GetQueryMask<Component4, Component1>();
        expected_mask = std::bitset<5>(0b01001ull);
        EXPECT_EQ(mask, expected_mask);

        mask = EntityQueriesCacheT::GetQueryMask<Component1, Component3, Component5, Component2, Component4>();
        expected_mask = std::bitset<5>(0b11111ull);
        EXPECT_EQ(mask, expected_mask);

        mask = EntityQueriesCacheT::GetQueryMask<>();
        expected_mask = std::bitset<5>(0b00000ull);
        EXPECT_EQ(mask, expected_mask);
    }

    TEST_F(EntityQueriesCacheTest, ComponentIndex) {
        constexpr auto index1 = EntityQueriesCacheT::GetComponentIndex<Component1>();
        EXPECT_EQ(index1, 0u);
        constexpr auto index2 = EntityQueriesCacheT::GetComponentIndex<Component2>();
        EXPECT_EQ(index2, 1u);
        constexpr auto index3 = EntityQueriesCacheT::GetComponentIndex<Component3>();
        EXPECT_EQ(index3, 2u);
        constexpr auto index4 = EntityQueriesCacheT::GetComponentIndex<Component4>();
        EXPECT_EQ(index4, 3u);
        constexpr auto index5 = EntityQueriesCacheT::GetComponentIndex<Component5>();
        EXPECT_EQ(index5, 4u);
    }

    TEST_F(EntityQueriesCacheTest, BaseFunctional) {
        m_test_cache.Put<Component1, Component4>({ 1, 2 });
        m_test_cache.Put<Component2, Component3>({ 2 });
        m_test_cache.Put<Component1, Component2>({ 2, 3 });

        auto result = m_test_cache.Get<Component1, Component4>();
        std::set<entity_id_t> expected{ 1, 2 };
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), expected);

        result = m_test_cache.Get<Component2, Component3>();
        expected = { 2 };
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), expected);

        result = m_test_cache.Get<Component1, Component2>();
        expected = { 2, 3 };
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), expected);

        result = m_test_cache.Get<Component3, Component5>();
        ASSERT_FALSE(result.has_value());

        m_test_cache.RemoveComponent<Component2>();

        result = m_test_cache.Get<Component1, Component4>();
        expected = { 1, 2 };
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), expected);

        result = m_test_cache.Get<Component2, Component3>();
        ASSERT_FALSE(result.has_value());

        result = m_test_cache.Get<Component1, Component2>();
        ASSERT_FALSE(result.has_value());

        result = m_test_cache.Get<Component3, Component5>();
        ASSERT_FALSE(result.has_value());
    }

    TEST_F(EntityQueriesCacheTest, LimitOverflow) {
        m_test_cache.Put<Component1, Component4>({ 1, 2 });
        m_test_cache.Put<Component2, Component3>({ 2 });
        m_test_cache.Put<Component1, Component2>({ 2, 3 });
        m_test_cache.Put<Component4, Component5>({ 2 });

        auto exists = m_test_cache.Get<Component1, Component4>().has_value();
        ASSERT_FALSE(exists);
        exists = m_test_cache.Get<Component4, Component5>().has_value();
        ASSERT_TRUE(exists);
        exists = m_test_cache.Get<Component1, Component2>().has_value();
        ASSERT_TRUE(exists);
        exists = m_test_cache.Get<Component2, Component3>().has_value();
        ASSERT_TRUE(exists);

        // oldest now is Component4, Component5

        m_test_cache.Put<Component1, Component4>({ 1, 2 });

        exists = m_test_cache.Get<Component1, Component4>().has_value();
        ASSERT_TRUE(exists);
        exists = m_test_cache.Get<Component4, Component5>().has_value();
        ASSERT_FALSE(exists);
        exists = m_test_cache.Get<Component1, Component2>().has_value();
        ASSERT_TRUE(exists);
        exists = m_test_cache.Get<Component2, Component3>().has_value();
        ASSERT_TRUE(exists);
    }

}