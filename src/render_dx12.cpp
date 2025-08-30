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
        QCE_CRITICAL(InitBuffersAndDescriptors());

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
        sd.BufferDesc.Format = BACK_BUFFER_FORMAT;
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

    ErrorCode RenderDX12::InitBuffersAndDescriptors() {
        assert(m_d3d_device);
        assert(m_swap_chain);
        assert(m_cmd_alloc);
        assert(m_cmd_list);

        QCE_CRITICAL(FlushCommandQueue());

        auto hr = m_cmd_list->Reset(m_cmd_alloc.Get(), nullptr);
        if (FAILED(hr))
            return ErrorCode::E_DX12_RESET_COMMAND_LIST_FAILED;

        for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++) {
            m_swap_chain_buffer[i].Reset();
        }
        m_depth_stencil_buffer.Reset();

        hr = m_swap_chain->ResizeBuffers(
            SWAP_CHAIN_BUFFER_COUNT,
            m_config.width, m_config.height,
            BACK_BUFFER_FORMAT,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
        if (FAILED(hr))
            return ErrorCode::E_DX12_SWAP_CHAIN_RESIZE_BUFFERS_FAILED;

        m_current_back_buffer = 0;

        //CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
        //for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++) {
        //    ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
        //    md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
        //    rtvHeapHandle.Offset(1, mRtvDescriptorSize);
        //}

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::FlushCommandQueue() {
        assert(m_cmd_queue);
        assert(m_fence);

        m_current_fence++;

        auto hr = m_cmd_queue->Signal(m_fence.Get(), m_current_fence);
        if (FAILED(hr))
            return ErrorCode::E_DX12_QUEUE_ADD_SIGNAL_FAILED;

        if (m_fence->GetCompletedValue() < m_current_fence) {
            HANDLE eventHandle = CreateEventEx(nullptr, nullptr, NULL, EVENT_ALL_ACCESS);
            if (!eventHandle)
                return ErrorCode::E_DX12_CREATE_EVENT_FAILED;

            hr = m_fence->SetEventOnCompletion(m_current_fence, eventHandle);
            if (FAILED(hr))
                return ErrorCode::E_DX12_SET_EVENT_ON_COMPLETE_FAILED;

            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }

        return ErrorCode::SUCCESS;
    }
}
