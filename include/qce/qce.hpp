// Copyright (c) 2025, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/render.hpp>

namespace QCE {
    struct ApplicationConfig {
        WindowConfig graphics_output{};
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

        static Application& Get(ApplicationConfig initial_config) {
            static Application app{ std::move(initial_config) };
            return app;
        }
        static Application& Get() {
            return Get(ReadConfig());
        }

        ErrorCode Run() {
            try {
                return m_graphics_output.MainLoop(this);
            }
            catch (QCE::ErrorCodeException qce_ex) {
                MessageBox(nullptr, L"QCE Exception", L"Error", 0);
                return qce_ex.code_value();
            }
            catch (std::exception ex) {
                MessageBox(nullptr, L"General Exception", L"Error", 0);
                return ErrorCode::E_ErrorCode_UNKNOWN;
            }
            catch (...) {
                MessageBox(nullptr, L"Unknown Exception", L"Error", 0);
                return ErrorCode::E_ErrorCode_UNKNOWN;
            }
        }

    private:
        explicit Application(ApplicationConfig initial_config)
        try :
            m_config(std::move(initial_config)),
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
            MessageBox(nullptr, L"QCE Exception", L"Error", 0);
            exit(qce_ex.code_value());
        }
        catch (std::exception ex) {
            MessageBox(nullptr, L"General Exception", L"Error", 0);
            exit(-1);
        }
        catch (...) {
            MessageBox(nullptr, L"Unknown Exception", L"Error", 0);
            exit(-1);
        }

        static ApplicationConfig ReadConfig() {
            // TODO
            return {};
        }

        ErrorCode StepForward() {
            assert(m_render);
            return m_render->Draw();
        }

        ApplicationConfig m_config{};
        GraphicsOutput m_graphics_output;
        // TODO: additional graphics outputs

        std::shared_ptr<RenderBase> m_render{};
    };
}

