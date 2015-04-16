#include "MenuHandler.h"

#include "MenusScene.h"

#include "GUIManager.h"

#include "CEGUI/Event.h"
#include "CEGUI/widgets/PushButton.h"

MenuHandler::MenuHandler(IScene* _scene)
	:scene(_scene), rootWindow(nullptr)
{
	guiManager = scene->GetGUIManager();
}

bool MenuHandler::IsVisible()
{
	if(rootWindow)
		return rootWindow->isVisible();

	return false;
}

void MenuHandler::Show()
{
	if(rootWindow)
	{
		rootWindow->setVisible(true);
		rootWindow->activate();
	}
}

void MenuHandler::Hide()
{
	if(rootWindow)
	{
		rootWindow->setVisible(false);
		rootWindow->deactivate();
	}
}

void MenuHandler::Disable()
{
	if(rootWindow)
		rootWindow->disable();
}

void MenuHandler::Enable()
{
	if(rootWindow)
		rootWindow->enable();
}

std::string MenuHandler::GetElementPrefix(ElementEnum::Element newElement)
{
	switch (newElement)
	{
	case ElementEnum::Air:
		return "A_";
		break;
	case ElementEnum::Earth:
		return "E_";
		break;
	case ElementEnum::Fire:
		return "F_";
		break;
	case ElementEnum::Water:
		return "W_";
		break;
	default:
		return "";
		break;
	}
}

void MenuHandler::SetControlButtonImage(CEGUI::Window* button, ButtonSkinType skinType, 
										ElementEnum::Element newElement)
{
	ButtonSkinListing buttonListing;
	std::string elementPrefix = GetElementPrefix(newElement);
	GetButtonSkinSuffix(skinType, buttonListing);

	button->setProperty("NormalImage", "MenuControls/" + elementPrefix + buttonListing.NormalImage);
	button->setProperty("DisabledImage", "MenuControls/" + elementPrefix + buttonListing.DisabledImage);
	button->setProperty("HoverImage", "MenuControls/" + elementPrefix + buttonListing.HoverImage);
	button->setProperty("PushedImage", "MenuControls/" + elementPrefix + buttonListing.ClickImage);
}

void MenuHandler::SetWindowImage(CEGUI::Window* button, const std::string& imagesetName, const std::string& imageName)
{
	if(!imageName.empty() && !imagesetName.empty())
		button->setProperty("Image", imagesetName + "/" + imageName);
}

void MenuHandler::GetButtonSkinSuffix(ButtonSkinType skinType, ButtonSkinListing& outVal)
{
	outVal.ClickImage = outVal.DisabledImage = outVal.HoverImage = outVal.NormalImage = "";

	switch (skinType)
	{
	case MenuHandler::BTN_SKIN_PUSH_BUTTON:
		outVal.ClickImage = "Btn_Click";
		outVal.DisabledImage = "Btn_Disabled";
		outVal.HoverImage = "Btn_Hover";
		outVal.NormalImage = "Btn_Normal";
		break;
	case MenuHandler::BTN_SKIN_MINUS_BTN_HORZ:
		outVal.ClickImage = "Hor_Minus_Click";
		outVal.DisabledImage = "Hor_Minus_Disabled";
		outVal.HoverImage = "Hor_Minus_Hover";
		outVal.NormalImage = "Hor_Minus_Normal";
		break;
	case MenuHandler::BTN_SKIN_ADD_BTN_HORZ:
		outVal.ClickImage = "Hor_Add_Click";
		outVal.DisabledImage = "Hor_Add_Disabled";
		outVal.HoverImage = "Hor_Add_Hover";
		outVal.NormalImage = "Hor_Add_Normal";
		break;
	case MenuHandler::BTN_SKIN_LEFT_BTN_HORZ:
		outVal.ClickImage = "Left_Sel_Click";
		outVal.DisabledImage = "Left_Sel_Disabled";
		outVal.HoverImage = "Left_Sel_Hover";
		outVal.NormalImage = "Left_Sel_Normal";
		break;
	case MenuHandler::BTN_SKIN_RIGHT_BTN_HORZ:
		outVal.ClickImage = "Right_Sel_Click";
		outVal.DisabledImage = "Right_Sel_Disabled";
		outVal.HoverImage = "Right_Sel_Hover";
		outVal.NormalImage = "Right_Sel_Normal";
		break;
	}
}

void MenuHandler::Update(float gameTime)
{
}

MainMenuHandler::MainMenuHandler(IScene* _scene)
	:MenuHandler(_scene)
{
	rootWindow = guiManager->GetChildWindow("MainMenuRoot");

	startButton = (CEGUI::PushButton*)guiManager->GetChildWindow("MainMenuRoot/EnterTournamentButton");
	startButton->subscribeEvent(CEGUI::PushButton::EventClicked, 
		CEGUI::Event::Subscriber(&MainMenuHandler::StartButtonClickEvent,this));
	
	MenusScene* menus = (MenusScene*)scene;

	menus->RegisterHoverEvents(startButton);
}

MainMenuHandler::~MainMenuHandler()
{

}

bool MainMenuHandler::StartButtonClickEvent(const CEGUI::EventArgs& e)
{
	MenusScene* menus = (MenusScene*)scene;
	//((MenusScene*)scene)->SetScreen(MenusScene::CharacterSetup);

	menus->UnregisterHoverEvents(startButton);
	menus->SetScreen(MenusScene::GameSetup);

	return true;
}

void MainMenuHandler::ReceiveAudioInput(const AudioData* audioData)
{
	if(audioData->ChildData->CommandValue == L"ENTER TOURNAMENT")
		StartButtonClickEvent(CEGUI::EventArgs());
}
