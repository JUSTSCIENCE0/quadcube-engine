// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/renders/render_dx12.hpp>

#include <directx/d3dx12.h>

namespace QCE {
    RenderDX12::RenderDX12(RenderConfig initial_config, HWND window) :
        RenderBase(std::in_place_type<RenderDX12>, std::move(initial_config)),
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

        m_rtv_descr_size = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_dsv_descr_size = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
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

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());
        for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++) {
            hr = m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&m_swap_chain_buffer[i]));
            if (FAILED(hr))
                return ErrorCode::E_DX12_SWAP_CHAIN_GET_BUFFER_FAILED;

            m_d3d_device->CreateRenderTargetView(m_swap_chain_buffer[i].Get(), nullptr, rtv_heap_handle);
            rtv_heap_handle.Offset(1, m_rtv_descr_size);
        }

        D3D12_RESOURCE_DESC depth_stencil_descr {
            .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            .Alignment = 0,
            .Width = UINT64(m_config.width),
            .Height = UINT64(m_config.height),
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = DXGI_FORMAT_R24G8_TYPELESS,
            .SampleDesc { // TODO: MSAA
                .Count = 1,
                .Quality = 0
            },
            .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
            .Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
        };
        D3D12_CLEAR_VALUE opt_clear {
            .Format = DEPTH_STENCIL_FORMAT,
            .DepthStencil {
                .Depth = 1.0f,
                .Stencil = 0
            }
        };
        CD3DX12_HEAP_PROPERTIES heap_props{ D3D12_HEAP_TYPE_DEFAULT };
        hr = m_d3d_device->CreateCommittedResource(
            &heap_props,
            D3D12_HEAP_FLAG_NONE,
            &depth_stencil_descr,
            D3D12_RESOURCE_STATE_COMMON,
            &opt_clear,
            IID_PPV_ARGS(m_depth_stencil_buffer.GetAddressOf())
        );
        if (FAILED(hr))
            return ErrorCode::E_DX12_CREATE_DS_BUFFER_FAILED;

        D3D12_DEPTH_STENCIL_VIEW_DESC dsv_descr {
            .Format = DEPTH_STENCIL_FORMAT,
            .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
            .Flags = D3D12_DSV_FLAG_NONE,
            .Texture2D {.MipSlice = 0 }
        };
        m_d3d_device->CreateDepthStencilView(m_depth_stencil_buffer.Get(), &dsv_descr, DepthStencilView());

        auto rb_transiton = CD3DX12_RESOURCE_BARRIER::Transition(m_depth_stencil_buffer.Get(),
            D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        m_cmd_list->ResourceBarrier(1, &rb_transiton);

        hr = m_cmd_list->Close();
        if (FAILED(hr))
            return E_DX12_CLOSE_COMMAND_LIST_FAILED;

        ID3D12CommandList* cmdsLists[] = { m_cmd_list.Get() };
        m_cmd_queue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

        QCE_CRITICAL(FlushCommandQueue());

        m_screen_viewport = {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = FLOAT(m_config.width),
            .Height = FLOAT(m_config.height),
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f
        };
        m_scissor_rect = {
            .left = 0,
            .top = 0,
            .right = m_config.width,
            .bottom = m_config.height
        };

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

    ErrorCode RenderDX12::Draw() {
        std::cout << "RenderDX12::Draw" << std::endl;
        return ErrorCode::SUCCESS;
    }
}
