#include "SceneRender.h"

#include <string>

#include <LOG/logger.h>

namespace GL {

	SceneRender::SceneRender()
	{
		glfwInit();

		m_vKeyPress.resize(120);
	}

	SceneRender::~SceneRender()
	{
		glfwTerminate();
	}

	bool SceneRender::init()
	{
#ifdef __DEBUG
		m_pWindow = glfwCreateWindow(1024, 768, "OpenGL", nullptr, nullptr);
#else
		m_pWindow = glfwCreateWindow(glfwGetVideoMode(glfwGetPrimaryMonitor())->width,
			glfwGetVideoMode(glfwGetPrimaryMonitor())->height,
			"OpenGL", glfwGetPrimaryMonitor(), nullptr);
#endif // __DEBUG

		if (m_pWindow == nullptr)
		{
			lib::logger::out("no window");
			return false;
		}

		glfwMakeContextCurrent(m_pWindow);

		glfwSetKeyCallback(m_pWindow, SceneRenderBase::keyCallbackDispatch);
		glfwSetCursorPosCallback(m_pWindow, SceneRenderBase::mouseMoveCallbackDispatch);

		glewExperimental = true; // Флаг необходим в Core-режиме OpenGL

		if( glewInit() != GLEW_OK )
			return false;

		int nWidth, nHeight;
		glfwGetFramebufferSize(m_pWindow, &nWidth, &nHeight);

		m_cursorCurrentPos = lib::fPoint2D(float(nWidth / 2), float(nHeight / 2));
		glfwSetCursorPos(m_pWindow, m_cursorCurrentPos.x, m_cursorCurrentPos.y);

		glViewport(0, 0, nWidth, nHeight);

		glEnable(GL_DEPTH_TEST);

		return true;
	}

	bool SceneRender::WindowShouldClose()
	{
		return glfwWindowShouldClose(m_pWindow);
	}

	void SceneRender::SwapBuffers()
	{
		glfwSwapBuffers(m_pWindow);
	}

	void SceneRender::clearScreen()
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	int SceneRender::GetVersion()
	{
		int nVersionFull = -1;

		if (nVersionFull != -1)
			return nVersionFull;

		//------------------------------------------------------------------------------------------

		const GLubyte* pVersion = glGetString(GL_VERSION);
		if (!pVersion)
			return nVersionFull;

		std::string sVersion = (const char*)pVersion;
		if (sVersion.length() < 3)
			return nVersionFull;

		int nVersionMain = sVersion[0] - '0';
		int nVersionSub = sVersion[2] - '0';

		nVersionFull = nVersionMain * 10 + nVersionSub;

		return nVersionFull;
	}
	
	void SceneRender::PollEvents()
	{
		glfwPollEvents();
	}

	bool SceneRender::isKeyPress(EKeyPress nKey_)
	{
		return m_vKeyPress[(int)nKey_];
	}

	lib::iPoint2D SceneRender::getScreenSize()
	{
		int nWidth, nHeight;
		glfwGetFramebufferSize(m_pWindow, &nWidth, &nHeight);

		return lib::iPoint2D(nWidth, nHeight);
	}

	void SceneRender::addElement(RenderPtr pRender_)
	{
		m_vElementRendr.push_back(pRender_);
	}

	lib::fPoint2D SceneRender::getCursorMove()
	{
		return std::exchange(m_cursorMove, {0, 0});
	}

	void SceneRender::draw()
	{
		clearScreen();

		for (RenderPtr pElement : m_vElementRendr)
			pElement->draw();

		SwapBuffers();
	}

	bool SceneRender::isCursoreMove()
	{
		return !(m_cursorMove.x == 0 && m_cursorMove.y == 0);
	}

	void SceneRender::keyCallback(GLFWwindow* pWindow_, int nKey_, int nScancode_, int nAction_, int nMods_)
	{
		bool bPress;

		if (nAction_ == GLFW_PRESS)
			bPress = true;
		else if (nAction_ == GLFW_RELEASE)
			bPress = false;
		else
			return;

		if ( nKey_ == GLFW_KEY_ESCAPE )
			m_vKeyPress[(int)EKeyPress::esk] = bPress;
		else if ( nKey_ == GLFW_KEY_1 )
			m_vKeyPress[(int)EKeyPress::key_1] = bPress;
		else if ( nKey_ == GLFW_KEY_2 )
			m_vKeyPress[(int)EKeyPress::key_2] = bPress;
		else if ( nKey_ == GLFW_KEY_3 )
			m_vKeyPress[(int)EKeyPress::key_3] = bPress;
		else if (nKey_ == GLFW_KEY_4)
			m_vKeyPress[(int)EKeyPress::key_4] = bPress;
		else if (nKey_ == GLFW_KEY_LEFT)
			m_vKeyPress[(int)EKeyPress::key_left] = bPress;
		else if (nKey_ == GLFW_KEY_RIGHT)
			m_vKeyPress[(int)EKeyPress::key_right] = bPress;
		else if (nKey_ == GLFW_KEY_UP)
			m_vKeyPress[(int)EKeyPress::key_up] = bPress;
		else if (nKey_ == GLFW_KEY_DOWN)
			m_vKeyPress[(int)EKeyPress::key_down] = bPress;
		else if (nKey_ == GLFW_KEY_DELETE)
			m_vKeyPress[(int)EKeyPress::key_delete] = bPress;
		else if (nKey_ == GLFW_KEY_PAGE_DOWN)
			m_vKeyPress[(int)EKeyPress::key_pagedown] = bPress;
		else if (nKey_ == GLFW_KEY_HOME)
			m_vKeyPress[(int)EKeyPress::key_home] = bPress;
		else if (nKey_ == GLFW_KEY_END)
			m_vKeyPress[(int)EKeyPress::key_end] = bPress;
	}

	bool SceneRender::isInteraction()
	{
		if (m_vKeyPress[(int)EKeyPress::esk])
			return true;
		else if (m_vKeyPress[(int)EKeyPress::key_1])
			return true;
		else if (m_vKeyPress[(int)EKeyPress::key_2])
			return true;
		else if (m_vKeyPress[(int)EKeyPress::key_left])
			return true;
		else if (m_vKeyPress[(int)EKeyPress::key_right])
			return true;
		else if (m_vKeyPress[(int)EKeyPress::key_up])
			return true;
		else if (m_vKeyPress[(int)EKeyPress::key_down])
			return true;
		else if (isCursoreMove())
			return true;


		return false;
	}

	void SceneRender::mouseMoveCallback(GLFWwindow* window_, double fPosX_, double fPosY_)
	{
		m_cursorMove = { m_cursorCurrentPos.x - float(fPosX_), m_cursorCurrentPos.y - float(fPosY_) };

		m_cursorCurrentPos = { float(fPosX_), float(fPosY_) };

	}


	bool SceneRender::read_error(bool check_error_, const char* szFileName /*= __FILE__*/, unsigned nLine /*= __LINE__*/, const char* szDateTime /*= __TIMESTAMP__*/)
	{
		bool result = true;

		GLenum err;
		int nCount = 0;
		while (((err = glGetError()) != GL_NO_ERROR) && ++nCount < 1000)
		{
			if (check_error_)
			{

			}

			result = false;
		}

		return result;
	}

}
