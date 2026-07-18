// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/systems/animation_systems.hpp>
#include <qce/objects/resource_manager.hpp>
#include <qce/ancillary/timer.hpp>

namespace QCE {
    static inline bool has_position_animation(const TransformAnimation& animation) {
        return animation.position_channel.size() > 1;
    }
    static inline bool has_rotation_animation(const TransformAnimation& animation) {
        return animation.rotation_channel.size() > 1;
    }
    static inline bool has_scale_animation(const TransformAnimation& animation) {
        return animation.scale_channel.size() > 1;
    }

    static inline float update_position_key(
            const TransformAnimation& animation,
            TransformAnimationComponent& animation_comp) {
        assert(has_position_animation(animation));

        // Now it only supports direct direction of time from the past to the future
        // TODO: support for negative time direction

        const auto& positions = animation.position_channel;
        const auto  last_key = positions.size() - 1;
        const auto  current_time = animation_comp.current_time;
        auto&       current_index = animation_comp.current_position_key;

        assert(current_time >= 0.0f);
        assert(current_index <= last_key);

        //const auto& current_key = positions[current_index];

        // check if animation is already finished
        if (current_index == last_key) {
            return 0.0f;
        }

        return 0.0f;
    }

    // updates position transform component
    static inline void calc_position(
            const TransformAnimation& animation,
            TransformAnimationComponent& animation_comp,
            TransformComponents& transform) {
    }

    ErrorCode TransformAnimationSystem::Update() {
        auto entities = m_entities.QueryEntities<
            TransformComponents,
            TransformAnimationComponent>();

        for (const auto& entity_id : entities) {
            //auto& transform_comp = m_entities.GetComponent<TransformComponents>(entity_id);
            auto& animation_comp = m_entities.GetComponent<TransformAnimationComponent>(entity_id);
            auto& animation = ResourceManager::Get().Read<TransformAnimation>(animation_comp.index);

            // TODO: use a separate time source that can run at a speed different from real time,
            // stop, or go backwards
            animation_comp.current_time += FrameTime::Get().Elapsed<float>();
            //auto is_animation_finished = false;

            if (has_position_animation(animation)) {
                update_position_key(animation, animation_comp);
                // TODO
            }

            if (has_rotation_animation(animation)) {
                // TODO
            }

            if (has_scale_animation(animation)) {
                // TODO
            }

        }


        return ErrorCode::SUCCESS;
    }
}
