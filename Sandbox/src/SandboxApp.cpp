#include "Hazel.h"
#include "Hazel/Core/EntryPoint.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Platform/OpenGL/OpenGLShader.h"
#include "Sandbox2D.h"


class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() : Layer("Example"), m_CameraController(1280.0f / 720.0f, true), m_SquarePosition(0.0f)
    {


        m_VertexArray = Hazel::VertexArray::Create();


        float vertices[3 * 7] = {
            -0.5, -0.5, 0, 0.8, 0.2, 0.8, 1,
            0.5, -0.5, 0, 0.2, 0.3, 0.8, 1,
            0, 0.5, 0, 0.8, 0.8, 0.2, 1
        };

        Hazel::Ref<Hazel::VertexBuffer> vertexBuffer = Hazel::VertexBuffer::Create(vertices, sizeof(vertices));


        Hazel::BufferLayout layout = {
            {Hazel::ShaderDataType::Float3, "a_Position"},
            {Hazel::ShaderDataType::Float4, "a_Color"}
        };

        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);


        unsigned int indices[3] = {0, 1, 2};
        Hazel::Ref<Hazel::IndexBuffer> indexBuffer = Hazel::IndexBuffer::Create(
            indices, sizeof(indices) / sizeof(unsigned int));
        m_VertexArray->SetIndexBuffer(indexBuffer);


        float squareVertices[5 * 4] = {
            -0.5, -0.5, 0, 0, 0,
            0.5, -0.5, 0, 1, 0,
            0.5, 0.5, 0, 1, 1,
            -0.5, 0.5, 0, 0, 1
        };


        m_SquareVA = Hazel::VertexArray::Create();
        Hazel::Ref<Hazel::VertexBuffer> squareVB = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));

        squareVB->SetLayout({
            {Hazel::ShaderDataType::Float3, "a_Position"},
            {Hazel::ShaderDataType::Float2, "a_TexCoord"}
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        unsigned int squareIndices[6] = {
            0, 1, 2,
            2, 3, 0
        };
        Hazel::Ref<Hazel::IndexBuffer> squareIB = Hazel::IndexBuffer::Create(
            squareIndices, sizeof(squareVertices) / sizeof(unsigned int));
        m_SquareVA->SetIndexBuffer(squareIB);

        std::string vertexSrc =
            R"(
        #version 460 core

        // vertexattribpointer 0 here as location, where the attribute is in the vertex buffer
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec3 v_Position;
        out vec4 v_Color;

        void main() {
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            //gl_Position = vec4(a_Position, 1.0);
            v_Position = a_Position;
            v_Color = a_Color;
        }

        )";

        std::string fragmentSrc =
            R"(
        #version 460 core

        layout(location = 0) out vec4 color;

        in vec3 v_Position;
        in vec4 v_Color;

        void main() {
            color = vec4(v_Position * 0.5 + 0.5, 1.0);
            color = v_Color;
        }
            
        )";


        // FLAT COLOR
        std::string flatColorShaderVertexSrc =
            R"(
        #version 460 core

        // vertexattribpointer 0 here as location, where the attribute is in the vertex buffer
        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec3 v_Position;

        void main() {
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            v_Position = a_Position;
        }

        )";

        std::string flatColorShaderFragmentSrc =
            R"(
        #version 460 core

        layout(location = 0) out vec4 color;

        uniform vec3 u_Color;

        in vec3 v_Position;

        void main() {
            color = vec4(u_Color, 1.0);
        }
            
        )";

        m_Shader = Hazel::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

        m_FlatColorShader = Hazel::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

        auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

        m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
        m_ChernoLogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");


        textureShader->Bind();
        textureShader->SetInt("u_Texture", 0);
    }

    void OnUpdate(Hazel::Timestep ts) override
    {
        //HZ_TRACE("{0}", ts);
        m_CameraController.OnUpdate(ts);

        Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        Hazel::RenderCommand::Clear();


        Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

        static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
        std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->
            UploadUniformFloat3("u_Color", m_SquareColor);

        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 20; x++)
            {
                glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
                Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
            }
        }

        auto textureShader = m_ShaderLibrary.Get("Texture");

        m_Texture->Bind();
        Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
        m_ChernoLogoTexture->Bind();
        Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        // Triangle
        //std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_Shader)->Bind();
        //Hazel::Renderer::Submit(m_Shader, m_VertexArray);

        Hazel::Renderer::EndScene();
    }

    void OnImGuiRender() override
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

    void OnEvent(Hazel::Event& e) override
    {
        m_CameraController.OnEvent(e);
    }

private:
    Hazel::ShaderLibrary m_ShaderLibrary;
    Hazel::Ref<Hazel::Shader> m_Shader;
    Hazel::Ref<Hazel::Shader> m_FlatColorShader;
    Hazel::Ref<Hazel::VertexArray> m_VertexArray;
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;

    Hazel::Ref<Hazel::Texture2D> m_Texture, m_ChernoLogoTexture;

    Hazel::OrthographicCameraController m_CameraController;

    glm::vec3 m_SquarePosition;
    glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};


class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
        PushLayer(new Sandbox2D);
        //PushLayer(new ExampleLayer);
    }

    ~Sandbox() override = default;
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox;
}
