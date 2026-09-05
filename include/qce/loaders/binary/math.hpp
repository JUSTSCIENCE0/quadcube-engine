// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/loaders/binary/compression.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>


namespace bitsery {
    namespace ext {
        class QuaternionCompressor {
        public:
            QCE::QuaternionQuantization quantization = QCE::E_32_BIT_QQ;

            template<typename Ser, typename Fnc>
            void serialize(Ser& ser, const QCE::quaternion& q, Fnc&&) const {
                switch (quantization) {
                case QCE::E_32_BIT_QQ: {
                    auto compressed = QCE::compress_quaternion<uint32_t>(q);
                    ser.value4b(compressed);
                    break;
                }
                case QCE::E_64_BIT_QQ: {
                    auto compressed = QCE::compress_quaternion<uint64_t>(q);
                    ser.value8b(compressed);
                    break;
                }
                default:
                    ser.value4b(q.x());
                    ser.value4b(q.y());
                    ser.value4b(q.z());
                    ser.value4b(q.w());
                    break;
                }
            }

            template<typename Des, typename Fnc>
            void deserialize(Des& des, QCE::quaternion& q, Fnc&&) const {
                switch (quantization) {
                case QCE::E_32_BIT_QQ: {
                    uint32_t compressed = 0;
                    des.value4b(compressed);
                    q = QCE::decompress_quaternion(compressed);
                    break;
                }
                case QCE::E_64_BIT_QQ: {
                    uint64_t compressed = 0;
                    des.value8b(compressed);
                    q = QCE::decompress_quaternion(compressed);
                    break;
                }
                default:
                    des.value4b(q.x());
                    des.value4b(q.y());
                    des.value4b(q.z());
                    des.value4b(q.w());
                    break;
                }
            }
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
