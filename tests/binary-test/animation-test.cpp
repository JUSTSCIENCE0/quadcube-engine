// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <gtest/gtest.h>

#include <qce/loaders/binary/resources.hpp>

using namespace QCE;

class AnimationSerializationTest :
        public ::testing::TestWithParam<
            std::tuple<const TransformAnimation*, TransformAnimationCompressionParams>> {
protected:
    void Process() {
        Buffer buffer;
        auto written_bytes = bitsery::quickSerialization<OutputAdapter>(buffer, source_animation);
        ASSERT_GT(written_bytes, 0) << "Failed to serialize animation";

        auto state = bitsery::quickDeserialization<InputAdapter>(
            { buffer.begin(), written_bytes }, deserialized_animation);
        ASSERT_EQ(state.first, bitsery::ReaderError::NoError);
        ASSERT_TRUE(state.second) << "Not all data in the buffer was deserialized";
    }

    void Validate() {
        EXPECT_EQ(source_animation.id, deserialized_animation.id);

        for (size_t i = 0; i < source_animation.rotation_channel.size(); ++i) {
            const auto& src = source_animation.rotation_channel[i];
            const auto& des = deserialized_animation.rotation_channel[i];

            CheckQuaternions(src.value, des.value);
            EXPECT_FLOAT_EQ(src.start_time, des.start_time);
            EXPECT_EQ(src.easing, des.easing);
        }

        for (size_t i = 0; i < source_animation.position_channel.size(); ++i) {
            const auto& src = source_animation.position_channel[i];
            const auto& des = deserialized_animation.position_channel[i];
            CheckFloat3d(src.value, des.value, compression_params.position_eps);
            EXPECT_FLOAT_EQ(src.start_time, des.start_time);
            EXPECT_EQ(src.easing, des.easing);
        }

        for (size_t i = 0; i < source_animation.scale_channel.size(); ++i) {
            const auto& src = source_animation.scale_channel[i];
            const auto& des = deserialized_animation.scale_channel[i];
            CheckFloat3d(src.value, des.value, compression_params.scale_eps);
            EXPECT_FLOAT_EQ(src.start_time, des.start_time);
            EXPECT_EQ(src.easing, des.easing);
        }

        EXPECT_FLOAT_EQ(source_animation.total_duration, deserialized_animation.total_duration);
        EXPECT_EQ(source_animation.spline_func, deserialized_animation.spline_func);
    }

private:
    float GetQuaternionDotError() {
        switch (compression_params.rotation_quantization) {
        case E_32_BIT_QQ: {
            constexpr float AngularError = deg_to_rad(0.5f);
            return std::cos(AngularError / 2);
        }
        case E_64_BIT_QQ: {
            constexpr float AngularError = deg_to_rad(0.0005f);
            return std::cos(AngularError / 2);
        }
        default:
            return 0.0f;
        }
    }

    void CheckQuaternions(const quaternion& lhs, const quaternion& rhs) {
        if (compression_params.rotation_quantization == E_NO_QQ) {
            EXPECT_FLOAT_EQ(lhs.x(), rhs.x()) << "Quaternion error";
            EXPECT_FLOAT_EQ(lhs.y(), rhs.y()) << "Quaternion error";
            EXPECT_FLOAT_EQ(lhs.z(), rhs.z()) << "Quaternion error";
            EXPECT_FLOAT_EQ(lhs.w(), rhs.w()) << "Quaternion error";
        }
        else {
            const float dot = std::abs(
                lhs.x() * rhs.x() +
                lhs.y() * rhs.y() +
                lhs.z() * rhs.z() +
                lhs.w() * rhs.w());

            EXPECT_NEAR(dot, 1.0f, GetQuaternionDotError()) << "Quaternion error";
        }
    }

    void CheckFloat3d(const float3d& lhs, const float3d& rhs, float eps) {
        EXPECT_NEAR(lhs.x(), rhs.x(), eps) << "Float3D error";
        EXPECT_NEAR(lhs.y(), rhs.y(), eps) << "Float3D error";
        EXPECT_NEAR(lhs.z(), rhs.z(), eps) << "Float3D error";
    }

    using Buffer = std::vector<uint8_t>;
    using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
    using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

    const TransformAnimation& source_animation = *std::get<0>(GetParam());
    const TransformAnimationCompressionParams& compression_params = std::get<1>(GetParam());
    TransformAnimation deserialized_animation{};
};

static inline TransformAnimation generate_full_sample_animation() {
    TransformAnimation square_path{};
    square_path.id = "square_path";
    square_path.position_channel = {
        {
            /*value*/ { -6.968599f, 1.783926f, 0.204209f },
            /*start_time*/ 0.0f
        },
        {
            /*value*/ { -7.277121f, 9.905811f, 6.174850f },
            /*start_time*/ 1.0f
        },
        {
            /*value*/ { -2.614493f, 3.784512f, -4.221519f },
            /*start_time*/ 2.0f
        },
        {
            /*value*/ { 3.799920f, 9.625124f, 0.092419f },
            /*start_time*/ 3.0f
        },
        {
            /*value*/ { -6.587712f, 0.703252f, 5.143878f },
            /*start_time*/ 4.0f
        }
    };
    square_path.scale_channel = {
        {
            /*value*/ { 1.261417f, 0.237270f, 6.808356f },
            /*start_time*/ 1.0f
        },
        {
            /*value*/ { 4.929912f, 9.098186f, 5.612872f },
            /*start_time*/ 2.0f
        },
        {
            /*value*/ { 1.807719f, 2.010254f, 7.056530f },
            /*start_time*/ 3.0f
        },
        {
            /*value*/ { 2.931365f, 5.758866f, 9.438544f },
            /*start_time*/ 4.0f
        }
    };
    square_path.rotation_channel = {
        {
            /*value*/ euler_deg_to_quaternion(265.0f, 257.0f, 108.0f),
            /*start_time*/ 0.0f
        },
        {
            /*value*/ euler_deg_to_quaternion(250.0f, 15.0f, 339.0f),
            /*start_time*/ 1.0f
        },
        {
            /*value*/ euler_deg_to_quaternion(140.0f, 107.0f, 308.0f),
            /*start_time*/ 2.0f
        },
        {
            /*value*/ euler_deg_to_quaternion(314.0f, 178.0f, 319.0f),
            /*start_time*/ 3.0f
        },
        {
            /*value*/ euler_deg_to_quaternion(51.0f, 252.0f, 4.0f),
            /*start_time*/ 4.0f
        }
    };
    square_path.total_duration = calculate_animation_duration(square_path);

    return square_path;
}

static TransformAnimation full_sample_animation = generate_full_sample_animation();
static TransformAnimationCompressionParams default_compression{};

INSTANTIATE_TEST_SUITE_P(
    Base,
    AnimationSerializationTest,
    ::testing::Values(
        std::make_tuple(&full_sample_animation, default_compression)
    )
);

TEST_P(AnimationSerializationTest, SerializeDeserialize) {
    Process();
    Validate();
}
