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
        explicit RenderDX12(RenderConfig initial_config, HWND window);

        ErrorCode Init();
        ErrorCode CreateCommandObjects();
        ErrorCode CreateSwapChain();
        ErrorCode CreateRtvAndDsvDescriptorHeaps();

        const HWND m_window;

        Microsoft::WRL::ComPtr<IDXGIFactory> m_dxgi_factory{};

        Microsoft::WRL::ComPtr<ID3D12Device> m_d3d_device{};
        Microsoft::WRL::ComPtr<ID3D12Fence>  m_fence{};

        Microsoft::WRL::ComPtr<ID3D12CommandQueue>        m_cmd_queue{};
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    m_cmd_alloc{};
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_cmd_list{};

        static constexpr int SWAP_CHAIN_BUFFER_COUNT = 2;
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap_chain{};

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtv_heap{};
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsv_heap{};

        UINT m_rtv_decr_size = 0;
        UINT m_dsv_decr_size = 0;
        UINT m_cbv_srv_uav_descr_size = 0;
    };
}

