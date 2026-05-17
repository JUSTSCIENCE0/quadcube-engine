// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#include <qce/ancillary/timer.hpp>
#include <qce/renders/render_dx12.hpp>

namespace QCE {
    RenderDX12::RenderDX12(Entities& entities, RenderConfig initial_config, HWND window) :
        RenderBase(entities, std::move(initial_config)),
            m_window(window) {
        assert(m_window);

        QCE_THROW_CRITICAL(Init());
    }

    ErrorCode RenderDX12::Init() {
//#if defined(DEBUG) || defined(_DEBUG) 
//        {
//            MsPtr<ID3D12Debug> debug_controller;
//            D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller));
//            debug_controller->EnableDebugLayer();
//        }
//#endif

        auto hr = CreateDXGIFactory(IID_PPV_ARGS(&m_dxgi_factory));
        if (FAILED(hr)) {
            return ErrorCode::E_DX12_CREATE_DXGI_FAILED;
        }

        hr = D3D12CreateDevice(
            nullptr,
            D3D_FEATURE_LEVEL_12_0,
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
        QCE_CRITICAL(CreateDescriptorHeaps());
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

    ErrorCode RenderDX12::CreateFrameResources() {
        m_frame_resources.clear();

        auto entities = m_entities.QueryEntities<
            MeshComponent,
            TransformComponents,
            TransformMatrix>();
        const auto units_count = UINT(entities.size());

        for (int i = 0; i < FRAME_RESOURCE_COUNT; i++) {
            try {
                m_frame_resources.emplace_back(
                    std::make_unique<FrameResource>(m_d3d_device.Get(), units_count));
            }
            catch (const ErrorCodeException& e) {
                return e.code_value();
            }
        }
        m_current_frame_resource_index = 0;
        m_current_frame_resource = m_frame_resources[m_current_frame_resource_index].get();
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

    ErrorCode RenderDX12::CreateDescriptorHeaps() {
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
            return ErrorCode::E_DX12_CREATE_DSV_HEAP_FAILED;

        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 3;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        hr = m_d3d_device->CreateDescriptorHeap(
            &srvHeapDesc, IID_PPV_ARGS(m_srv_heap.GetAddressOf()));
        if (FAILED(hr))
            return ErrorCode::E_DX12_CREATE_SRV_HEAP_FAILED;

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

    ErrorCode RenderDX12::NextFrameResource() {
        m_current_frame_resource_index = (m_current_frame_resource_index + 1) % FRAME_RESOURCE_COUNT;
        m_current_frame_resource = m_frame_resources[m_current_frame_resource_index].get();

        if (m_current_frame_resource->m_fence_value != 0 &&
            m_fence->GetCompletedValue() < m_current_frame_resource->m_fence_value) {
            HANDLE eventHandle = CreateEventEx(nullptr, nullptr, NULL, EVENT_ALL_ACCESS);
            if (!eventHandle)
                return ErrorCode::E_DX12_CREATE_EVENT_FAILED;

            if (FAILED(m_fence->SetEventOnCompletion(m_current_fence, eventHandle)))
                return ErrorCode::E_DX12_SET_EVENT_ON_COMPLETE_FAILED;

            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::UpdateConstantBuffers() {
        auto cameras_indeces = m_entities.QueryEntities<CameraView, CameraProj>();
        assert(cameras_indeces.size() == 1); // TODO: support more than one camera
        auto& camera_view = m_entities.GetComponent<CameraView>(*cameras_indeces.begin());
        if (!camera_view.actual_fru || !camera_view.actual_view)
            camera_recalc_view(camera_view);
        auto& camera_proj = m_entities.GetComponent<CameraProj>(*cameras_indeces.begin());
        if (!camera_proj.actual_proj)
            camera_recalc_proj(camera_proj);

        auto v = matrix_init(camera_view.view.arr);
        auto p = matrix_init(camera_proj.proj.arr);
        auto vp = matrix_mul(v, p);

        PassConstants pass_constants{};
        matrix_copy(v, pass_constants.view_matrix);
        matrix_copy(matrix_inverse(v, matrix_determinant(v)),
            pass_constants.view_matrix_inv);
        matrix_copy(p, pass_constants.proj_matrix);
        matrix_copy(matrix_inverse(p, matrix_determinant(p)),
            pass_constants.proj_matrix_inv);
        matrix_copy(vp, pass_constants.view_proj_matrix);
        matrix_copy(matrix_inverse(vp, matrix_determinant(vp)),
            pass_constants.view_proj_matrix_inv);
        std::memcpy(pass_constants.eye_position,
                    camera_view.position.arr,
                    sizeof(pass_constants.eye_position));
        pass_constants.render_target_size[0] = static_cast<float>(m_config.width);
        pass_constants.render_target_size[1] = static_cast<float>(m_config.height);
        pass_constants.render_target_size_inv[0] = 1.0f / pass_constants.render_target_size[0];
        pass_constants.render_target_size_inv[1] = 1.0f / pass_constants.render_target_size[1];
        pass_constants.near_z = camera_proj.znear;
        pass_constants.far_z  = camera_proj.zfar;
        pass_constants.delta_time = static_cast<float>(FrameTime::Get().Elapsed());

        //auto current_pass_constant_buffer = m_current_frame_resource->m_pass_constant_buffer.get();
        //current_pass_constant_buffer->CopyData(0, pass_constants);

        auto entities = m_entities.QueryEntities<
            MeshComponent,
            TransformComponents,
            TransformMatrix>();

        UINT index = 0;
        for (const auto& entity_id : entities) {
            auto& world = m_entities.GetComponent<TransformMatrix>(entity_id);
            if (!world.actual) {
                auto& transform_comp = m_entities.GetComponent<TransformComponents>(entity_id);
                calculate_transform_matrix(transform_comp, world);
            }

            auto w = matrix_init(world.data.arr);
            auto wvp = matrix_mul(w, vp);
            wvp = matrix_transpose(wvp);

            UnitConstants transform{};
            matrix_copy(wvp, transform.world_matrix);

            m_scene_gpu.m_units_constant_buffers->CopyData(index, transform);
            index++;
        }

        return ErrorCode::SUCCESS;
    }

    void RenderDX12::DrawSceneEntities() {
        auto vbv = GetVertexBufferView();
        auto ibv = GetIndexBufferView();
        m_cmd_list->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_cmd_list->IASetVertexBuffers(0, 1, &vbv);
        m_cmd_list->IASetIndexBuffer(&ibv);

        auto entities = m_entities.QueryEntities<
            MeshComponent,
            TransformComponents,
            TransformMatrix>();

        UINT entity_index = 0;
        for (const auto& entity_id : entities) {
            const auto& mesh_comp = m_entities.GetComponent<MeshComponent>(entity_id);
            const auto& unit_descr = m_scene_cpu.units[mesh_comp.render_unit_index];

            auto cbv_handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_cbv_heap->GetGPUDescriptorHandleForHeapStart());
            cbv_handle.Offset(entity_index, m_cbv_srv_uav_descr_size);
            m_cmd_list->SetGraphicsRootDescriptorTable(0, cbv_handle);

            m_cmd_list->DrawIndexedInstanced(
                unit_descr.indeces_count,
                1,
                unit_descr.index_offset,
                unit_descr.vertex_offset,
                0
            );

            entity_index++;
        }
    }

    ErrorCode RenderDX12::Draw() {
        QCE_CRITICAL(UpdateConstantBuffers());

        auto hr = m_cmd_alloc->Reset();
        if (FAILED(hr)) {
            return ErrorCode::E_DX12_RESET_COMMAND_ALLOCATOR_FAILED;
        }

        hr = m_cmd_list->Reset(m_cmd_alloc.Get(), m_PSO.Get());
        if (FAILED(hr)) {
            return ErrorCode::E_DX12_RESET_COMMAND_LIST_FAILED;
        }

        auto barrier_pt = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_cmd_list->ResourceBarrier(1, &barrier_pt);

        m_cmd_list->RSSetViewports(1, &m_screen_viewport);
        m_cmd_list->RSSetScissorRects(1, &m_scissor_rect);

        auto current_bbv = CurrentBackBufferView();
        m_cmd_list->ClearRenderTargetView(
            current_bbv, DirectX::Colors::LightSteelBlue /* TODO: configurable */, 0, nullptr);
        m_cmd_list->ClearDepthStencilView(
            DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        auto dsv = DepthStencilView();
        m_cmd_list->OMSetRenderTargets(1, &current_bbv, true, &dsv);

        ID3D12DescriptorHeap* descr_heaps[] = { m_cbv_heap.Get() };
        m_cmd_list->SetDescriptorHeaps(_countof(descr_heaps), descr_heaps);

        m_cmd_list->SetGraphicsRootSignature(m_root_signature.Get());

        DrawSceneEntities();

        auto barrier_tp = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_cmd_list->ResourceBarrier(1, &barrier_tp);

        hr = m_cmd_list->Close();
        if (FAILED(hr)) {
            return ErrorCode::E_DX12_CLOSE_COMMAND_LIST_FAILED;
        }

        ID3D12CommandList* cmds_lists[] = { m_cmd_list.Get() };
        m_cmd_queue->ExecuteCommandLists(_countof(cmds_lists), cmds_lists);

        hr = m_swap_chain->Present(0, 0);
        if (FAILED(hr)) {
            return ErrorCode::E_DX12_PRESENT_SWAP_CHAIN_FAILED;
        }
        m_current_back_buffer = (m_current_back_buffer + 1) % SWAP_CHAIN_BUFFER_COUNT;

        return FlushCommandQueue();
    }

    ErrorCode RenderDX12::UpdateGpuScene() { 
        auto hr = m_cmd_list->Reset(m_cmd_alloc.Get(), nullptr);
        if (FAILED(hr))
            return ErrorCode::E_DX12_RESET_COMMAND_LIST_FAILED;

        QCE_CRITICAL(CreateCBVDescriptorHeap());
        QCE_CRITICAL(CreateConstantBuffers());
        QCE_CRITICAL(CreateRootSignature());
        QCE_CRITICAL(CreateInputLayout());

        QCE_CRITICAL(UploadMeshes());
        QCE_CRITICAL(UploadTextures());

        QCE_CRITICAL(CreatePSO());

        hr = m_cmd_list->Close();
        if (FAILED(hr))
            return ErrorCode::E_DX12_CLOSE_COMMAND_LIST_FAILED;

        ID3D12CommandList* cmds_lists[] = { m_cmd_list.Get() };
        m_cmd_queue->ExecuteCommandLists(_countof(cmds_lists), cmds_lists);
        return FlushCommandQueue();
    }

    ErrorCode RenderDX12::UploadMeshes() {
        m_scene_gpu.DisposeUploaders();

        QCE_CRITICAL(dx12_create_default_buffer(
            m_d3d_device.Get(),
            m_cmd_list.Get(),
            m_scene_cpu.vertex_buffer.data(),
            m_scene_cpu.vertex_buffer_size,
            m_scene_gpu.vertex_buffer,
            m_scene_gpu.vertex_buffer_uploader));

        QCE_CRITICAL(dx12_create_default_buffer(
            m_d3d_device.Get(),
            m_cmd_list.Get(),
            m_scene_cpu.index_buffer.data(),
            m_scene_cpu.index_buffer_size,
            m_scene_gpu.index_buffer,
            m_scene_gpu.index_buffer_uploader));

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::UploadTextures() {
        auto entities = m_entities.QueryEntities<
            MaterialComponent>();

        // reset
        m_scene_gpu.textures.clear();
        for (auto& entity : entities) {
            auto& material = m_entities.GetComponent<MaterialComponent>(entity);

            if (material.cpu_albedo_index.has_value()) {
                auto& albedo = ResourceManager::Get().Read<Texture2D>(material.cpu_albedo_index.value());
                albedo.gpu_texture_index.reset();
            }
        }

        for (auto& entity : entities) {
            auto& material = m_entities.GetComponent<MaterialComponent>(entity);

            if (material.cpu_albedo_index.has_value()) {
                auto& albedo = ResourceManager::Get().Read<Texture2D>(material.cpu_albedo_index.value());
                if (!albedo.gpu_texture_index.has_value()) {
                    auto index = m_scene_gpu.textures.size();
                    QCE_CRITICAL(LoadTexture(albedo));
                    albedo.gpu_texture_index = index;
                }

                material.gpu_albedo_index = albedo.gpu_texture_index;
            }
        }

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::CreateRootSignature() {
        CD3DX12_ROOT_PARAMETER slotRootParameter{};

        CD3DX12_DESCRIPTOR_RANGE cbvTable{};
        cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
        slotRootParameter.InitAsDescriptorTable(1, &cbvTable);

        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, &slotRootParameter, 0, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        MsPtr<ID3DBlob> serializedRootSig = nullptr;
        MsPtr<ID3DBlob> errorBlob = nullptr;
        auto hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
            serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

        if (errorBlob) {
            // use log system
            std::cout << reinterpret_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
        }
        if (FAILED(hr))
            return ErrorCode::E_DX12_SERIALIZE_ROOT_SIGNATURE_FAILED;

        if (FAILED(m_d3d_device->CreateRootSignature(
                    0,
                    serializedRootSig->GetBufferPointer(),
                    serializedRootSig->GetBufferSize(),
                    IID_PPV_ARGS(&m_root_signature))))
            return ErrorCode::E_DX12_CREATE_ROOT_SIGNATURE_FAILED;

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::CreateCBVDescriptorHeap() {
        auto entities = m_entities.QueryEntities<
            MeshComponent,
            TransformComponents,
            TransformMatrix>();
        const auto units_count = UINT(entities.size());

        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
        cbvHeapDesc.NumDescriptors = units_count + 1;
        cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.NodeMask = 0;

        auto hr = m_d3d_device->CreateDescriptorHeap(
            &cbvHeapDesc, IID_PPV_ARGS(m_cbv_heap.GetAddressOf()));
        if (FAILED(hr))
            return ErrorCode::E_DX12_CREATE_CBV_HEAP_FAILED;

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::CreateConstantBuffers() {
        auto entities = m_entities.QueryEntities<
            MeshComponent,
            TransformComponents,
            TransformMatrix>();
        const auto units_count = UINT(entities.size());

        auto& cb = m_scene_gpu.m_units_constant_buffers;
        cb = std::make_unique<Dx12UploadBuffer<UnitConstants>>(
                m_d3d_device.Get(), units_count, true);

        const auto cb_unit_size = cb->m_element_size;
        D3D12_GPU_VIRTUAL_ADDRESS cb_address = cb->Resource()->GetGPUVirtualAddress();

        for (UINT index = 0; index < units_count; index++) {
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbv_heap->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(index, m_cbv_srv_uav_descr_size);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_descr;
            cbv_descr.BufferLocation = cb_address;
            cbv_descr.SizeInBytes = cb_unit_size;

            m_d3d_device->CreateConstantBufferView(&cbv_descr, handle);
            cb_address += cb_unit_size;
        }

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::CreateInputLayout() {
        // TODO: configurable IL from Shader's props
        m_input_layout = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::CreatePSO() {
        if (!ResourceManager::Get().Exists<Shader>(
                m_shader_indeces[ShaderType::E_VERTEX_SHADER]))
            return ErrorCode::E_ENG_SCENE_VS_NOT_SELECTED;
        if (!ResourceManager::Get().Exists<Shader>(
                m_shader_indeces[ShaderType::E_PIXEL_SHADER]))
            return ErrorCode::E_ENG_SCENE_PS_NOT_SELECTED;

        auto& vs = ResourceManager::Get().Read<Shader>(
            m_shader_indeces[ShaderType::E_VERTEX_SHADER]);
        auto& ps = ResourceManager::Get().Read<Shader>(
            m_shader_indeces[ShaderType::E_PIXEL_SHADER]);

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_descr{};
        ZeroMemory(&pso_descr, sizeof(pso_descr));
        pso_descr.InputLayout = { m_input_layout.data(), UINT(m_input_layout.size()) };
        pso_descr.pRootSignature = m_root_signature.Get();
        pso_descr.VS =
        {
            vs.bytecode_cache.data(),
            vs.bytecode_cache.size()
        };
        pso_descr.PS =
        {
            ps.bytecode_cache.data(),
            ps.bytecode_cache.size()
        };
        pso_descr.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

        // TODO: configurable SOLID/WIREFRAME in same scene
        // pso_descr.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

        pso_descr.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        pso_descr.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        pso_descr.SampleMask = UINT_MAX;
        pso_descr.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pso_descr.NumRenderTargets = 1;
        pso_descr.RTVFormats[0] = BACK_BUFFER_FORMAT;
        // TODO: MSAA
        pso_descr.SampleDesc.Count = 1;
        pso_descr.SampleDesc.Quality = 0;
        pso_descr.DSVFormat = DEPTH_STENCIL_FORMAT;

        if (FAILED(m_d3d_device->CreateGraphicsPipelineState(&pso_descr, IID_PPV_ARGS(&m_PSO))))
            return ErrorCode::E_DX12_CREATE_PSO_FAILED;

        return ErrorCode::SUCCESS;
    }

    ErrorCode RenderDX12::LoadTexture(Texture2D& texture) {
        RenderSceneGPU::Texture gpu_texture;
        gpu_texture.format = dx12_get_texture_format(texture.format);
        if (DXGI_FORMAT_UNKNOWN == gpu_texture.format)
            return ErrorCode::E_DX12_UNSUPPORTED_TEXTURE_FORMAT;

        if (texture.base_width  > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION ||
            texture.base_height > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION)
            return ErrorCode::E_DX12_UNSUPPORTED_TEXTURE_FORMAT;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width  = texture.base_width;
        desc.Height = texture.base_height;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = static_cast<UINT16>(texture.mip_levels.size());
        desc.Format = gpu_texture.format;
        desc.SampleDesc.Count   = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;

        auto heap_props1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        if (FAILED(m_d3d_device->CreateCommittedResource(
            &heap_props1,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(gpu_texture.buffer.GetAddressOf())))) {
            return ErrorCode::E_DX12_CREATE_DEFAULT_BUFFER_RESOURCE_FAILED;
        }

        std::vector<D3D12_SUBRESOURCE_DATA> subresources;
        subresources.reserve(texture.mip_levels.size());
        for (const auto& mip : texture.mip_levels) {
            auto& sr = subresources.emplace_back();
            sr.pData = mip.data;
            sr.RowPitch = mip.row_pitch;
            sr.SlicePitch = mip.slice_pitch;
        }

        UINT64 uploadSize = GetRequiredIntermediateSize(
            gpu_texture.buffer.Get(), 0, static_cast<UINT>(subresources.size()));

        auto heap_props2 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resource_desc2 = CD3DX12_RESOURCE_DESC::Buffer(uploadSize);
        if FAILED(m_d3d_device->CreateCommittedResource(
            &heap_props2,
            D3D12_HEAP_FLAG_NONE,
            &resource_desc2,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&gpu_texture.uploader))) {
            return ErrorCode::E_DX12_CREATE_UPLOAD_BUFFER_RESOURCE_FAILED;
        }

        UpdateSubresources(
            m_cmd_list.Get(),
            gpu_texture.buffer.Get(),
            gpu_texture.uploader.Get(),
            0,
            0,
            static_cast<UINT>(subresources.size()),
            subresources.data());

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            gpu_texture.buffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        m_cmd_list->ResourceBarrier(1, &barrier);

        CD3DX12_CPU_DESCRIPTOR_HANDLE descr(m_srv_heap->GetCPUDescriptorHandleForHeapStart());
        auto texture_resource = gpu_texture.buffer;

        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.Format = texture_resource->GetDesc().Format;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MostDetailedMip = 0;
        srv_desc.Texture2D.MipLevels = texture_resource->GetDesc().MipLevels;
        srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;

        m_d3d_device->CreateShaderResourceView(texture_resource.Get(), &srv_desc, descr);

        m_scene_gpu.textures.emplace_back(std::move(gpu_texture));
        return ErrorCode::SUCCESS;
    }
}
