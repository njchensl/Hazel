﻿#pragma once

#include "Hazel/Core/Core.h"
#include <string>

namespace Hazel
{
    class Texture2D;

    class Texture {
    public:
        virtual ~Texture() = default;

        virtual unsigned int GetWidth() = 0;
        virtual unsigned int GetHeight() = 0;

        virtual void SetData(void* data, unsigned size) = 0;

        virtual void Bind(unsigned int slot = 0) const = 0;

        virtual bool operator==(const Texture& other) const = 0;
    };

    class Texture2D : public Texture
    {
    public:
        static Ref<Texture2D> Create(const std::string& path);
        static Ref<Texture2D> Create(unsigned width, unsigned height);
    };

}
