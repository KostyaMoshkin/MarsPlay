#include "SceneRender.h"
#include "ShaderProgram.h"
#include "RenderPedr.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

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
	lib::logger::outLine("");


	GL::SceneRenderPtr pSceneRender = GL::SceneRender::Create();

	if (!pSceneRender->init())
	{
		message("OpenGL init ERROR");
		wait_return();
	}

	pSceneRender->setEventHandling();

	int nVersionFull = pSceneRender->GetVersion();

	std::cout << "OpenGL version: " << nVersionFull << std::endl;

	lib::iPoint2D ptScreenSize = pSceneRender->getScreenSize();

	//------------------------------------------------------------------------------------------

	GL::RenderPedrPtr pRenderPedr = GL::RenderPedr::Create();
	pRenderPedr->setConfig(pXMLconfig->getRoot());

	if (!pRenderPedr->init(ptScreenSize))
	{
		pSceneRender.reset();
		message("OpenGL CUBE init ERROR");
		wait_return();
	}

	pRenderPedr->setVisible(true);
	pSceneRender->addElement(pRenderPedr);
	//------------------------------------------------------------------------------------------

	GL::RenderPedrPtr pRenderMegdr = GL::RenderPedr::Create();
	pRenderMegdr->setConfig(pXMLconfig->getRoot());

	if (!pRenderMegdr->init(ptScreenSize))
	{
		pSceneRender.reset();
		message("OpenGL CUBE init ERROR");
		wait_return();
	}

	pSceneRender->addElement(pRenderMegdr);

	//------------------------------------------------------------------------------------------

	lib::Vector3 vCamPosition3D(0, 0, -1.2);
	lib::Vector3 vCamRight3D(1, 0, 0);

	float fStepForward = 1.0f;
	float fRoteteAngle = 1.0f;

	pRenderPedr->rotate(vCamPosition3D, glm::cross(-vCamPosition3D, vCamRight3D));
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

		if (pSceneRender->isKeyPress(GL::EKeyPress::key_1))
			pRenderPedr->keyPress(GL::EKeyPress::key_1);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_2))
			pRenderPedr->keyPress(GL::EKeyPress::key_2);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_3))
			pRenderPedr->keyPress(GL::EKeyPress::key_3);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_4))
			pRenderPedr->keyPress(GL::EKeyPress::key_4);

		lib::fPoint2D ptCursorMove = pSceneRender->getCursorMove();

		//  ����� - ������
		vCamPosition3D *= 1 - ptCursorMove.y / 300;

		//  ������� ������ - �����
		lib::Quat qMove(0.0f, 0.0f, 0.0f, 0.0f);

		if (pSceneRender->isKeyPress(GL::EKeyPress::key_up))
			qMove = lib::makeQuat(fStepForward, vCamRight3D);
		else if (pSceneRender->isKeyPress(GL::EKeyPress::key_down))
			qMove = lib::makeQuat(-fStepForward, vCamRight3D);

		// ������� ����� ����� - ������
		lib::Quat qRotate = lib::makeQuat(fRoteteAngle * ptCursorMove.x, vCamPosition3D);
		vCamRight3D = qRotate * vCamRight3D;

		//  ���������� ��������
		vCamPosition3D = qMove * qRotate * vCamPosition3D;

		pRenderPedr->rotate(vCamPosition3D, glm::cross(-vCamPosition3D, vCamRight3D));
		pRenderMegdr->rotate(vCamPosition3D, glm::cross(-vCamPosition3D, vCamRight3D));
		pSceneRender->draw();
	}

	return 0;
}
