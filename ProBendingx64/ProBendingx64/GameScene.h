#pragma once
#include "IScene.h"
#include "ProbenderData.h"
#include "InputObserver.h"
#include "CollisionReporter.h"
#include "PxBatchQuery.h"

#include <vector>

class Arena;

namespace CEGUI
{
	class Window;
};

class GameScene:public IScene, public InputObserver
{
public:
	enum GameState{GS_INTRO, GS_COUNTDOWN, GS_TRANSITION, GS_GAMEPLAY, GS_END_GAME};

protected:
	Arena* battleArena;
	CollisionReporter collisionReporter;

	CEGUI::Window* screenSeparator;
	CEGUI::Window* progressTrackerWindow;

	Ogre::Camera* Camera2;
	
	GameState currentState;

	bool horizontalScreens;

	void SetUpCameras();
	void ChangeScreenSplit();

public:

	GameScene(void);

	///<summary>Constructor of the game scene</summary>
	///<param name="_owningManager">The Scene Manager that manages this scene</param>
	///<param name="root">The Ogre Root for rendering</param>
	///<param name="_arenaNameToLoad">The name of the arena that the game scene should create and manage</param>
	///<param name="contestantData">Contestant Data from the previous scene in the form of a Vector</param>
	GameScene(SceneManager* _owningManager, Ogre::Root* root, 
		std::string _arenaNameToLoad, std::vector<ProbenderData> contestantData);

	virtual ~GameScene(void);

	///<summary>Gets the arena used by the game scene</summary>
	///<returns>Pointer to the Game Arena</returns>
	inline Arena* const GetArena()const{return battleArena;}

	inline GameState GetCurrentState()const{return currentState;}

	virtual physx::PxSceneDesc* GetSceneDescription(physx::PxVec3& gravity, bool initializeCuda);

	virtual void Initialize();

	virtual void Start();

	virtual bool Update(float gameTime);

	void SetGameState(GameState newState);

	virtual void Close();

	inline Ogre::Camera* GetCamera2()const{return Camera2;}

	void ChangeScreenOrientation(bool horizontal)
	{
		if(horizontal != horizontalScreens)
		{
			horizontalScreens = horizontal;
			ChangeScreenSplit();
		}
	}

	virtual void keyPressed( const OIS::KeyEvent &arg );
	virtual void keyReleased( const OIS::KeyEvent &arg ){}
	virtual void mouseMoved( const OIS::MouseEvent &arg ) {}
	virtual void mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {}
	virtual void mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){}


};

