// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <exception>
#include <cassert>

namespace QCE {
#define CU_ENUMS_DESCRIPTION \
    CU_BEGIN_ENUM(ErrorCode) \
        CU_ENUM_UNIT(SUCCESS) \
        CU_VALUED_ENUM_UNIT(GROUP_WARNING,          0x1)     \
        CU_VALUED_ENUM_UNIT(GROUP_ERROR,            0xFFFF)  \
        CU_VALUED_ENUM_UNIT(GROUP_WINDOW,           0x10000) \
        CU_ENUM_UNIT(E_WINNT_REGISRER_FAILED) \
        CU_ENUM_UNIT(E_WINNT_INVALID_WINDOW_RECT) \
        CU_ENUM_UNIT(E_WINNT_CREATE_FAILED) \
        CU_VALUED_ENUM_UNIT(GROUP_RENDER,           0x20000) \
        CU_VALUED_ENUM_UNIT(GROUP_DX12,             0x30000) \
        CU_ENUM_UNIT(E_DX12_CREATE_DXGI_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_DEVICE_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_FENCE_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_COMMAND_QUEUE_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_COMMAND_ALLOCATOR_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_COMMAND_LIST_FAILED) \
        CU_ENUM_UNIT(E_DX12_CLOSE_COMMAND_LIST_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_SWAP_CHAIN_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_RTV_HEAP_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_DSV_HEAP_FAILED) \
        CU_ENUM_UNIT(E_DX12_QUEUE_ADD_SIGNAL_FAILED) \
        CU_ENUM_UNIT(E_DX12_SET_EVENT_ON_COMPLETE_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_EVENT_FAILED) \
        CU_ENUM_UNIT(E_DX12_RESET_COMMAND_LIST_FAILED) \
        CU_ENUM_UNIT(E_DX12_SWAP_CHAIN_RESIZE_BUFFERS_FAILED) \
        CU_ENUM_UNIT(E_DX12_SWAP_CHAIN_GET_BUFFER_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_DS_BUFFER_FAILED) \
        CU_ENUM_UNIT(E_DX12_RESET_COMMAND_ALLOCATOR_FAILED) \
        CU_ENUM_UNIT(E_DX12_PRESENT_SWAP_CHAIN_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_DEFAULT_BUFFER_RESOURCE_FAILED) \
        CU_ENUM_UNIT(E_DX12_CREATE_UPLOAD_BUFFER_RESOURCE_FAILED) \
        CU_ENUM_UNIT(E_DX12_MAP_UPLOAD_BUFFER_FAILED) \
        CU_VALUED_ENUM_UNIT(GROUP_VULKAN,           0x40000) \
        CU_VALUED_ENUM_UNIT(GROUP_ENGINE,           0x50000) \
        CU_VALUED_ENUM_UNIT(GROUP_RESOURCE_MANAGER, 0x60000) \
        CU_ENUM_UNIT(E_RM_MESH_ID_COLLISION) \
        CU_ENUM_UNIT(E_RM_MODEL_ID_COLLISION) \
        CU_ENUM_UNIT(E_RM_MESH_NOT_FOUND) \
    CU_END_ENUM(ErrorCode)
#include <cu/enum-utils.hpp>
#undef CU_ENUMS_DESCRIPTION

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

        /* Group Warning*/

        /* Group Window*/
        case ErrorCode::E_WINNT_REGISRER_FAILED:
            result << "WinNTWindow - RegisterClass Failed";
            break;
        case ErrorCode::E_WINNT_CREATE_FAILED:
            result << "WinNTWindow - CreateWindow Failed";
            break;
        case ErrorCode::E_WINNT_INVALID_WINDOW_RECT:
            result << "WinNTWindow - AdjustWindowRect Failed";
            break;

        /* Group Render*/

