#pragma once
#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel
{
    class OpenGLRendererAPI : public RendererAPI
    {
    public:
        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;

        void DrawIndexed(const Ref<VertexArray>& vertexArray, long long indexCount = 0) override;
        void Init() override;
        void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height) override;
    };
}


