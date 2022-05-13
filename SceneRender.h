#pragma once
#include "SceneRenderBase.h"
#include "Render.h"
#include "lib.h"
#include "Vocabulary.h"

#include <memory>
#include <vector>

namespace GL {

	class SceneRender;
	using SceneRenderPtr = std::shared_ptr<SceneRender>;

	class SceneRender : public SceneRenderBase
	{
		GLFWwindow* m_pWindow = nullptr;

		std::vector<GL::RenderPtr> m_vElementRendr;

		//  Состояние клавиш
		std::vector<bool> m_vKeyPress;

		lib::fPoint2D m_cursorStartPos;
		lib::fPoint2D m_cursorCurrentPos;
		lib::fPoint2D m_cursorMove;

	public:
		SceneRender();
		~SceneRender();

	private:
		virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
		virtual void mouseMoveCallback(GLFWwindow* window, double fPosX, double fPosY) override;

	public:
		bool init();

		bool WindowShouldClose();

		void SwapBuffers();

		void clearScreen();

		int GetVersion();

		void PollEvents();

	public:
		void addElement(GL::RenderPtr pRender_);
		void draw();

	public:

		bool isKeyPress(EKeyPress nKey_);

		lib::iPoint2D getScreenSize();

		bool isCursoreMove();
		lib::fPoint2D getCursorMove();

	public:
		static bool read_error(bool check_error_, const char* szFileName = __FILE__, unsigned nLine = __LINE__, const char* szDateTime = __TIMESTAMP__);


	};
}

