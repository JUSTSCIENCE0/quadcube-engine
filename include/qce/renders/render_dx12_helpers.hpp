// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/error_codes.hpp>

#include <d3d12.h>
#include <wrl.h>
#include <directx/d3dx12.h>
#include <D3Dcompiler.h>
#include <DirectXColors.h>

#include <iostream>
#include <fstream>

namespace QCE {
    constexpr static DXGI_FORMAT dx12_get_index_format() {
        switch (QCE_INDEX_WIDTH) {
        case IndexWidth::INDEX_WIDTH_1:
            return DXGI_FORMAT_R8_UINT;
        case IndexWidth::INDEX_WIDTH_2:
            return DXGI_FORMAT_R16_UINT;
        case IndexWidth::INDEX_WIDTH_4:
            return DXGI_FORMAT_R32_UINT;
        default:
            static_assert(
                QCE_INDEX_WIDTH == IndexWidth::INDEX_WIDTH_1 ||
                QCE_INDEX_WIDTH == IndexWidth::INDEX_WIDTH_2 ||
                QCE_INDEX_WIDTH == IndexWidth::INDEX_WIDTH_4,
                "Unknown index format");
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    static ErrorCode dx12_create_default_buffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmd_list,
        const void* init_data,
        size_t size,
        Microsoft::WRL::ComPtr<ID3D12Resource>& result,
        Microsoft::WRL::ComPtr<ID3D12Resource>& upload_buffer) {
        using namespace Microsoft::WRL;

        auto heap_props1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto resource_desc1 = CD3DX12_RESOURCE_DESC::Buffer(size);
        if (FAILED(device->CreateCommittedResource(
            &heap_props1,
            D3D12_HEAP_FLAG_NONE,
            &resource_desc1,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(result.GetAddressOf())))) {
            return ErrorCode::E_DX12_CREATE_DEFAULT_BUFFER_RESOURCE_FAILED;
        }

        auto heap_props2 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resource_desc2 = CD3DX12_RESOURCE_DESC::Buffer(size);
        if (FAILED(device->CreateCommittedResource(
            &heap_props2,
            D3D12_HEAP_FLAG_NONE,
            &resource_desc2,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(upload_buffer.GetAddressOf())))) {
            return ErrorCode::E_DX12_CREATE_UPLOAD_BUFFER_RESOURCE_FAILED;
        }

        D3D12_SUBRESOURCE_DATA sub_resource_data = {};
        sub_resource_data.pData = init_data;
        sub_resource_data.RowPitch = size;
        sub_resource_data.SlicePitch = sub_resource_data.RowPitch;

        auto trans_descr1 = CD3DX12_RESOURCE_BARRIER::Transition(result.Get(),
            D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        cmd_list->ResourceBarrier(1, &trans_descr1);
        UpdateSubresources<1>(cmd_list, result.Get(), upload_buffer.Get(), 0, 0, 1, &sub_resource_data);
        auto trans_descr2 = CD3DX12_RESOURCE_BARRIER::Transition(result.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
        cmd_list->ResourceBarrier(1, &trans_descr2);

        return ErrorCode::SUCCESS;
    }

    static inline UINT dx12_calculate_constant_buffer_size(UINT size) {
        assert(size > 0);

        constexpr UINT CONSTANT_BUFFER_SIZE_ALIGNMENT = 256;

        if (0 != (size % CONSTANT_BUFFER_SIZE_ALIGNMENT))
            return ((size / CONSTANT_BUFFER_SIZE_ALIGNMENT) + 1) * CONSTANT_BUFFER_SIZE_ALIGNMENT;
        else
            return size;
    }

    template<typename T>
    class Dx12UploadBuffer {
    public:
        Dx12UploadBuffer(
                ID3D12Device* device,
                UINT elements_count,
                bool is_constant_buffer = false) :
            m_is_constant_buffer(is_constant_buffer),
            m_elements_count(elements_count),
            m_element_size(calculate_element_size(is_constant_buffer)) {

            auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(m_element_size * elements_count);
            if FAILED(device->CreateCommittedResource(
                    &heap_props,
                    D3D12_HEAP_FLAG_NONE,
                    &resource_desc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&m_upload_buffer))) {
                throw ErrorCodeException(ErrorCode::E_DX12_CREATE_UPLOAD_BUFFER_RESOURCE_FAILED);
            }

            if FAILED(m_upload_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped_data))) {
                throw ErrorCodeException(ErrorCode::E_DX12_MAP_UPLOAD_BUFFER_FAILED);
            }
        }

        Dx12UploadBuffer(const Dx12UploadBuffer&) = delete;
        Dx12UploadBuffer(Dx12UploadBuffer&&) = delete;
        Dx12UploadBuffer& operator=(const Dx12UploadBuffer&) = delete;
        Dx12UploadBuffer& operator=(Dx12UploadBuffer&&) = delete;
        ~Dx12UploadBuffer() {
            if (m_upload_buffer != nullptr)
                m_upload_buffer->Unmap(0, nullptr);
            m_mapped_data = nullptr;
        }

        ID3D12Resource* Resource() const {
            return m_upload_buffer.Get();
        }

        void CopyData(int elementIndex, const T& data) {
            memcpy(&m_mapped_data[elementIndex * m_element_size], &data, sizeof(T));
        }

        const bool m_is_constant_buffer;
        const UINT m_elements_count;
        const UINT m_element_size;

    private:
        static UINT calculate_element_size(bool is_constant_buffer) {
            if (is_constant_buffer)
                return dx12_calculate_constant_buffer_size(sizeof(T));

            return sizeof(T);
        }

        Microsoft::WRL::ComPtr<ID3D12Resource> m_upload_buffer;
        BYTE* m_mapped_data = nullptr;
    };
}

