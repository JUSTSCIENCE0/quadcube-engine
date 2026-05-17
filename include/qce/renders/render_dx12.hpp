// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/renders/render_base.hpp>
#include <qce/renders/render_dx12_helpers.hpp>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3d12.h>
#include <wrl.h>
#include <dxgi.h>
#include <directx/d3dx12.h>

namespace QCE {
    class RenderDX12 final :
            public RenderBase {
    public:
        /// ctor
        RenderDX12(Entities& entities, RenderConfig initial_config, HWND window);

        ErrorCode Draw() override;

    private:
        /// types
        template<typename T> using MsPtr = Microsoft::WRL::ComPtr<T>;
        struct RenderSceneGPU {
            MsPtr<ID3D12Resource> vertex_buffer = nullptr;
            MsPtr<ID3D12Resource> index_buffer  = nullptr;

            MsPtr<ID3D12Resource> vertex_buffer_uploader = nullptr;
            MsPtr<ID3D12Resource> index_buffer_uploader  = nullptr;

            std::unique_ptr<Dx12UploadBuffer<UnitConstants>> m_units_constant_buffers{};

            constexpr static DXGI_FORMAT INDEX_FORMAT = dx12_get_index_format();

            struct Texture {
                DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

                MsPtr<ID3D12Resource> buffer   = nullptr;
                MsPtr<ID3D12Resource> uploader = nullptr;
            };
            std::vector<Texture> textures{};

            void DisposeUploaders() {
                vertex_buffer_uploader = nullptr;
                index_buffer_uploader = nullptr;
            }
        };
        struct FrameResource {
            FrameResource(ID3D12Device* device, UINT units_count) {
                if (FAILED(device->CreateCommandAllocator(
                        D3D12_COMMAND_LIST_TYPE_DIRECT,
                        IID_PPV_ARGS(&m_cmd_alloc))))
                    throw ErrorCodeException(ErrorCode::E_DX12_CREATE_COMMAND_ALLOCATOR_FAILED);

                m_units_constant_buffers = std::make_unique<Dx12UploadBuffer<UnitConstants>>(
                    device, units_count, true);
            }
            FrameResource(const FrameResource&) = delete;
            FrameResource& operator=(const FrameResource&) = delete;
            FrameResource(FrameResource&&) = delete;
            FrameResource& operator=(FrameResource&&) = delete;

            MsPtr<ID3D12CommandAllocator> m_cmd_alloc = nullptr;

            // TODO: add pass constant buffer
            std::unique_ptr<Dx12UploadBuffer<UnitConstants>> m_units_constant_buffers{};

            uint64_t m_fence_value = 0;
        };

        /// consts
        static constexpr int SWAP_CHAIN_BUFFER_COUNT = 2;
        static constexpr D3D_DRIVER_TYPE D3D_DRIVER_TYPE = D3D_DRIVER_TYPE_HARDWARE;
        static constexpr DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
        static constexpr DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;

        /// methods
        // Initialization
        ErrorCode Init();
        ErrorCode CreateFrameResources();
        ErrorCode CreateCommandObjects();
        ErrorCode CreateSwapChain();
        ErrorCode CreateDescriptorHeaps();
        ErrorCode InitBuffersAndDescriptors();

        // Scene
        void DrawSceneEntities();
        ErrorCode UpdateGpuScene();
        ErrorCode CreateRootSignature();
        ErrorCode CreateCBVDescriptorHeap();
        ErrorCode CreateConstantBuffers();
        ErrorCode CreateInputLayout();
        ErrorCode CreatePSO();
        ErrorCode UpdateConstantBuffers();
        ErrorCode UploadMeshes();
        ErrorCode UploadTextures();

        // Utils
        ErrorCode FlushCommandQueue();
        ErrorCode NextFrameResource();
        ErrorCode LoadTexture(Texture2D& texture);
        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const {
            return m_dsv_heap->GetCPUDescriptorHandleForHeapStart();
        }
        ID3D12Resource* CurrentBackBuffer() const {
            return m_swap_chain_buffer[m_current_back_buffer].Get();
        }
        D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const {
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(
                m_rtv_heap->GetCPUDescriptorHandleForHeapStart(),
                m_current_back_buffer,
                m_rtv_descr_size);
        }
        D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const {
            assert(m_scene_gpu.vertex_buffer);
            assert(m_scene_cpu.vertex_buffer_size);

            D3D12_VERTEX_BUFFER_VIEW vbv;
            vbv.BufferLocation = m_scene_gpu.vertex_buffer->GetGPUVirtualAddress();
            vbv.StrideInBytes = m_scene_cpu.VERTEX_STRIDE;
            vbv.SizeInBytes = m_scene_cpu.vertex_buffer_size;
            return vbv;
        }
        D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const {
            assert(m_scene_gpu.index_buffer);
            assert(m_scene_cpu.index_buffer_size);

            D3D12_INDEX_BUFFER_VIEW ibv;
            ibv.BufferLocation = m_scene_gpu.index_buffer->GetGPUVirtualAddress();
            ibv.Format = m_scene_gpu.INDEX_FORMAT;
            ibv.SizeInBytes = m_scene_cpu.index_buffer_size;
            return ibv;
        }

        /// Attributes
        const HWND m_window;

        MsPtr<IDXGIFactory> m_dxgi_factory{};

        MsPtr<ID3D12Device> m_d3d_device{};
        MsPtr<ID3D12Fence>  m_fence{};

        MsPtr<ID3D12CommandQueue>        m_cmd_queue{};
        MsPtr<ID3D12CommandAllocator>    m_cmd_alloc{};
        MsPtr<ID3D12GraphicsCommandList> m_cmd_list{};

        std::vector<std::unique_ptr<FrameResource>> m_frame_resources{};
        FrameResource* m_current_frame_resource = nullptr;
        int m_current_frame_resource_index = 0;

        int m_current_back_buffer = 0;
        MsPtr<IDXGISwapChain> m_swap_chain{};
        MsPtr<ID3D12Resource> m_swap_chain_buffer[SWAP_CHAIN_BUFFER_COUNT];
        MsPtr<ID3D12Resource> m_depth_stencil_buffer{};

        MsPtr<ID3D12RootSignature> m_root_signature{};

        MsPtr<ID3D12DescriptorHeap> m_rtv_heap{};
        MsPtr<ID3D12DescriptorHeap> m_dsv_heap{};
        MsPtr<ID3D12DescriptorHeap> m_cbv_heap{};
        MsPtr<ID3D12DescriptorHeap> m_srv_heap{};

        D3D12_VIEWPORT m_screen_viewport{};
        D3D12_RECT     m_scissor_rect{};

        RenderSceneGPU m_scene_gpu{};
        std::vector<D3D12_INPUT_ELEMENT_DESC> m_input_layout;
        MsPtr<ID3D12PipelineState> m_PSO{};

        UINT m_rtv_descr_size = 0;
        UINT m_dsv_descr_size = 0;
        UINT m_cbv_srv_uav_descr_size = 0;

        uint64_t m_current_fence = 0;
    };
}

