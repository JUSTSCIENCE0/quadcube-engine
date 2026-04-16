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
#include <qce/ancillary/directories.hpp>

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
        ErrorCode Setup() {
            QCE_CRITICAL(m_graphics_output.Setup());

            auto& render_system = m_systems.Get<RenderSystem>();
#ifdef WIN32
            render_system.SetWindow(m_graphics_output.GetHwnd());
#else
            render_system.SetWindow(nullptr);
#endif

            const auto CONFIGS_DIR = QCE::get_configs_directory();
            const auto common_config_json_file = CONFIGS_DIR / "common_config.json";

            std::string error_descr = "";
            CommonConfig common_config{};
            auto parse_result = macrojson::json_file_to_object(common_config_json_file, common_config, error_descr);
            if (macrojson::E_MJSON_OK != parse_result) {
                // TODO: use log system
                std::cout << error_descr << std::endl;
                return ErrorCode::E_ENG_BAD_CONFIG_FILE;
            }
            ResourceManager::Initialize(common_config.render_type);

            return m_systems.Setup<
                RenderConfig,
                CameraConfig,
                MovementConfig,
                HidConfig,
                AdditionalConfigs...
            >();
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

