// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/render.hpp>
#include <qce/objects/scene.hpp>

#include <cu/string-utils.hpp>

namespace QCE {
    struct ApplicationConfig {
        GraphicsOutputConfig graphics_output{};
        RenderConfig render{};
    };

    // TODO:  additional logic, graphics, etc
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
        ResourceManager& Resources() { return m_rm; }
        // for prototyping only
        Scene& CurrentScene() { return m_current_scene; }

    private:
        explicit Application(
                const ApplicationConfig& initial_config)
        try :
            m_config(initial_config),
            m_graphics_output(m_config.graphics_output) {
#ifdef WIN32
            auto window = m_graphics_output.GetHwnd();
            auto app = nullptr;
#else
            auto window = nullptr;
            auto app = nullptr;
#endif
            m_render = QCE::GetRender(m_config.render, window, app);
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
            assert(m_render);
            return m_render->Draw();
        }

        ApplicationConfig m_config{};
        GraphicsOutput m_graphics_output;
        // TODO: additional graphics outputs

        std::shared_ptr<RenderBase> m_render{};

        ResourceManager m_rm {
            m_config.render.render_type,
        };
        Scene m_current_scene{m_rm};
        // TODO: Level & World;
    };
}

