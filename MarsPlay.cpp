#include "SceneRender.h"
#include "ShaderProgram.h"
#include "RenderCube.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include <windows.h>

#include "Vocabulary.h"
#include "Utils.h"


int main()
{
	//static_assert(index < count, "index larger than array size");

	GL::SceneRenderPtr pSceneRender = std::make_shared<GL::SceneRender>();

	if (!pSceneRender->init())
	{
		message("OpenGL init ERROR");
		wait_return();
	}

	pSceneRender->setEventHandling();

	int nVersionFull = pSceneRender->GetVersion();

	std::cout << "OpenGL version: " << nVersionFull << std::endl;

	//------------------------------------------------------------------------------------------

	GL::RenderCubePtr pRenderCube = std::make_shared<GL::RenderCube>();

	lib::iPoint2D ptScreenSize = pSceneRender->getScreenSize();

	if (!pRenderCube->init(ptScreenSize))
	{
		pSceneRender.reset();
		message("OpenGL CUBE init ERROR");
		wait_return();
	}

	pSceneRender->addElement(pRenderCube);

	//------------------------------------------------------------------------------------------

	lib::Vector3 vCamPosition3D(0, 0, -1.2);
	lib::Vector3 vCamRight3D(1, 0, 0);

	float fStepForward = 1.0f;
	float fRoteteAngle = 1.0f;

	pRenderCube->rotate(vCamPosition3D, glm::cross(-vCamPosition3D, vCamRight3D));
	pSceneRender->draw();

	while (!pSceneRender->WindowShouldClose())
	{
		Sleep(50);
		pSceneRender->PollEvents();

		if ( pSceneRender->isKeyPress(GL::EKeyPress::esk) )
			break;

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

		pRenderCube->rotate(vCamPosition3D, glm::cross(-vCamPosition3D, vCamRight3D));
		pSceneRender->draw();
	}

	return 0;
}
