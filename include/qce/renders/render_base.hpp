// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/mjson/generated_objects.hpp>

#include <qce/ancillary/error_codes.hpp>
#include <qce/ancillary/structs.hpp>
#include <qce/objects/resource_manager.hpp>
#include <qce/ecs/ecs.hpp>

#include <array>
#include <utility>
#include <cstring>
#include <string>
#include <stdint.h>

namespace QCE {
    class RenderBase {
    public:
        RenderBase(Entities& entities, RenderConfig initial_config) :
            m_entities(entities),
            m_config(std::move(initial_config)) {
            m_shader_map.fill(ResourceManager::INVALID_RESOURCE_INDEX);
        }

        RenderBase(const RenderBase&) = delete;
        RenderBase(RenderBase&&) = delete;
        RenderBase& operator=(const RenderBase&) = delete;
        RenderBase& operator=(RenderBase&&) = delete;
        virtual ~RenderBase() = default;

        virtual ErrorCode UpdateConfig(RenderConfig config) {
            m_config = std::move(config);
            return ErrorCode::SUCCESS;
        }

        virtual ErrorCode Draw() = 0;
        virtual ErrorCode UpdateScene();

        ErrorCode UseShader(const std::string& name, ShaderType type);

    protected:
        /// consts
        static constexpr auto FRAME_RESOURCE_COUNT = 3;
        static constexpr auto MAX_LIGHTS = 16;
        static constexpr size_t NO_TEXTURE_INDEX = 0;
        static constexpr size_t MISSED_TEXTURE_INDEX = 1;

        /// types
        using ShaderMap = std::array<
            size_t /* index*/,
            ShaderType::E_SHADERS_TYPE_COUNT>;

        struct SceneGeometry {
            constexpr static uint32_t VERTEX_STRIDE = sizeof(vertex);

            struct Unit {
                uint32_t indeces_count = 0;
                uint32_t index_offset = 0;
                uint32_t vertex_offset = 0;
            };

            std::vector<vertex>  vertex_buffer;
            std::vector<index_t> index_buffer;

            uint32_t vertex_buffer_size = 0;
            uint32_t index_buffer_size = 0;

            std::vector<Unit> units;
        };

        struct SceneMaterials {
            std::vector<size_t> components;
            std::vector<int>    dirty_frames; // same indeces as material buffer
        };

        struct UnitConstants {
            float world_matrix[16];
        };

        struct PassConstants {
            float view_matrix[16] = {};
            float view_matrix_inv[16] = {};
            float proj_matrix[16] = {};
            float proj_matrix_inv[16] = {};
            float view_proj_matrix[16] = {};
            float view_proj_matrix_inv[16] = {};
            float eye_position[3] = { 0.0f, 0.0f, 0.0f };
            float padding1;
            float render_target_size[2] = { 0.0f, 0.0f };
            float render_target_size_inv[2] = { 0.0f, 0.0f };
            float near_z = 0.0f;
            float far_z = 0.0f;
            float delta_time = 0.0f;
            float padding2;

            float  ambient_light_color[4] = { 0.25f, 0.25f, 0.25f, 1.0f };
            struct SceneLights {
                struct Light {
                    float color[3] = {};
                    float falloff_begin = 0.0f;
                    float direction[3] = {};
                    float falloff_end = 0.0f;
                    float position[3] = {};
                    float spot_power = 0.0f;
                };
                Light lights[MAX_LIGHTS];

                int directional_light_end = 0;
                int point_light_end = 0;
                int spot_light_end = 0;
                int padding;
            } scene_lights;
        };

        struct MaterialConstants {
            float albedo_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
            float fresnel[3] = { 0.01f, 0.01f, 0.01f };
            float shininess = 0.75f;
        };

        /// attributes
        Entities&      m_entities;
        RenderConfig   m_config{};
        SceneGeometry  m_scene_static_geometry{};
        SceneGeometry  m_scene_dynamic_geometry{};
        SceneMaterials m_scene_materials{};

        // buffers map
        ShaderMap m_shader_map{};                // shader type -> shader index
        BufferMap m_static_geometry_unit_map{};  // mesh resource index -> scene static geometry unit index
        BufferMap m_dynamic_geometry_unit_map{}; // entity id -> scene dynamic geometry unit index
        BufferMap m_material_buffer_map{};       // material resource index -> scene materials buffer index

    private:
        void UpdateStaticGeometry();
        void UpdateDynamicGeometry();
        void UpdateSceneMaterials(const query_result& entities);
    };
}
