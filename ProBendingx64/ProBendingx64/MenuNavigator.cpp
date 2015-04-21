#include "MenuNavigator.h"

#include "InputManager.h"
#include "InputNotifier.h"

#include "MenusScene.h"
#include "GUIManager.h"

#include "SceneManager.h"

#include "CEGUI/Window.h"
#include "CEGUI/CoordConverter.h"

MenuNavigator::MenuNavigator(MenusScene* _menuScene /*= NULL*/)
	:menuScene(_menuScene), AllowMenuControls(false)
{
}

MenuNavigator::~MenuNavigator(void)
{
	InputManager::GetInstance()->UnregisterBodyListener(this);
	InputManager::GetInstance()->FlushListeners();

	InputNotifier::GetInstance()->RemoveObserver(this);
}

void MenuNavigator::BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData)
{
	if(!AllowMenuControls)
		return;

	DepthSpacePoint screen = GetBody()->GetBodyReader()->GetKinectReader()->GetWindowSize();

	bool useRightHand = false;

	if(currentData.JointData[JointType_HandRight].Position.Z < currentData.JointData[JointType_HandLeft].Position.Z)
		useRightHand = true;
	else
		useRightHand = false;

	//Get the cursor from within the body's comfortable range
	DepthSpacePoint cursorPoint = PointToBodyRectangle(screen, useRightHand, currentData);

	//If the point from above is valid, inject the position
	if(cursorPoint.X != -1.0f)
	{
		menuScene->GetGUIManager()->InjectMousePosition(cursorPoint.X, cursorPoint.Y);	

		GUIManager* guiManager = menuScene->GetGUIManager();

		if(guiManager)
		{
			guiManager->InjectMousePosition(cursorPoint.X, cursorPoint.Y);

			CEGUI::USize progressBarSize = MenusScene::progressBar.GetWindow()->getSize();

			SceneManager* sceneManager = menuScene->GetSceneManager();

			CEGUI::Sizef baseWindow = CEGUI::Sizef(sceneManager->GetWindowWidth(), sceneManager->GetWindowHeight());

			CEGUI::Sizef absPos = CEGUI::CoordConverter::asAbsolute(progressBarSize, baseWindow);

			MenusScene::progressBar.SetAbsolutePosition(
				cursorPoint.X - (absPos.d_width * 0.35f),
				cursorPoint.Y - (absPos.d_height * 0.35f));
		}

		//Only check click state if its a valid position
		float confidenceTolerance = 0.4f;

		if(currentData.RightHandConfidence >= confidenceTolerance && previousData.RightHandConfidence >= confidenceTolerance)
		{
			if(currentData.RightHandState == HandState::HandState_Closed)
				menuScene->GetGUIManager()->InjectMouseButtonDown(OIS::MB_Left);
			else if(currentData.RightHandState == HandState::HandState_Open)
				menuScene->GetGUIManager()->InjectMouseButtonUp(OIS::MB_Left);
		}
	}
}

void MenuNavigator::BodyLost(const CompleteData& currentData, const CompleteData& previousData)
{
	InputManager::GetInstance()->UnregisterBodyListener(this);
}

void MenuNavigator::mouseMoved(const OIS::MouseEvent &arg)
{
	//menuScene->GetGUIManager()->InjectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	if(!AllowMenuControls)
		return;

	GUIManager* guiManager = menuScene->GetGUIManager();

	if(guiManager)
	{
		guiManager->InjectMousePosition(arg.state.X.abs, arg.state.Y.abs);

		CEGUI::USize progressBarSize = MenusScene::progressBar.GetWindow()->getSize();

		SceneManager* sceneManager = menuScene->GetSceneManager();

		CEGUI::Sizef baseWindow = CEGUI::Sizef(sceneManager->GetWindowWidth(), sceneManager->GetWindowHeight());

		CEGUI::Sizef absPos = CEGUI::CoordConverter::asAbsolute(progressBarSize, baseWindow);

		MenusScene::progressBar.SetAbsolutePosition(
			arg.state.X.abs - (absPos.d_width * 0.35f),
			arg.state.Y.abs - (absPos.d_height * 0.35f));
	}
}

void MenuNavigator::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(!AllowMenuControls)
		return;
	menuScene->GetGUIManager()->InjectMouseButtonDown(id);
}

void MenuNavigator::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(!AllowMenuControls)
		return;
	menuScene->GetGUIManager()->InjectMouseButtonUp(id);
}

void MenuNavigator::Update(float gameTime)
{
	if(!IsListening())
	{
		InputManager* inputManager = InputManager::GetInstance();

		if(inputManager->RegisterListenerToNewBody(this))
			printf("Registered!\n");
	}
}

void MenuNavigator::AudioDataReceived(AudioData* audioData)
{
	//throw std::logic_error("The method or operation is not implemented.");
}
