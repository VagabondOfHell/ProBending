#pragma once
#include "IScene.h"
#include "ProbenderData.h"
#include <vector>
#include "InputObserver.h"
#include "CollisionReporter.h"
#include "PxBatchQuery.h"

class Arena;

class GameScene:public IScene, public InputObserver
{
protected:
	Arena* battleArena;
	CollisionReporter collisionReporter;
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

	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg ){return true;}
	virtual bool mouseMoved( const OIS::MouseEvent &arg ) {return true;}
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {return true;}
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){return true;}


};

