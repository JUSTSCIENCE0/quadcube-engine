// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include "mesh_deformator.hpp"

AnimateHills::AnimateHills(const QCE::PlaneParams& plane_params, bool is_reflected) :
        QCE::BaseCommand("AnimateHills"),
        m_width(plane_params.width),
        m_length(plane_params.length),
        m_number_columns(CalculateVerticiesCount(plane_params, false)),
        m_number_rows(CalculateVerticiesCount(plane_params, true)),
        m_is_reflected(is_reflected) {
    
}

QCE::ErrorCode AnimateHills::Execute(QCE::CommandContext* context) {
    return QCE::ErrorCode::SUCCESS;
}

index_t AnimateHills::CalculateVerticiesCount(const QCE::PlaneParams& plane_params, bool rows) {
    if (plane_params.unit_squares) {
        if (rows)
            return static_cast<index_t>(plane_params.length) + 1;
        else
            return static_cast<index_t>(plane_params.width) + 1;
    }
    else
        return 2;
}

