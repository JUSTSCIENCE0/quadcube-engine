// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <gtest/gtest.h>

#include <qce/ecs/ecs.hpp>

namespace QCE {
    struct Component1 { int val = 0; };
    struct Component2 { int val = 0; };
    struct Component3 { int val = 0; };
    struct Component4 { int val = 0; };
    struct Component5 { int val = 0; };

    class EntityQueriesCacheTest :
            public testing::Test {
    protected:
        EntityQueriesCache<
            64,
            Component1,
            Component2,
            Component3,
            Component4,
            Component5> m_test_cache{};
    };

    TEST_F(EntityQueriesCacheTest, QueryMask) {
        auto mask = m_test_cache.GetQueryMask<Component1, Component2, Component5>();
        std::bitset<5> expected_mask(0b10011ull);
        EXPECT_EQ(mask, expected_mask);

        mask = m_test_cache.GetQueryMask<Component4, Component1>();
        expected_mask = std::bitset<5>(0b01001ull);
        EXPECT_EQ(mask, expected_mask);

        mask = m_test_cache.GetQueryMask<Component1, Component3, Component5, Component2, Component4>();
        expected_mask = std::bitset<5>(0b11111ull);
        EXPECT_EQ(mask, expected_mask);

        mask = m_test_cache.GetQueryMask<>();
        expected_mask = std::bitset<5>(0b00000ull);
        EXPECT_EQ(mask, expected_mask);
    }

    // TODO: check query cache working
}

// TODO: refactore tests - use fixtures

TEST(EntityManagerTest, BaseFunctional) {
    using Component1 = uint32_t;
    using Component2 = float;
    using Component3 = int64_t;

    QCE::EntityManager<Component1, Component2, Component3> entity_manager;

    auto entity1 = entity_manager.AddEntity();
    auto entity2 = entity_manager.AddEntity();
    auto entity3 = entity_manager.AddEntity();

    ASSERT_EQ(entity1, 0);
    ASSERT_EQ(entity2, 1);
    ASSERT_EQ(entity3, 2);

    // check invalid entity
    ASSERT_EQ(QCE::ErrorCode::E_ENG_ENTITY_NOT_FOUND,
              entity_manager.AddComponent<Component1>(100, 100));

    /*         | entity1 | entity2 | entity3
    -----------|---------|---------|---------
    Component1 |   42    |  None   |  None
    Component2 |  36.6   |  None   |  3.14
    Component3 |  None   |   50    |   99  */

    ASSERT_EQ(QCE::ErrorCode::SUCCESS, entity_manager.AddComponent<Component1>(entity1, 42));
    ASSERT_EQ(QCE::ErrorCode::SUCCESS, entity_manager.AddComponent<Component2>(entity1, 36.6f));

    ASSERT_EQ(QCE::ErrorCode::SUCCESS, entity_manager.AddComponent<Component3>(entity2, 50));

    ASSERT_EQ(QCE::ErrorCode::SUCCESS, entity_manager.AddComponent<Component2>(entity3, 3.14f));
    ASSERT_EQ(QCE::ErrorCode::SUCCESS, entity_manager.AddComponent<Component3>(entity3, 99));

    // check queries
    EXPECT_EQ(entity_manager.QueryEntities<Component1>(),
              std::set<QCE::entity_id_t>({ entity1 }));
    EXPECT_EQ(entity_manager.QueryEntities<Component2>(),
              std::set<QCE::entity_id_t>({ entity1, entity3 }));
    EXPECT_EQ(entity_manager.QueryEntities<Component3>(),
              std::set<QCE::entity_id_t>({ entity2, entity3 }));
    EXPECT_EQ((entity_manager.QueryEntities<Component1,Component2>()),
              std::set<QCE::entity_id_t>({ entity1 }));
    EXPECT_EQ((entity_manager.QueryEntities<Component1, Component3>()),
        std::set<QCE::entity_id_t>({}));
    EXPECT_EQ((entity_manager.QueryEntities<Component2, Component3>()),
        std::set<QCE::entity_id_t>({ entity3 }));

    // remove entity
    ASSERT_EQ(QCE::ErrorCode::E_ENG_ENTITY_NOT_FOUND, entity_manager.RemoveEntity(100));
    ASSERT_EQ(QCE::ErrorCode::SUCCESS, entity_manager.RemoveEntity(entity2));

    // check removed entity
    ASSERT_EQ(QCE::ErrorCode::E_ENG_ENTITY_NOT_FOUND,
              entity_manager.AddComponent<Component1>(entity2, 100));

    // check queries
    EXPECT_EQ(entity_manager.QueryEntities<Component1>(),
              std::set<QCE::entity_id_t>({ entity1 }));
    EXPECT_EQ(entity_manager.QueryEntities<Component2>(),
              std::set<QCE::entity_id_t>({ entity1, entity3 }));
    EXPECT_EQ(entity_manager.QueryEntities<Component3>(),
              std::set<QCE::entity_id_t>({ entity3 }));
    EXPECT_EQ((entity_manager.QueryEntities<Component1, Component2>()),
        std::set<QCE::entity_id_t>({ entity1 }));
    EXPECT_EQ((entity_manager.QueryEntities<Component1, Component3>()),
        std::set<QCE::entity_id_t>({}));
    EXPECT_EQ((entity_manager.QueryEntities<Component2, Component3>()),
        std::set<QCE::entity_id_t>({ entity3 }));
}
