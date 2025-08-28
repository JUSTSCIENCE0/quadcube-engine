// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/renders/render_dx12.hpp>

namespace QCE {
    RenderDX12::RenderDX12(RenderConfig initial_config, HWND window) :
            RenderBase(std::move(initial_config)),
            m_window(window) {
        assert(RenderType::E_RENDER_DIRECTX12 == initial_config.render_type);
        assert(m_window);

        QCE_THROW_CRITICAL(Init());
    }

    ErrorCode RenderDX12::Init() {
        auto hr = CreateDXGIFactory(IID_PPV_ARGS(&m_dxgi_factory));
        if (FAILED(hr)) {
            return ErrorCode::E_DX12_CREATE_DXGI_FAILED;
        }

        hr = D3D12CreateDevice(
            nullptr,
            D3D_FEATURE_LEVEL_12_2,
            IID_PPV_ARGS(&m_d3d_device));
        if (FAILED(hr)) {
            return ErrorCode::E_DX12_CREATE_DEVICE_FAILED;
        }

        hr = m_d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&m_fence));
        if (FAILED(hr)) {
            return ErrorCode::E_DX12_CREATE_FENCE_FAILED;
        }

        m_rtv_decr_size = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_dsv_decr_size = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        m_cbv_srv_uav_descr_size = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        QCE_CRITICAL(CreateCommandObjects());
        QCE_CRITICAL(CreateSwapChain());
        QCE_CRITICAL(CreateRtvAndDsvDescriptorHeaps());

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::CreateCommandObjects() {
        D3D12_COMMAND_QUEUE_DESC queue_descr = {};
        queue_descr.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queue_descr.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        auto hr = m_d3d_device->CreateCommandQueue(&queue_descr, IID_PPV_ARGS(&m_cmd_queue));
        if (FAILED(hr))
            return ErrorCode::E_DX12_CREATE_COMMAND_QUEUE_FAILED;

        hr = m_d3d_device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(m_cmd_alloc.GetAddressOf()));
        if (FAILED(hr))
            return ErrorCode::E_DX12_CREATE_COMMAND_ALLOCATOR_FAILED;

        hr = m_d3d_device->CreateCommandList(0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_cmd_alloc.Get(),
            nullptr,
            IID_PPV_ARGS(m_cmd_list.GetAddressOf()));
        if (FAILED(hr))
            return ErrorCode::E_DX12_CREATE_COMMAND_LIST_FAILED;

        hr = m_cmd_list->Close();
        if (FAILED(hr))
            return ErrorCode::E_DX12_CLOSE_COMMAND_LIST_FAILED;

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::CreateSwapChain() {
        m_swap_chain.Reset();

        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferDesc.Width = m_config.width;
        sd.BufferDesc.Height = m_config.height;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
        sd.OutputWindow = m_window;
        sd.Windowed = true;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        // Note: Swap chain uses queue to perform flush.
        auto hr = m_dxgi_factory->CreateSwapChain(
            m_cmd_queue.Get(),
            &sd,
            m_swap_chain.GetAddressOf());
        if (FAILED(hr))
            return ErrorCode::E_DX12_CREATE_SWAP_CHAIN_FAILED;

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::CreateRtvAndDsvDescriptorHeaps() {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
        rtvHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;
        auto hr = m_d3d_device->CreateDescriptorHeap(
            &rtvHeapDesc, IID_PPV_ARGS(m_rtv_heap.GetAddressOf()));
        if (FAILED(hr))
            return ErrorCode::E_DX12_CREATE_RTV_HEAP_FAILED;

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dsvHeapDesc.NodeMask = 0;
        hr = m_d3d_device->CreateDescriptorHeap(
            &dsvHeapDesc, IID_PPV_ARGS(m_dsv_heap.GetAddressOf()));
        if (FAILED(hr))
            return ErrorCode::E_DX12_CREATE_RTV_HEAP_FAILED;

        return ErrorCode::SUCCESS;
    }
}
