// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#ifdef WIN32
#  include <qce/renders/render_dx12.hpp>
#endif

namespace QCE {
    std::shared_ptr<RenderBase> GetRender(
            RenderConfig config, void* window, void* app) {
        static RenderType current_type = RenderType::E_RenderType_UNKNOWN;
        static std::shared_ptr<RenderBase> result = nullptr;

        if (current_type != config.render_type) {
            current_type = config.render_type;
            result.reset();

            switch (config.render_type) {
            case RenderType::E_RENDER_DIRECTX12:
#ifdef WIN32
                // unfortunatly we can't use std::make_shared for private constructor
                result = std::shared_ptr<RenderBase>(new RenderDX12(config, HWND(window)));
#else
                assert(!"Current platform doesn't support DirectX 12");
                current_type = RenderType::E_RenderType_UNKNOWN;
#endif
                break;
            default:
                current_type = RenderType::E_RenderType_UNKNOWN;
                break;
            }
        }

        return result;
    }
}
