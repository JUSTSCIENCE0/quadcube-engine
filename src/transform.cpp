// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifndef CU_BUILD_SPECIFIC_SIMD
#  define CU_BUILD_SPECIFIC_SIMD def /* TODO sse2/avx2 */
#endif // !CU_BUILD_SPECIFIC_SIMD

#include <qce/transform.hpp>

namespace QCE {
    Transform::Transform(const float4x4& start_matrix) :
        m_matrix(start_matrix) {
        UpdateSRT();
    }

    Transform::Transform(
            const quaternion& rotation,
            const float3d& position,
            const float3d& scale) :
        m_rotation(rotation),
        m_position(position),
        m_scale(scale) {
        UpdateMatrix(true);
    }

    void Transform::UpdateMatrix(bool check_normalized) {
        // TODO
    }

    void Transform::UpdateSRT() {
        // TODO
    }
}
