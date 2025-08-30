// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/renders/render_base.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3d12.h>
#include <wrl.h>
#include <dxgi.h>

namespace QCE {
    class RenderDX12 final :
            public RenderBase {
    public:
        friend std::shared_ptr<RenderBase> GetRender(RenderConfig config, void* window, void* app);

    private:
        /// ctor
        explicit RenderDX12(RenderConfig initial_config, HWND window);

        /// types
        template<typename T> using MsPtr = Microsoft::WRL::ComPtr<T>;

        /// consts
        static constexpr int SWAP_CHAIN_BUFFER_COUNT = 2;
        static constexpr D3D_DRIVER_TYPE D3D_DRIVER_TYPE = D3D_DRIVER_TYPE_HARDWARE;
        static constexpr DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
        static constexpr DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;

        /// methods
        // Initialization
        ErrorCode Init();
        ErrorCode CreateCommandObjects();
        ErrorCode CreateSwapChain();
        ErrorCode CreateRtvAndDsvDescriptorHeaps();
        ErrorCode InitBuffersAndDescriptors();

        // Utils
        ErrorCode FlushCommandQueue();

        /// Attributes
        const HWND m_window;

        MsPtr<IDXGIFactory> m_dxgi_factory{};

        MsPtr<ID3D12Device> m_d3d_device{};
        MsPtr<ID3D12Fence>  m_fence{};

        MsPtr<ID3D12CommandQueue>        m_cmd_queue{};
        MsPtr<ID3D12CommandAllocator>    m_cmd_alloc{};
        MsPtr<ID3D12GraphicsCommandList> m_cmd_list{};

        int m_current_back_buffer = 0;
        MsPtr<IDXGISwapChain> m_swap_chain{};
        MsPtr<ID3D12Resource> m_swap_chain_buffer[SWAP_CHAIN_BUFFER_COUNT];
        MsPtr<ID3D12Resource> m_depth_stencil_buffer;

        MsPtr<ID3D12DescriptorHeap> m_rtv_heap{};
        MsPtr<ID3D12DescriptorHeap> m_dsv_heap{};

        UINT m_rtv_decr_size = 0;
        UINT m_dsv_decr_size = 0;
        UINT m_cbv_srv_uav_descr_size = 0;

        uint64_t m_current_fence = 0;
    };
}

