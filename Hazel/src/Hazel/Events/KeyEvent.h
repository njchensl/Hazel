#pragma once
#include "hzpch.h"
#include "Event.h"

namespace Hazel
{
    class HAZEL_API KeyEvent : public Hazel::Event
    {
    public:
        int GetKeyCode() const
        {
            return m_KeyCode;
        }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(int keycode) : m_KeyCode(keycode)
        {
            //HZ_CORE_TRACE(keycode);
        }

        int m_KeyCode;
    };

    class HAZEL_API KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(const int keycode, const int repeatCount) : KeyEvent(keycode), m_RepeatCount(repeatCount)
        {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
            return ss.str();
        }

        int GetRepeatCount() const
        {
            return m_RepeatCount;
        }

        EVENT_CLASS_TYPE(KeyPressed)

    private:
        int m_RepeatCount;
    };

    class HAZEL_API KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(const int keycode) : KeyEvent(keycode)
        {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };

    class HAZEL_API KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(int keycode) : KeyEvent(keycode)
        {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };
}
