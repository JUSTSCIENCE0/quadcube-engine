// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <gtest/gtest.h>

#include <qce/ecs/component.hpp>

TEST(ComponentStorageTest, BaseFunctional) {
    QCE::ComponentStorage<int> storage;
    std::set<CU::id_t> expected_entities;

    // check SetEntity
    storage.SetEntity(2, 42);
    storage.SetEntity(4, 55);
    storage.SetEntity(6, 100);
    expected_entities = { 2, 4, 6 };

    EXPECT_EQ(storage.Size(), 3);
    EXPECT_EQ(storage.GetActualEntities(), expected_entities);
    EXPECT_EQ(storage[2], 42);
    EXPECT_EQ(storage[4], 55);
    EXPECT_EQ(storage[6], 100);

    // check update existing entity
    storage.SetEntity(2, 84);

    EXPECT_EQ(storage.Size(), 3);
    EXPECT_EQ(storage.GetActualEntities(), expected_entities);
    EXPECT_EQ(storage[2], 84);
    EXPECT_EQ(storage[4], 55);
    EXPECT_EQ(storage[6], 100);

    // check remove entity
    storage.RemoveEntity(4);
    expected_entities = { 2, 6 };

    EXPECT_EQ(storage.Size(), 2);
    EXPECT_EQ(storage.GetActualEntities(), expected_entities);
    EXPECT_EQ(storage[2], 84);
    EXPECT_EQ(storage[6], 100);

    // check adding after removal
    storage.SetEntity(3, 200);
    expected_entities = { 2, 3, 6 };

    EXPECT_EQ(storage.Size(), 3);
    EXPECT_EQ(storage.GetActualEntities(), expected_entities);
    EXPECT_EQ(storage[2], 84);
    EXPECT_EQ(storage[3], 200);
    EXPECT_EQ(storage[6], 100);
}

TEST(ComponentStorageTest, SharedPtr) {
    QCE::ComponentStorage<std::shared_ptr<int>> storage;

    auto val1 = std::make_shared<int>(10);
    auto val2 = std::make_shared<int>(20);
    storage.SetEntity(1, val1);
    storage.SetEntity(2, val2);

    EXPECT_EQ(val1.use_count(), 2);
    EXPECT_EQ(val2.use_count(), 2);

    storage.RemoveEntity(1);

    EXPECT_EQ(val1.use_count(), 1);
    EXPECT_EQ(val2.use_count(), 2);
}
