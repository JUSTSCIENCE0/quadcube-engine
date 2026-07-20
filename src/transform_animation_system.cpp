// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/systems/animation_systems.hpp>
#include <qce/objects/resource_manager.hpp>
#include <qce/ancillary/timer.hpp>

namespace QCE {
    template<KeyChannel Channel>
    bool has_channel_animation(const Channel& channel) {
        return channel.size() > 1;
    }

    template<KeyChannel Channel>
    static inline void update_channel_key(
            const Channel& keys,
            TransformAnimationComponent& animation_comp) {
        assert(has_channel_animation(keys));

        // Now it only supports direct direction of time from the past to the future
        // TODO: support for negative time direction

        const auto  last_key = keys.size() - 1;
        const auto  current_time  = animation_comp.current_time;

        size_t* current_index = nullptr;
        if constexpr (std::is_same_v<typename Channel::value_type, TransformAnimation::PositionKey>) {
            current_index = &animation_comp.current_position_key;
        }
        else if constexpr (std::is_same_v<typename Channel::value_type, TransformAnimation::RotationKey>) {
            current_index = &animation_comp.current_rotation_key;
        }
        else if constexpr (std::is_same_v<typename Channel::value_type, TransformAnimation::ScaleKey>) {
            current_index = &animation_comp.current_scale_key;
        }
        else {
            static_assert(0 == sizeof(Channel), "Unsupported channel type!");
        }

        assert(current_time >= 0.0f);
        assert(*current_index <= last_key);

        const auto* current_key = &keys[*current_index];
        assert(current_key->start_time <= current_time);

        // check if animation is already finished
        if (*current_index == last_key) {
            return;
        }

        const auto* next_key = &keys[*current_index + 1];

        while (next_key->start_time <= current_time) {
            (*current_index)++;
            next_key++;

            if (*current_index == last_key) {
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

            if (has_channel_animation(animation.position_channel)) {
                update_channel_key(animation.position_channel, animation_comp);
                // TODO
            }

            if (has_channel_animation(animation.rotation_channel)) {
                update_channel_key(animation.rotation_channel, animation_comp);
                // TODO
            }

            if (has_channel_animation(animation.scale_channel)) {
                update_channel_key(animation.scale_channel, animation_comp);
                // TODO
            }

        }


        return ErrorCode::SUCCESS;
    }
}
