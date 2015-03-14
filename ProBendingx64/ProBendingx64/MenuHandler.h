#pragma once
#include "CEGUI/EventArgs.h"

#include "RadialProgressBar.h"
#include "ProbenderData.h"

namespace CEGUI
{
	class PushButton;
};

class IScene;
class GUIManager;

class MenuHandler
{
public:
	enum ButtonSkinType{BTN_SKIN_PUSH_BUTTON, BTN_SKIN_MINUS_BTN_HORZ, 
		BTN_SKIN_ADD_BTN_HORZ, BTN_SKIN_LEFT_BTN_HORZ, BTN_SKIN_RIGHT_BTN_HORZ};

protected:

	CEGUI::Window* rootWindow;

	GUIManager* guiManager;
	IScene* scene;

	struct ButtonSkinListing
	{
		std::string ClickImage, HoverImage, DisabledImage, NormalImage;
	};

	void SetButtonImage(CEGUI::Window* button, ButtonSkinType skinType, ElementEnum::Element newElement);

	std::string GetElementPrefix(ElementEnum::Element newElement);
	void GetButtonSkinSuffix(ButtonSkinType skinType, ButtonSkinListing& outVal);

public:
	MenuHandler(IScene* _scene);

	virtual ~MenuHandler(void){}

	virtual void Update(float gameTime);

	bool IsVisible();

	virtual void Show();
	virtual void Hide();

	void Disable();
	void Enable();
};

class MainMenuHandler: public MenuHandler
{
	CEGUI::PushButton* startButton;
	
public:
	MainMenuHandler(IScene* _scene);
	virtual ~MainMenuHandler();

	bool StartButtonClickEvent(const CEGUI::EventArgs& e); 

};

