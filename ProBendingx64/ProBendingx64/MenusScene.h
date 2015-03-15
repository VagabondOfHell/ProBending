#pragma once
#include "IScene.h"
#include "MenuNavigator.h"
#include "MenuHandler.h"
#include "ProbenderData.h"

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
	
	ProbenderData Player1Data, Player2Data;

	MenusScene(void);
	MenusScene(SceneManager* _owningManager, Ogre::Root* root, Screens screenToSet = MainMenu);

	virtual ~MenusScene(void);

	void SetScreen(Screens screenToSet);

	virtual void Initialize();

	virtual void Start();

	virtual bool Update(float gameTime);

	virtual void Close();

	void SwitchToGame();
};

