#include "MenuNavigator.h"

#include "InputManager.h"
#include "MenusScene.h"
#include "GUIManager.h"

MenuNavigator::MenuNavigator(MenusScene* _menuScene /*= NULL*/)
	:menuScene(_menuScene)
{

}

MenuNavigator::~MenuNavigator(void)
{
}

void MenuNavigator::BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData)
{
	
}

void MenuNavigator::BodyLost(const CompleteData& currentData, const CompleteData& previousData)
{
	InputManager::GetInstance()->UnregisterBodyListener(this);
}

void MenuNavigator::mouseMoved(const OIS::MouseEvent &arg)
{
	menuScene->GetGUIManager()->InjectMouseMove(arg.state.X.rel, arg.state.Y.rel);
}

void MenuNavigator::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	menuScene->GetGUIManager()->InjectMouseButtonDown(id);
}

void MenuNavigator::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	menuScene->GetGUIManager()->InjectMouseButtonUp(id);
}
