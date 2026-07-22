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
    void update_channel_key(
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

    static inline float apply_easing(float t, EasingFunc easing) {
        switch (easing) {
            case EasingFunc::E_EASING_LINEAR:
                return t;
            case EasingFunc::E_EASING_EASE_IN:
            case EasingFunc::E_EASING_EASE_OUT:
            case EasingFunc::E_EASING_EASE_IN_OUT:
                assert(!"TODO: Implement easing functions");
                return t;
            default:
                assert(!"Unknown easing function");
                return t;
        };
    }

    static inline float calc_interpolation_factor(float start_time, float end_time, float current_time, EasingFunc easing) {
        const auto delta_time = end_time - start_time;
        const auto t = (current_time - start_time) / delta_time;
        return apply_easing(t, easing);
    }

    template<KeyChannel Channel, typename OutType>
    static inline void update_transform_component(
            const Channel& keys,
            size_t current_index, float current_time,
            OutType& output) {
        // Now it only supports direct direction of time from the past to the future
        // TODO: support for negative time direction

        const auto  last_index = keys.size() - 1;
        const auto& current_key = keys[current_index];

        bool is_not_started = (0 == current_index) && (current_time < current_key.start_time);
        bool is_finished    = (last_index == current_index);
        if (is_not_started || is_finished) {
            output = current_key.value;
            return;
        }

        auto& next_key = keys[current_index + 1];
        const auto t = calc_interpolation_factor(
            current_key.start_time, next_key.start_time, current_time, current_key.easing);
        if constexpr (std::is_same_v<OutType, float3d>)
            lerp4(current_key.value.arr, next_key.value.arr, t, output.arr);
        // TODO: else if constexpr (std::is_same_v<OutType, quaternion>) {
        //     slerp(current_key.value, next_key.value, t, output);
        // }
        else
            static_assert(0 == sizeof(OutType), "Unsupported type for interpolation");
    }

    ErrorCode TransformAnimationSystem::Update() {
        auto entities = m_entities.QueryEntities<
            TransformComponents,
            TransformAnimationComponent>();

        for (const auto& entity_id : entities) {
            auto& transform_comp = m_entities.GetComponent<TransformComponents>(entity_id);
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
                update_transform_component(
                    animation.position_channel,
                    animation_comp.current_position_key,
                    animation_comp.current_time,
                    transform_comp.position);
            }

            if (has_channel_animation(animation.rotation_channel)) {
                update_channel_key(animation.rotation_channel, animation_comp);
                // TODO
            }

            if (has_channel_animation(animation.scale_channel)) {
                update_channel_key(animation.scale_channel, animation_comp);
                update_transform_component(
                    animation.scale_channel,
                    animation_comp.current_scale_key,
                    animation_comp.current_time,
                    transform_comp.scale);
            }

            if (m_entities.HasComponent<TransformMatrix>(entity_id)) {
                auto& transform_matrix = m_entities.GetComponent<TransformMatrix>(entity_id);
                transform_matrix.actual = false;
            }
        }


        return ErrorCode::SUCCESS;
    }
}
