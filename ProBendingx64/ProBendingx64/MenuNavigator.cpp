#include "MenuNavigator.h"

#include "InputManager.h"
#include "InputNotifier.h"

#include "MenusScene.h"
#include "GUIManager.h"

MenuNavigator::MenuNavigator(MenusScene* _menuScene /*= NULL*/)
	:menuScene(_menuScene)
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
	DepthSpacePoint screen = GetBody()->GetBodyReader()->GetKinectReader()->GetWindowSize();

	//Get the cursor from within the body's comfortable range
	DepthSpacePoint cursorPoint = PointToBodyRectangle(screen, currentData.JointData[JointType_HandRight], currentData);

	//If the point from above is valid, inject the position
	if(cursorPoint.X != -1.0f)
	{
		menuScene->GetGUIManager()->InjectMousePosition(cursorPoint.X, cursorPoint.Y);	

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

	menuScene->GetGUIManager()->InjectMousePosition(arg.state.X.abs, arg.state.Y.abs);
}

void MenuNavigator::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	menuScene->GetGUIManager()->InjectMouseButtonDown(id);
}

void MenuNavigator::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
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
