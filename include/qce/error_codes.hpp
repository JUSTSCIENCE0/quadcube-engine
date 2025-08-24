// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(ErrorCode) \
        CU_ENUM_UNIT(SUCCESS) \
        CU_VALUED_ENUM_UNIT(GROUP_WARNING, 0)       \
        CU_VALUED_ENUM_UNIT(GROUP_ERROR,   0x10000) \
        CU_VALUED_ENUM_UNIT(GROUP_WINDOW,  0x10000) \
        CU_VALUED_ENUM_UNIT(GROUP_RENDER,  0x20000) \
        CU_VALUED_ENUM_UNIT(GROUP_DX12,    0x30000) \
        CU_VALUED_ENUM_UNIT(GROUP_VULKAN,  0x30000) \
    CU_END_ENUM(ErrorCode)

#include <cu/enum-utils.hpp>

    static inline bool is_success(ErrorCode code) {
        return ErrorCode::SUCCESS == code;
    }

    static inline bool is_fail(ErrorCode code) {
        return code > ErrorCode::GROUP_ERROR;
    }

    static inline std::string describe_error(ErrorCode code) {
        std::stringstream result;
        result << to_string(code);
        result << " (" << code << ") :";

        switch (code) {
        case ErrorCode::SUCCESS:
            result << "Everything is awesome";
            break;
        default:
            result << "Error has no description";
            break;
        }

        return result.str();
    }
}

#define QCE_CHECK_IMPL(operation, check_func, .../*return_value*/) do { \
    QCE::ErrorCode code = operation; \
    if (!QCE::is_success(code)) { \
        std::cout << QCE::describe_error(code) << std::endl; /* TODO: use cu log instead of stdout */ \
    } \
    __VA_OPT__( if (check_func(code)) return __VA_ARGS__; ) \
} while(0)

/// return if not success
#define QCE_CRITICAL(operation) \
    QCE_CHECK_IMPL(operation, !QCE::is_success, code)

/// return if not success
/// CRV - custom return value
#define QCE_CRITICAL_CRV(operation, return_value) \
    QCE_CHECK_IMPL(operation, !QCE::is_success, return_value)

/// return if fail
#define QCE_HARD(operation) \
    QCE_CHECK_IMPL(operation, QCE::is_fail, code)

/// return if fail
/// CRV - custom return value
#define QCE_HARD_CRV(operation, return_value) \
    QCE_CHECK_IMPL(operation, QCE::is_fail, return_value)

/// log if not success
#define QCE_SOFT(operation) \
    QCE_CHECK_IMPL(operation, !QCE::is_success)
