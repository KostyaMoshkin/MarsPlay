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

	toLog("==============================================================");
	toLog(lib::getCurrentDateTime());
	toLog("");


	GL::SceneRenderPtr pSceneRender = GL::SceneRender::Create();

	if (!pSceneRender->init())
	{
		pSceneRender.reset();
		toLog("OpenGL init ERROR");
		wait_return();
	}

	pSceneRender->setEventHandling();

	int nVersionFull = pSceneRender->GetVersion();

	std::cout << "OpenGL version: " << nVersionFull << std::endl;
	toLog("OpenGL version : " + std::to_string(nVersionFull));

	if (nVersionFull < 40)
	{
		pSceneRender.reset();
		toLog("OpenGL version should be 40 or higher.");
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
		toLog("OpenGL RenderMegdr init ERROR");
		wait_return();
	}
	pRenderMegdr->setVisible(true);
	pSceneRender->addElement(pRenderMegdr);

	//------------------------------------------------------------------------------------------

	lib::Vector3 vCamPosition3D(0, 0, -1.5);
	lib::Vector3 vCamRight3D(1, 0, 0);
	lib::Vector3 vCamCenter(0, 1, 0);

	float fStepForward = 0.3f;
	float fRoteteAngle = 0.4f;

	pRenderMegdr->rotate(vCamPosition3D, vCamCenter, vCamPosition3D);
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

		//  ����� - ������
		vCamPosition3D *= 1.0 - ptCursorMove.y / 500;

		//  ������� ������ - �����
		lib::Quat qMove(0.0f, 0.0f, 0.0f, 0.0f);

		if (pSceneRender->isKeyPress(GL::EKeyPress::key_up))
			qMove = lib::makeQuat(fStepForward, vCamRight3D);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_down))
			qMove = lib::makeQuat(-fStepForward, vCamRight3D);

		// ������� ����� - ������
		if (pSceneRender->isKeyPress(GL::EKeyPress::key_left))
			ptCursorMove.x = +3.0;
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_right))
			ptCursorMove.x = -3.0;

		lib::Quat qRotate = lib::makeQuat(fRoteteAngle * ptCursorMove.x, vCamPosition3D);
		vCamRight3D = qRotate * vCamRight3D;

		//  ���������� ��������
		vCamPosition3D = qMove * qRotate * vCamPosition3D;

		lib::Vector3 vCamUp = vCamPosition3D;
		vCamCenter = glm::cross(-vCamPosition3D, vCamRight3D);

		pRenderMegdr->rotate(vCamPosition3D, vCamCenter, vCamUp);
		pSceneRender->draw();
	}

	return 0;
}
