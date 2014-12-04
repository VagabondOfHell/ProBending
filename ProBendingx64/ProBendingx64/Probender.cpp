#include "Probender.h"
#include "IScene.h"
#include "InputNotifier.h"
#include "NotImplementedException.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "ProbenderData.h"
#include "PxPhysics.h"
#include "extensions\PxSimpleFactory.h"
#include "PxMaterial.h"

Probender::Probender()
	:physicsBody(NULL), leftHandAttack(NULL), rightHandAttack(NULL), currentTarget(NULL)
{
}

Probender::Probender(const unsigned short _contestantID, Arena* _owningArena)
	:contestantID(_contestantID), owningArena(_owningArena), physicsBody(NULL), 
		leftHandAttack(NULL), rightHandAttack(NULL), currentTarget(NULL)
{
	
}

Probender::~Probender(void)
{
}

void Probender::AttachToScene(IScene* scene)
{
	using namespace physx;

	//Set this probender to the input handle so it has reference
	inputHandler.SetProbenderToHandle(this);
	
	PxBoxGeometry box = PxBoxGeometry(1.0f, 1.0f, 1.0f);
	PxMaterial* mat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	physicsBody = PxCreateDynamic(PxGetPhysics(), PxTransform(PxVec3(),PxQuat::createIdentity()),box, *mat, 1.0f);

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

void Probender::CreateInGameData(const ProbenderData& data)
{
	characterData.ElementAbilities.Element = data.Attributes.MainElement;
	characterData.SubelementAbilities.Element = data.Attributes.SubElement;

	currentElement = characterData.GetMainElement();
}

void Probender::SetInputState(const InputState newState)
{
	switch (newState)
	{
	case Probender::Listen:
		inputHandler.BeginListeningToAll();
		break;
	case Probender::Pause:
		inputHandler.PauseListeningToAll();
		break;
	case Probender::Stop:
		inputHandler.StopListeningToAll();
		break;
	}
}

void Probender::SetCurrentElement(const ElementEnum::Element elementToSet)
{
	if(currentElement != elementToSet)
	{
		//If the element is one of the elements available to the bender
		/*if(elementToSet == characterData.GetMainElement() ||
		elementToSet == characterData.GetSubElement())*/
		{
			currentElement = elementToSet;
		}
	}
}

void Probender::RemoveProjectile(Projectile* projectileToRemove)
{
	if(leftHandAttack == projectileToRemove)
		leftHandAttack = NULL;
	else if(rightHandAttack == projectileToRemove)
		rightHandAttack = NULL;
}
