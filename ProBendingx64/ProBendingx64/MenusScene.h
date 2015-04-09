#pragma once
#include "IScene.h"
#include "MenuNavigator.h"
#include "MenuHandler.h"
#include "ProbenderData.h"

#include "RadialProgressBar.h"

class MenusScene :
	public IScene, public KinectSensorListener
{
public:
	//enum Screens{MainMenu, CharacterSetup, GameSetup, Count};
	enum Screens{MainMenu, GameSetup, Count};

	static RadialProgressBar progressBar;

protected:
	static const float HOVER_TIME;

	MenuNavigator player1Nav;
	MenuNavigator player2Nav;

	MenuHandler* handlers[Screens::Count];

	Screens currentScreen;

	bool kinectConnected;

	CEGUI::Window* hoverWindow;

	bool onWindow;
	float timePassed;

public:
	
	ProbenderData Player1Data, Player2Data;

	MenusScene(void);
	MenusScene(SceneManager* _owningManager, Ogre::Root* root, Screens screenToSet = MainMenu);

	virtual ~MenusScene(void);

	void SetScreen(Screens screenToSet);

	virtual void Initialize();

	RadialProgressBar* GetRadialProgressBar()const{return &progressBar;}

	virtual void Start();

	virtual bool Update(float gameTime);

	virtual void Close();

	void SwitchToGame();

	inline void HandleAudioInput(const AudioData* audioData)
	{handlers[currentScreen]->ReceiveAudioInput(audioData);}

	void RegisterHoverBeginEvent(CEGUI::Window* window);
	void RegisterHoverEndEvent(CEGUI::Window* window);

	inline void RegisterHoverEvents(CEGUI::Window* window)
	{
		RegisterHoverBeginEvent(window);
		RegisterHoverEndEvent(window);
	}

	void UnregisterHoverBeginEvent(CEGUI::Window* window);
	void UnregisterHoverEndEvent(CEGUI::Window* window);

	inline void UnregisterHoverEvents(CEGUI::Window* window)
	{
		UnregisterHoverBeginEvent(window);
		UnregisterHoverEndEvent(window);
	}

	virtual bool HoverBeginEvent(const CEGUI::EventArgs& e); 
	virtual bool HoverEndEvent(const CEGUI::EventArgs& e); 

	virtual void SensorDisconnected();

};

