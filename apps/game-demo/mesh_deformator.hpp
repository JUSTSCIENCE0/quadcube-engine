// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/objects/command.hpp>
#include <qce/objects/figures.hpp>

using index_t = QCE::index_t;

struct AnimateHills : public QCE::BaseCommand {
    AnimateHills(const QCE::PlaneParams& plane_params, bool is_reflected = false);
    virtual ~AnimateHills() = default;

    QCE::ErrorCode Execute(const QCE::CommandContext* context) override;

private:
    static index_t CalculateVerticiesCount(const QCE::PlaneParams& plane_params, bool row);

    const float m_width;
    const float m_length;
    const index_t m_number_columns; // along width
    const index_t m_number_rows;    // along length
    const bool m_is_reflected;
};