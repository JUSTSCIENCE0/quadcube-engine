// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/ancillary/timer.hpp>

#include <qce/ecs/ecs.hpp>
#include <qce/ecs/systems_hub.hpp>
#include <qce/systems/movement_system.hpp>
#include <qce/systems/camera_system.hpp>
#include <qce/systems/hid_system.hpp>
#include <qce/systems/render_system.hpp>

#include <cu/string-utils.hpp>

namespace QCE {
    template <typename... AdditionalConfigs>
    struct ApplicationConfig {
        GraphicsOutputConfig graphics_output{};

        std::tuple<
            RenderConfig,
            CameraConfig,
            MovementConfig,
            HidConfig,
            AdditionalConfigs...
        > systems_configs{};
    };
    //struct ApplicationConfig {
    //    GraphicsOutputConfig graphics_output{};
    //    RenderConfig render{};
    //};

    template <SystemT... AdditionalSystems>
    class Application {
        friend GraphicsOutput;

    public:
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

        static Application& Get() {
            static Application app{};
            return app;
        }

        template <typename... AdditionalConfigs>
        ErrorCode Setup(ApplicationConfig<AdditionalConfigs...>& config) {
            QCE_CRITICAL(m_graphics_output.Setup());

            auto& render_system = m_systems.Get<RenderSystem>();
#ifdef WIN32
            render_system.SetWindow(m_graphics_output.GetHwnd());
#else
            render_system.SetWindow(nullptr);
#endif

            auto& render_config = std::get<RenderConfig>(config.systems_configs);
            ResourceManager::Initialize(render_config.render_type);

            QCE_CRITICAL(m_systems.Setup(config.systems_configs));
            return ErrorCode::SUCCESS;
        }

        ErrorCode Run() {
            try {
                QCE_CRITICAL(PreRun());
                return m_graphics_output.MainLoop(this);
            }
            catch (QCE::ErrorCodeException qce_ex) {
                m_graphics_output.ShowMessage(
                    L"QCE Error",
                    CU::str_to_wstr_simple(qce_ex.what()));
                return qce_ex.code_value();
            }
            catch (std::exception ex) {
                m_graphics_output.ShowMessage(
                    L"General Error",
                    CU::str_to_wstr_simple(ex.what()));
                return ErrorCode::E_ErrorCode_UNKNOWN;
            }
            catch (...) {
                m_graphics_output.ShowMessage(
                    L"Error",
                    L"Unknown Error");
                return ErrorCode::E_ErrorCode_UNKNOWN;
            }
        }

        // for prototyping only
        ResourceManager& Resources() { return ResourceManager::Get(); }

    public:
        using Systems = SystemsHub<
            HidSystem,
            MovementSystem,
            CameraSystem,
            RenderSystem,
            AdditionalSystems...
        >;

        Entities m_entities{};
        Systems  m_systems{ m_entities };

    private:
        explicit Application() :
            m_graphics_output(m_systems.Get<QCE::HidSystem>()) {}

        ErrorCode PreRun() {
            auto& render = m_systems.Get<RenderSystem>();
            QCE_CRITICAL(render.UpdateScene());
            return ErrorCode::SUCCESS;
        }

        ErrorCode StepForward() {
            FrameTime::Get().NextFrame();

            QCE_CRITICAL(m_systems.UpdateAll());

            return ErrorCode::SUCCESS;
        }

        GraphicsOutput m_graphics_output;
        // TODO: additional graphics outputs
    };
}

