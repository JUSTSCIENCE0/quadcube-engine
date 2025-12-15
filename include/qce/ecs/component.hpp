// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <cu/id-utils.hpp>

#include <stack>
#include <vector>
#include <utility>
#include <memory>
#include <type_traits>

namespace QCE {
    template <typename Component>
    class ComponentStorage {
    public:
        void Reserve(size_t count) {
            m_map.resize(count, CU::INVALID_ID);
        }

        void SetEntity(CU::id_t entity_id, Component component) {
            if (entity_id >= m_map.size()) {
                m_map.resize(entity_id + 1, CU::INVALID_ID);
            }

            Cell new_value{
                .entity_id = entity_id,
                .value = std::move(component)
            };

            if (m_map[entity_id] == CU::INVALID_ID) {
                if (m_empty_cells.empty()) {
                    m_map[entity_id] = m_data.size();
                    m_data.emplace_back(std::move(new_value));
                }
                else {
                    size_t cell_index = m_empty_cells.top();
                    m_empty_cells.pop();
                    m_map[entity_id] = cell_index;
                    m_data[cell_index] = std::move(new_value);
                }
            }
            else {
                m_data[m_map[entity_id]] = std::move(new_value);
            }
        }

        Component& operator[](CU::id_t entity_id) {
            assert(HasEntity(entity_id));

            size_t data_index = m_map[entity_id];
            return m_data[data_index].value;
        }

        const Component& operator[](CU::id_t entity_id) const {
            assert(HasEntity(entity_id));

            size_t data_index = m_map[entity_id];
            return m_data[data_index].value;
        }

        size_t Size() const {
            return m_data.size() - m_empty_cells.size();
        }

        std::set<CU::id_t> GetActualEntities() const {
            std::set<CU::id_t> result;
            for (const auto& cell : m_data) {
                if (cell.entity_id != CU::INVALID_ID)
                 result.emplace(cell.entity_id);
            }
            return result;
        }

        bool HasEntity(CU::id_t entity_id) const {
            if (entity_id >= m_map.size())
                return false;

            return m_map[entity_id] != CU::INVALID_ID;
        }

        void RemoveEntity(CU::id_t entity_id) {
            if (entity_id >= m_map.size())
                return;

            size_t data_index = m_map[entity_id];
            if (data_index == CU::INVALID_ID)
                return;

            m_map[entity_id] = CU::INVALID_ID;
            m_data[data_index].entity_id = CU::INVALID_ID;

            m_empty_cells.push(data_index);

            if constexpr (requires(Component comp) { comp.reset(); }) {
                m_data[data_index].value.reset();
            }
        }

    private:
        struct Cell {
            CU::id_t  entity_id = CU::INVALID_ID;
            Component value;
        };

        std::vector<size_t> m_map;
        std::vector<Cell>   m_data;
        std::stack<size_t>  m_empty_cells;
    };
}
