#pragma once
#include "IScene.h"
#include "ProbenderData.h"
#include <vector>
#include "InputObserver.h"
#include "CollisionReporter.h"
#include "PxBatchQuery.h"
#include "CEGUI/Window.h"

class Arena;

class GameScene:public IScene, public InputObserver
{
protected:
	Arena* battleArena;
	CollisionReporter collisionReporter;

	CEGUI::Window* screenSeparator;

	Ogre::Camera* Camera2;
	
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

	virtual physx::PxSceneDesc* GetSceneDescription(physx::PxVec3& gravity, bool initializeCuda);

	virtual void Initialize();

	virtual void Start();

	virtual bool Update(float gameTime);

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

