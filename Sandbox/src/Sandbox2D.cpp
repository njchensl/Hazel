﻿#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>


#include "Hazel/ECS/Components.h"
#include "Hazel/Renderer/ParticleSystem.h"

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

static Hazel::Entity s_Ent;

void Sandbox2D::OnAttach()
{
    HZ_PROFILE_FUNCTION();

    m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");

    Hazel::Entity e = m_ECS.CreateQuad({0.0f, 0.0f, 0.8f}, {1.0f, 20.0f}, {glm::vec4(1.0f)});

    Hazel::Entity e1 = m_ECS.CreateQuad({0.0f, 0.0f, 0.9f}, {3.0f, 2.0f}, m_CheckerboardTexture, 0.5f,
                                        {0.2f, 0.8f, 0.3f, 1.0f});
    s_Ent = e1;
}

void Sandbox2D::OnDetach()
{
    HZ_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
    HZ_PROFILE_FUNCTION();

    {
        auto& transform = m_ECS.GetComponent<Hazel::Transform>(s_Ent);
        transform.Rotation -= 180.0f * ts;
    }

    {
        HZ_PROFILE_SCOPE("CameraController::OnUpdate");
        m_CameraController.OnUpdate(ts);
    }

    Hazel::Renderer2D::ResetStats();
    {
        HZ_PROFILE_SCOPE("Renderer Prep");
        Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        Hazel::RenderCommand::Clear();
    }

    {
        HZ_PROFILE_SCOPE("Renderer Draw");
        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

        Layer::OnUpdate(ts);

        static Hazel::ECS ecs;
        static int i = 0;
        i++;
        if (i % 180 == 45)
        {
            ecs = m_ECS;
        }
        else if (i % 180 == 90)
        {
            m_ECS = ecs;
        }

        // particle effect
        auto [mouseX, mouseY] = Hazel::Input::GetMousePosition();
        glm::vec3 mousePos = {mouseX / 1280.0f * 2.0f - 1.0f, -mouseY / 720.0f * 2.0f + 1.0f, 0.95f};
        if (Hazel::Input::IsMouseButtonPressed(0))
        {
            m_ParticleSystem.Emit({
                mousePos, {0.0f, 0.0f}, {0.5f, 0.5f}, {0.2f, 0.3f, 0.8f, 1.0f}, {0.2f, 0.8f, 0.3f, 0.5f}, 0.1f, 0.0f,
                0.02f, 1.0f
            });
        }

#if 0

        static float rotation = 0.0f;
        rotation += ts * 50.0f;

        Hazel::Renderer2D::DrawRotatedQuad({1.0f, 0.0f}, {0.8f, 0.8f}, 45.0f, {0.8f, 0.2f, 0.3f, 1.0f});
        Hazel::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
        Hazel::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
        Hazel::Renderer2D::DrawQuad(
            {0.0f, 0.0f, -0.1f}, {20.0f, 20.0f}, m_CheckerboardTexture,
            10.0f
        );
        Hazel::Renderer2D::DrawRotatedQuad(
            {-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, rotation, m_CheckerboardTexture,
            20.0f
        );

        Hazel::Renderer2D::EndScene();

        Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
        for (float y = -5.0f; y < 5.0f; y += 0.5f)
        {
            for (float x = -5.0f; x < 5.0f; x += 0.5f)
            {
                glm::vec4 color = {(x + 0.5f) / 10.0f, 0.4f, (y + 0.5f) / 10.0f, 0.70f};
                Hazel::Renderer2D::DrawQuad({x, y}, {0.45f, 0.45f}, color);
            }
        }

#endif
        Hazel::Renderer2D::EndScene();
    }
}

void Sandbox2D::OnImGuiRender()
{
    HZ_PROFILE_FUNCTION();

    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

    auto stats = Hazel::Renderer2D::GetStats();
    ImGui::Text("Renderer2D stats:");
    ImGui::Text("Draw calls: %d", stats.DrawCalls);
    ImGui::Text("Quad count: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& event)
{
    m_CameraController.OnEvent(event);
}
