// Copyright (c) 2025-2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/renders/render.hpp>
#include <qce/objects/scene.hpp>
#include <qce/ancillary/timer.hpp>
#include <qce/ecs/ecs.hpp>
#include <qce/ecs/systems_hub.hpp>
#include <qce/systems/camera_system.hpp>

#include <cu/string-utils.hpp>

namespace QCE {
    struct ApplicationConfig {
        GraphicsOutputConfig graphics_output{};
        RenderConfig render{};
    };

    template <SystemT... AdditionalSystems>
    class Application {
        friend GraphicsOutput;

    public:
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

        static Application& Get(
                const ApplicationConfig& initial_config) {
            static Application app {
                initial_config
            };
            return app;
        }
        static Application& Get() {
            // TODO: fix it
            return Get(ReadConfig());
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
        // for prototyping only, TODO: remove it
        Scene& CurrentScene() { return m_current_scene; }
        // for prototyping only
        HidEventsManager& HidManager() { return HidEventsManager::Get(); }

    private:
        explicit Application(
                const ApplicationConfig& initial_config)
        try :
            m_config(initial_config),
            m_graphics_output(
                m_config.graphics_output) {
            ResourceManager::Initialize(m_config.render.render_type);

#ifdef WIN32
            auto window = m_graphics_output.GetHwnd();
            auto app = nullptr;
#else
            auto window = nullptr;
            auto app = nullptr;
#endif
            m_render = QCE::GetRender(m_entities, m_config.render, window, app);
        }
        catch (QCE::ErrorCodeException qce_ex) {
            m_graphics_output.ShowMessage(
                L"QCE Error",
                CU::str_to_wstr_simple(qce_ex.what()));
            exit(qce_ex.code_value());
        }
        catch (std::exception ex) {
            m_graphics_output.ShowMessage(
                L"General Error",
                CU::str_to_wstr_simple(ex.what()));
            exit(-1);
        }
        catch (...) {
            m_graphics_output.ShowMessage(
                L"Error",
                L"Unknown Error");
            exit(-1);
        }

        static ApplicationConfig ReadConfig() {
            // TODO
            return {};
        }

        ErrorCode PreRun() {
            assert(m_render);
            QCE_CRITICAL(m_render->SetCurrentScene(&m_current_scene));
            return ErrorCode::SUCCESS;
        }

        ErrorCode StepForward() {
            FrameTime::Get().NextFrame();

            HidEventsManager::Get().Process();

            QCE_CRITICAL(m_systems.UpdateAll());

            assert(m_render);
            return m_render->Draw();
        }

        ApplicationConfig m_config{};
        Scene m_current_scene{
            m_config.render.render_type
        };
        // TODO: Level & World;

        GraphicsOutput m_graphics_output;
        // TODO: additional graphics outputs

    public:
        using Systems = SystemsHub<
            CameraSystem,
            AdditionalSystems...
        >;

        Entities m_entities{};
        Systems  m_systems{ m_entities };

    public: // for prototyping only, TODO: remove it
        std::shared_ptr<RenderBase> m_render{};
    };
}