        /* Group DX12*/
        case ErrorCode::E_DX12_CREATE_DXGI_FAILED:
            result << "DX12 - CreateDXGIFactory Failed";
            break;
        case ErrorCode::E_DX12_CREATE_FENCE_FAILED:
            result << "DX12 - CreateFence Failed";
            break;
        case ErrorCode::E_DX12_CREATE_DEVICE_FAILED:
            result << "DX12 - CreateDevice Failed";
            break;
        case ErrorCode::E_DX12_CREATE_COMMAND_QUEUE_FAILED:
            result << "DX12 - CreateCommandQueue Failed";
            break;
        case ErrorCode::E_DX12_CREATE_COMMAND_ALLOCATOR_FAILED:
            result << "DX12 - CreateCommandAllocator Failed";
            break;
        case ErrorCode::E_DX12_CREATE_COMMAND_LIST_FAILED:
            result << "DX12 - CreateCommandList Failed";
            break;
        case ErrorCode::E_DX12_CLOSE_COMMAND_LIST_FAILED:
            result << "DX12 - CommandList Close Failed";
            break;
        case ErrorCode::E_DX12_CREATE_SWAP_CHAIN_FAILED:
            result << "DX12 - CreateSwapChain Failed";
            break;
        case ErrorCode::E_DX12_CREATE_RTV_HEAP_FAILED:
            result << "DX12 - RTV CreateDescriptorHeap Failed";
            break;
        case ErrorCode::E_DX12_CREATE_DSV_HEAP_FAILED:
            result << "DX12 - DSV CreateDescriptorHeap Failed";
            break;
        case ErrorCode::E_DX12_QUEUE_ADD_SIGNAL_FAILED:
            result << "DX12 - Add Signal to CommandQueue Failed";
            break;
        case ErrorCode::E_DX12_SET_EVENT_ON_COMPLETE_FAILED:
            result << "DX12 - Fence SetEventOnCompletion Failed";
            break;
        case ErrorCode::E_DX12_CREATE_EVENT_FAILED:
            result << "DX12 - CreateEvent Failed";
            break;
        case ErrorCode::E_DX12_RESET_COMMAND_LIST_FAILED:
            result << "DX12 - CommandList Reset Failed";
            break;
        case ErrorCode::E_DX12_SWAP_CHAIN_RESIZE_BUFFERS_FAILED:
            result << "DX12 - SwapChain ResizeBuffers Failed";
            break;
        case ErrorCode::E_DX12_SWAP_CHAIN_GET_BUFFER_FAILED:
            result << "DX12 - SwapChain GetBuffer Failed";
            break;
        case ErrorCode::E_DX12_CREATE_DS_BUFFER_FAILED:
            result << "DX12 - Create Depth Stencil Buffer Failed";
            break;
        case ErrorCode::E_DX12_RESET_COMMAND_ALLOCATOR_FAILED:
            result << "DX12 - Reset CommandAllocator Failed";
            break;
        case ErrorCode::E_DX12_PRESENT_SWAP_CHAIN_FAILED:
            result << "DX12 - Present SwapChain Failed";
            break;
        case ErrorCode::E_DX12_CREATE_DEFAULT_BUFFER_RESOURCE_FAILED:
            result << "DX12 - CreateCommittedResource HEAP_TYPE_DEFAULT Failed";
            break;
        case ErrorCode::E_DX12_CREATE_UPLOAD_BUFFER_RESOURCE_FAILED:
            result << "DX12 - CreateCommittedResource HEAP_TYPE_UPLOAD Failed";
            break;
        case ErrorCode::E_DX12_MAP_UPLOAD_BUFFER_FAILED:
            result << "DX12 - Upload Buffer Map Failed";
            break;

        /* Group Vulkan*/

        /* Group Engine*/

        /* Group Resource Manager*/
        case ErrorCode::E_RM_MESH_ID_COLLISION:
            result << "Resource Manager - Mesh with such Id already exitst";
            break;
        case ErrorCode::E_RM_MODEL_ID_COLLISION:
            result << "Resource Manager - Model with such Id already exitst";
            break;
        case ErrorCode::E_RM_MESH_NOT_FOUND:
            result << "Resource Manager - Mesh with such Id not found";
            break;

        default:
            result << "Error has no description";
            break;
        }

        return result.str();
    }

    class ErrorCodeException final :
            public std::exception {
    public:
        explicit ErrorCodeException(ErrorCode code):
            m_code(code),
            m_message(describe_error(code)) {}

        ErrorCode code_value() const noexcept { return m_code; }

        const char* what() const noexcept override {
            return m_message.c_str();
        }
    private:
        const ErrorCode m_code;
        const std::string m_message;
    };
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

#define QCE_THROW_IMPL(operation, check_func) do { \
    QCE::ErrorCode code = operation; \
    if (check_func(code)) throw QCE::ErrorCodeException(code); \
} while(0)

/// throw if not success
#define QCE_THROW_CRITICAL(operation) \
    QCE_THROW_IMPL(operation, !QCE::is_success)

/// throw if fail
#define QCE_THROW(operation) \
    QCE_THROW_IMPL(operation, QCE::is_fail)

