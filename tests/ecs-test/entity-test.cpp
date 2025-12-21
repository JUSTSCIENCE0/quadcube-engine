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

    class EntityManagerTest :
        public testing::Test {
    protected:
        using EntityManagerT =
            EntityManager<
            64,
            Component1,
            Component2,
            Component3
            >;
        EntityManagerT m_entity_manager{};

        entity_id_t m_entity1 = CU::INVALID_ID;
        entity_id_t m_entity2 = CU::INVALID_ID;
        entity_id_t m_entity3 = CU::INVALID_ID;

        void SetUp() override {
            m_entity1 = m_entity_manager.AddEntity();
            m_entity2 = m_entity_manager.AddEntity();
            m_entity3 = m_entity_manager.AddEntity();

            ASSERT_EQ(m_entity1, 0);
            ASSERT_EQ(m_entity2, 1);
            ASSERT_EQ(m_entity3, 2);

            // check invalid entity
            ASSERT_EQ(QCE::ErrorCode::E_ENG_ENTITY_NOT_FOUND,
                m_entity_manager.AddComponent<Component1>(100, { 100 }));
            ASSERT_EQ(QCE::ErrorCode::E_ENG_ENTITY_NOT_FOUND,
                m_entity_manager.RemoveEntity(100));

            /*         | entity1 | entity2 | entity3
            -----------|---------|---------|---------
            Component1 |   42    |  None   |  None
            Component2 |   36    |  None   |   14
            Component3 |  None   |   50    |   99  */

            ASSERT_EQ(QCE::ErrorCode::SUCCESS,
                m_entity_manager.AddComponent<Component1>(m_entity1, { 42 }));
            ASSERT_EQ(QCE::ErrorCode::SUCCESS,
                m_entity_manager.AddComponent<Component2>(m_entity1, { 36 }));

            ASSERT_EQ(QCE::ErrorCode::SUCCESS,
                m_entity_manager.AddComponent<Component3>(m_entity2, { 50 }));

            ASSERT_EQ(QCE::ErrorCode::SUCCESS,
                m_entity_manager.AddComponent<Component2>(m_entity3, { 14 }));
            ASSERT_EQ(QCE::ErrorCode::SUCCESS,
                m_entity_manager.AddComponent<Component3>(m_entity3, { 99 }));
        }
    };

    TEST_F(EntityManagerTest, CheckQueries) {
        EXPECT_EQ(m_entity_manager.QueryEntities<Component1>(),
                    std::set<QCE::entity_id_t>({ m_entity1 }));
        EXPECT_EQ(m_entity_manager.QueryEntities<Component2>(),
                    std::set<QCE::entity_id_t>({ m_entity1, m_entity3 }));
        EXPECT_EQ(m_entity_manager.QueryEntities<Component3>(),
                    std::set<QCE::entity_id_t>({ m_entity2, m_entity3 }));
        EXPECT_EQ((m_entity_manager.QueryEntities<Component1,Component2>()),
                    std::set<QCE::entity_id_t>({ m_entity1 }));
        EXPECT_EQ((m_entity_manager.QueryEntities<Component1, Component3>()),
            std::set<QCE::entity_id_t>({}));
        EXPECT_EQ((m_entity_manager.QueryEntities<Component2, Component3>()),
            std::set<QCE::entity_id_t>({ m_entity3 }));

        // remove entity
        ASSERT_EQ(QCE::ErrorCode::SUCCESS,
            m_entity_manager.RemoveEntity(m_entity1));

        // check removed entity
        ASSERT_EQ(QCE::ErrorCode::E_ENG_ENTITY_NOT_FOUND,
            m_entity_manager.AddComponent<Component1>(m_entity1, { 100 }));

        // check queries
        EXPECT_EQ(m_entity_manager.QueryEntities<Component1>(),
                    std::set<QCE::entity_id_t>({}));
        EXPECT_EQ(m_entity_manager.QueryEntities<Component2>(),
                    std::set<QCE::entity_id_t>({ m_entity3 }));
        EXPECT_EQ(m_entity_manager.QueryEntities<Component3>(),
                    std::set<QCE::entity_id_t>({ m_entity2, m_entity3 }));
        EXPECT_EQ((m_entity_manager.QueryEntities<Component1, Component2>()),
            std::set<QCE::entity_id_t>({}));
        EXPECT_EQ((m_entity_manager.QueryEntities<Component1, Component3>()),
            std::set<QCE::entity_id_t>({}));
        EXPECT_EQ((m_entity_manager.QueryEntities<Component2, Component3>()),
            std::set<QCE::entity_id_t>({ m_entity3 }));

        // remove entity component
        ASSERT_EQ(QCE::ErrorCode::SUCCESS,
            m_entity_manager.RemoveComponent<Component2>(m_entity3));

        // check queries
        EXPECT_EQ(m_entity_manager.QueryEntities<Component1>(),
            std::set<QCE::entity_id_t>({}));
        EXPECT_EQ(m_entity_manager.QueryEntities<Component2>(),
            std::set<QCE::entity_id_t>({}));
        EXPECT_EQ(m_entity_manager.QueryEntities<Component3>(),
            std::set<QCE::entity_id_t>({ m_entity2, m_entity3 }));
        EXPECT_EQ((m_entity_manager.QueryEntities<Component1, Component2>()),
            std::set<QCE::entity_id_t>({}));
        EXPECT_EQ((m_entity_manager.QueryEntities<Component1, Component3>()),
            std::set<QCE::entity_id_t>({}));
        EXPECT_EQ((m_entity_manager.QueryEntities<Component2, Component3>()),
            std::set<QCE::entity_id_t>({}));
    }

    TEST_F(EntityManagerTest, ReadValues) {
        ASSERT_EQ(42, m_entity_manager.GetComponent<Component1>(m_entity1).val);
        ASSERT_EQ(36, m_entity_manager.GetComponent<Component2>(m_entity1).val);
        ASSERT_EQ(50, m_entity_manager.GetComponent<Component3>(m_entity2).val);
        ASSERT_EQ(14, m_entity_manager.GetComponent<Component2>(m_entity3).val);
        ASSERT_EQ(99, m_entity_manager.GetComponent<Component3>(m_entity3).val);

        // remove entity
        ASSERT_EQ(QCE::ErrorCode::SUCCESS,
            m_entity_manager.RemoveEntity(m_entity2));

        ASSERT_EQ(42, m_entity_manager.GetComponent<Component1>(m_entity1).val);
        ASSERT_EQ(36, m_entity_manager.GetComponent<Component2>(m_entity1).val);
        ASSERT_EQ(14, m_entity_manager.GetComponent<Component2>(m_entity3).val);
        ASSERT_EQ(99, m_entity_manager.GetComponent<Component3>(m_entity3).val);

        // remove entity component
        ASSERT_EQ(QCE::ErrorCode::SUCCESS,
            m_entity_manager.RemoveComponent<Component2>(m_entity3));

        ASSERT_EQ(42, m_entity_manager.GetComponent<Component1>(m_entity1).val);
        ASSERT_EQ(36, m_entity_manager.GetComponent<Component2>(m_entity1).val);
        ASSERT_EQ(99, m_entity_manager.GetComponent<Component3>(m_entity3).val);
    }
}
