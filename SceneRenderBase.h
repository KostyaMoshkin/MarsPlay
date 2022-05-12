#pragma once
#include <GLEW/glew.h>
#include <GLFW/Win32/glfw3.h>

namespace GL {

	class SceneRenderBase
	{
        virtual void keyCallback(
            GLFWwindow* window,
            int key,
            int scancode,
            int action,
            int mods) = 0;

        virtual void mouseMoveCallback(
            GLFWwindow* window,
            double fPosX,
            double fPosY) = 0;

        static SceneRenderBase* event_handling_instance;

    public:
        virtual void setEventHandling() { event_handling_instance = this; }

        static void keyCallbackDispatch(
            GLFWwindow* window,
            int key,
            int scancode,
            int action,
            int mods)
        {
            if (event_handling_instance)
                event_handling_instance->keyCallback(window, key, scancode, action, mods);
        }

        static void mouseMoveCallbackDispatch(
            GLFWwindow* window,
            double fPosX,
            double fPosY
            )
        {
            if (event_handling_instance)
                event_handling_instance->mouseMoveCallback(window, fPosX, fPosY);
        }
    };
}

