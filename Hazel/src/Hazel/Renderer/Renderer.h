#pragma once

#include "RendererAPI.h"
#include "Shader.h"

namespace Hazel
{
    class OrthographicCamera;

    class Renderer
    {
    public:
        Renderer() = delete;

        static void Init();
        static void Shutdown();

        static void OnWindowResize(unsigned width, unsigned height);

        static void BeginScene(OrthographicCamera& camera);
        static void EndScene();

        static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

        static RendererAPI::API GetAPI()
        {
            return RendererAPI::GetAPI();
        }
    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;
        };

        static SceneData* m_SceneData;
    };
}
