// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>

#if defined(USE_BITSERY_SERIALIZER)
#  include <bitsery/ext/value_range.h>
#endif // USE_BITSERY_SERIALIZER

namespace bitsery {
    namespace ext {
        class QuaternionCompressor {
        public:
            template<typename Ser, typename Fnc>
            void serialize(Ser& ser, const QCE::quaternion& q, Fnc&&) const {
#if !defined(USE_BITSERY_SERIALIZER)
                auto compressed = QCE::compress_quaternion(q);
                ser.adapter().template writeBytes<sizeof(compressed)>(compressed);
#else
                float tmp[4] = { std::fabs(q.x()), std::fabs(q.y()), std::fabs(q.z()), std::fabs(q.w()) };

                const auto max_abs = std::max_element(tmp, tmp + 4);
                const auto max_index = static_cast<uint8_t>(std::distance(tmp, max_abs));
                const auto max_it = &q.arr[max_index];
                const auto k = (*max_it < 0.0f) ? -1.0f : 1.0f;

                float smallest_three[3];
                int i = 0;
                for (auto it = q.arr; it != q.arr + 4; ++it) {
                    if (it != max_it) {
                        smallest_three[i++] = (*it) * k;
                    }
                }

                auto write = [&ser](auto value, size_t Bits) {
                    using T = decltype(value);
                    ser.adapter().template writeBits<T>(value, Bits);
                };
                auto quantize = [this](float value) {
                    return details::getRangeValue(value, RANGE_SPEC);
                };

                write(max_index, 2);
                write(quantize(smallest_three[0]), BITS_PER_COMPONENT);
                write(quantize(smallest_three[1]), BITS_PER_COMPONENT);
                write(quantize(smallest_three[2]), BITS_PER_COMPONENT);
#endif // !USE_BITSERY_SERIALIZER
            }

            template<typename Des, typename Fnc>
            void deserialize(Des& des, QCE::quaternion& q, Fnc&&) const {
#if !defined(USE_BITSERY_SERIALIZER)
                auto& reader = des.adapter();
                uint32_t compressed = 0;
                reader.template readBytes<sizeof(uint32_t)>(compressed);
                q = QCE::decompress_quaternion(compressed);
#else
                auto& reader = des.adapter();
                uint8_t max_index = 0;
                reader.readBits(max_index, 2);
                float smallest_three[3];
                reader.readBits(reinterpret_cast<details::SameSizeUnsigned<float>&>(smallest_three[0]), BITS_PER_COMPONENT);
                details::setRangeValue(smallest_three[0], RANGE_SPEC);
                CheckRange(
                    reader, smallest_three[0], std::integral_constant<bool, Des::TConfig::CheckDataErrors>{});

                reader.readBits(reinterpret_cast<details::SameSizeUnsigned<float>&>(smallest_three[1]), BITS_PER_COMPONENT);
                details::setRangeValue(smallest_three[1], RANGE_SPEC);
                CheckRange(
                    reader, smallest_three[1], std::integral_constant<bool, Des::TConfig::CheckDataErrors>{});

                reader.readBits(reinterpret_cast<details::SameSizeUnsigned<float>&>(smallest_three[2]), BITS_PER_COMPONENT);
                details::setRangeValue(smallest_three[2], RANGE_SPEC);
                CheckRange(
                    reader, smallest_three[2], std::integral_constant<bool, Des::TConfig::CheckDataErrors>{});

                static_assert(false, "Not fully implemented yet");
#endif // !USE_BITSERY_SERIALIZER
            }

#if defined(USE_BITSERY_SERIALIZER)
        private:
            template<typename Reader>
            void CheckRange(Reader& reader, float& v, std::true_type) const {
                if (!details::isRangeValid(v, RANGE_SPEC)) {
                    reader.error(ReaderError::InvalidData);
                    v = RANGE_SPEC.min;
                }
            }

            template<typename Reader>
            void CheckRange(Reader&, float&, std::false_type) const {}

            static constexpr size_t BITS_PER_COMPONENT = 10;
            static constexpr auto   RANGE_SPEC = bitsery::details::RangeSpec<float>(
                -QCE::SIN45, QCE::SIN45, bitsery::ext::BitsConstraint(BITS_PER_COMPONENT));
#endif // USE_BITSERY_SERIALIZER
        };
    }

    namespace traits {
        template<typename T>
        struct ExtensionTraits<ext::QuaternionCompressor, T>
        {
            using TValue = void;
            static constexpr bool SupportValueOverload = false;
            static constexpr bool SupportObjectOverload = true;
            static constexpr bool SupportLambdaOverload = false;
        };
    }
}

namespace QCE {
    template<typename S>
    void serialize(S& s, float3d& value) {
        s.value4b(value.x());
        s.value4b(value.y());
        s.value4b(value.z());
    }

    template<typename S>
    void serialize(S& s, quaternion& value) {
        s.ext(value, bitsery::ext::QuaternionCompressor{});
    }
}
