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

    // TODO: make it template update_key
    static inline void update_position_key(
            const TransformAnimation& animation,
            TransformAnimationComponent& animation_comp) {
        assert(has_position_animation(animation));

        // Now it only supports direct direction of time from the past to the future
        // TODO: support for negative time direction

        const auto& keys = animation.position_channel;
        const auto  last_key = keys.size() - 1;
        const auto  current_time = animation_comp.current_time;
        auto&       current_index = animation_comp.current_position_key;

        assert(current_time >= 0.0f);
        assert(current_index <= last_key);

        const auto* current_key = &keys[current_index];
        assert(current_key->start_time <= current_time);

        // check if animation is already finished
        if (current_index == last_key) {
            return;
        }

        const auto* next_key = &keys[current_index + 1];

        while (next_key->start_time <= current_time) {
            current_index++;
            next_key++;

            if (current_index == last_key) {
                // animation finished
                return;
            }
        }
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
            if (bool is_animation_finished =
                    animation_comp.current_time >= animation.total_duration) {
                if (animation_comp.is_looped) {

                    animation_comp.current_time -= animation.total_duration;
                    animation_comp.current_rotation_key = 0;
                    animation_comp.current_position_key = 0;
                    animation_comp.current_scale_key = 0;
                }
                else {
                    QCE_CRITICAL(
                        m_entities.RemoveComponent<TransformAnimationComponent>(entity_id));

                    // TODO: if time can go backwards, we need to save animations history
                }
            }

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
