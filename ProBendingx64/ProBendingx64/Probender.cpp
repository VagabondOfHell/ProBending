#include "Probender.h"
#include "IScene.h"
#include "InputNotifier.h"
#include "NotImplementedException.h"

Probender::Probender()
{
	physicsBody = NULL;

	//Start the hand attacks to NULL
	leftHandAttack = NULL;
	rightHandAttack = NULL;

	//Set no current target
	currentTarget = NULL;
}


Probender::~Probender(void)
{
}

void Probender::AttachToScene(IScene* scene)
{
	//Set this probender to the input handle so it has reference
	inputHandler.SetProbenderToHandle(this);
	//Begin listening for input
	inputHandler.BeginListeningToAll();
	scene->GetPhysXScene()->addActor(*physicsBody);
}

void Probender::Update(float gameTime)
{
	stateManager.Update(gameTime);	
}

void Probender::AcquireNewTarget(bool toRight)
{
	throw NotImplementedException();
}