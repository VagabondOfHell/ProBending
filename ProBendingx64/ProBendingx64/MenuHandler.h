#pragma once
#include "CEGUI/EventArgs.h"

namespace CEGUI
{
	class PushButton;
};

class IScene;
class GUIManager;

class MenuHandler
{
protected:
	CEGUI::Window* rootWindow;

	GUIManager* guiManager;
	IScene* scene;

public:
	MenuHandler(IScene* _scene);

	virtual ~MenuHandler(void){}

	virtual void Update(float gameTime){}

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

