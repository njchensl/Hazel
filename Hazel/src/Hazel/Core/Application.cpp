#include "hzpch.h"
#include "Application.h"


#include "HazelAudio.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "GLFW/glfw3.h"
#include "Log.h"
#include "Hazel/Debug/Instrumentor.h"
#include "Hazel/Renderer/Renderer.h"

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        HZ_PROFILE_FUNCTION();

        HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
        m_Window->SetVSync(true);

        Renderer::Init();

        m_ImGuiLayer = new ImGuiLayer;
        PushOverlay(m_ImGuiLayer);

        Audio::Init();
    }

    void Application::OnEvent(Event& e)
    {
        HZ_PROFILE_FUNCTION();

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
        //HZ_CORE_TRACE("{0}", e);

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
            {
                break;
            }
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        HZ_PROFILE_FUNCTION();

        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        HZ_PROFILE_FUNCTION();

        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    bool Application::OnWindowClosed(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        HZ_PROFILE_FUNCTION();

        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

        return false;
    }

    Application::~Application()
    {
        HZ_PROFILE_FUNCTION();

        Renderer::Shutdown();
    }

    void Application::Run()
    {
        HZ_PROFILE_FUNCTION();
 
        while (m_Running)
        {
            HZ_PROFILE_SCOPE("Run Loop");

            auto time = (float) glfwGetTime(); // future : Platform::GetTime
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized) {
                for (Layer* layer : m_LayerStack) {
                    layer->OnUpdate(timestep);
                }
            }

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
            {
                layer->OnImGuiRender();
            }
            m_ImGuiLayer->End();


            //auto [x, y] = Input::GetMousePosition();
            //HZ_CORE_TRACE("{0}, {1}", x, y);

            m_Window->OnUpdate();
        }
    }
}
