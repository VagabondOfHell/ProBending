#pragma once
#include "IScene.h"
#include "MenuNavigator.h"
#include "MenuHandler.h"

class MenusScene :
	public IScene
{
public:
	enum Screens{MainMenu, CharacterSetup, GameSetup, Count};

protected:
	MenuNavigator player1Nav;
	MenuNavigator player2Nav;

	MenuHandler* handlers[Screens::Count];

	Screens currentScreen;
	
public:

	MenusScene(void);
	MenusScene(SceneManager* _owningManager, Ogre::Root* root, Screens screenToSet = MainMenu);

	virtual ~MenusScene(void);

	void SetScreen(Screens screenToSet);

	virtual void Initialize();

	virtual void Start();

	virtual bool Update(float gameTime);

	virtual void Close();

};

