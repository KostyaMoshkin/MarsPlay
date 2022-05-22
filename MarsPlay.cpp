#include "SceneRender.h"
#include "ShaderProgram.h"
#include "RenderMegdr.h"

#include <windows.h>

#include "Vocabulary.h"
#include "Utils.h"

#include "XML\XMLreader.h"
#include "LOG\logger.h"

int main()
{
	lib::XMLreaderPtr pXMLconfig = lib::XMLreader::Create("MarsPlay.config");

	lib::LoggerSetup(lib::XMLreader::getNode(pXMLconfig->getRoot(), "Logger"));

	lib::logger::putSeparator();
	lib::logger::putTimeStamp();
	messageLn("");


	GL::SceneRenderPtr pSceneRender = GL::SceneRender::Create();

	if (!pSceneRender->init())
	{
		pSceneRender.reset();
		message("OpenGL init ERROR");
		wait_return();
	}

	pSceneRender->setEventHandling();

	int nVersionFull = pSceneRender->GetVersion();

	std::cout << "OpenGL version: " << nVersionFull << std::endl;
	messageLn(std::string(std::string("OpenGL version : ") + std::to_string(nVersionFull)).c_str());

	if (nVersionFull < 40)
	{
		pSceneRender.reset();
		messageLn("OpenGL version should be 40 or higher.");
		wait_return();
	}

	lib::iPoint2D ptScreenSize = pSceneRender->getScreenSize();

	//------------------------------------------------------------------------------------------

	GL::RenderMegdrPtr pRenderMegdr = GL::RenderMegdr::Create();
	pRenderMegdr->setConfig(pXMLconfig->getRoot());
	pRenderMegdr->setVersionGl(nVersionFull);

	if (!pRenderMegdr->init(ptScreenSize))
	{
		pSceneRender.reset();
		messageLn("OpenGL RenderMegdr init ERROR");
		wait_return();
	}
	pRenderMegdr->setVisible(true);
	pSceneRender->addElement(pRenderMegdr);

	//------------------------------------------------------------------------------------------

	lib::Vector3 vCamPosition3D(0, 0, -1.2);
	lib::Vector3 vCamRight3D(1, 0, 0);

	float fStepForward = 0.3f;
	float fRoteteAngle = 0.4f;

	pRenderMegdr->rotate(vCamPosition3D, glm::cross(-vCamPosition3D, vCamRight3D));
	pSceneRender->draw();

	while (!pSceneRender->WindowShouldClose())
	{
		Sleep(50);
		pSceneRender->PollEvents();

		if (!pSceneRender->isInteraction())
			continue;

		if (pSceneRender->isKeyPress(GL::EKeyPress::esk))
			break;

		bool bError = false;

		if (pSceneRender->isKeyPress(GL::EKeyPress::key_1))
			bError |= !pRenderMegdr->keyPress(GL::EKeyPress::key_1);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_2))
			bError |= !pRenderMegdr->keyPress(GL::EKeyPress::key_2);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_3))
			bError |= !pRenderMegdr->keyPress(GL::EKeyPress::key_3);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_4))
			bError |= !pRenderMegdr->keyPress(GL::EKeyPress::key_4);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_5))
			bError |= !pRenderMegdr->keyPress(GL::EKeyPress::key_5);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_6))
			bError |= !pRenderMegdr->keyPress(GL::EKeyPress::key_6);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_7))
			bError |= !pRenderMegdr->keyPress(GL::EKeyPress::key_7);

		if (bError)
			break;

		if (pSceneRender->isKeyPress(GL::EKeyPress::key_home))
			pRenderMegdr->mouseScroll(-3.0f);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_end))
			pRenderMegdr->mouseScroll(+3.0f);

		if (pSceneRender->isKeyPress(GL::EKeyPress::key_minus))
			fStepForward /= 1.5f;
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_equal))
			fStepForward *= 1.5f;

		fStepForward = std::max<float>(std::min<float>(fStepForward, 10.0f), 0.1f);

		lib::fPoint2D ptCursorMove = pSceneRender->getCursorMove();

		//  Ближе - дальше
		vCamPosition3D *= 1 - ptCursorMove.y / 700;

		//  Полетет вперед - назад
		lib::Quat qMove(0.0f, 0.0f, 0.0f, 0.0f);

		if (pSceneRender->isKeyPress(GL::EKeyPress::key_up))
			qMove = lib::makeQuat(fStepForward, vCamRight3D);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_down))
			qMove = lib::makeQuat(-fStepForward, vCamRight3D);

		// Поворот влево - вправо
		if (pSceneRender->isKeyPress(GL::EKeyPress::key_left))
			ptCursorMove.x = +3.0;
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_right))
			ptCursorMove.x = -3.0;

		lib::Quat qRotate = lib::makeQuat(fRoteteAngle * ptCursorMove.x, vCamPosition3D);
		vCamRight3D = qRotate * vCamRight3D;

		//  Применение поворота
		vCamPosition3D = qMove * qRotate * vCamPosition3D;

		pRenderMegdr->rotate(vCamPosition3D, glm::cross(-vCamPosition3D, vCamRight3D));
		pSceneRender->draw();
	}

	return 0;
}
