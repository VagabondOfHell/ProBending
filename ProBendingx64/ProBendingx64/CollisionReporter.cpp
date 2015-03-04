#include "CollisionReporter.h"
#include "PxRigidActor.h"

#include "GameObject.h"
#include <stdexcept>

using namespace physx;

CollisionReporter::CollisionReporter(void)
{
}


CollisionReporter::~CollisionReporter(void)
{
}

void CollisionReporter::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CollisionReporter::onWake(PxActor** actors, PxU32 count)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CollisionReporter::onSleep(PxActor** actors, PxU32 count)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CollisionReporter::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{

	/*const PxU32 bufferSize = 64;
	PxContactPairPoint contacts[bufferSize];*/

	for(PxU32 i=0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		GameObject* thisActor = NULL;
		GameObject* otherActor = NULL;

		if(!pairHeader.flags.isSet(PxContactPairHeaderFlag::eDELETED_ACTOR_0))
			thisActor = static_cast<GameObject*>(pairHeader.actors[0]->userData);

		if(!pairHeader.flags.isSet(PxContactPairHeaderFlag::eDELETED_ACTOR_1))
			otherActor = static_cast<GameObject*>(pairHeader.actors[1]->userData);
		
		CollisionReport report;
		/*PxU32 nbContacts = pairs[i].extractContacts(contacts, bufferSize);
		for(PxU32 j=0; j < nbContacts; j++)
		{
			report.ContactPoints.push_back(contacts[j]);
		}
*/
		if(cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			if(thisActor && otherActor)
			{
				report.Collider = otherActor;
				thisActor->OnCollisionEnter(report);

				report.Collider = thisActor;
				otherActor->OnCollisionEnter(report);
			}

			/*if(otherActor)
			{
				report.Collider = thisActor;
				otherActor->OnCollisionEnter(report);
			}*/
		}
		
		if(cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			if(thisActor && otherActor)
			{
				report.Collider = otherActor;
				thisActor->OnCollisionLeave(report);
				report.Collider = thisActor;
				otherActor->OnCollisionLeave(report);
			}
		}

		if(cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			if(thisActor && otherActor)
			{
				report.Collider = otherActor;
				thisActor->OnCollisionStay(report);
				report.Collider = thisActor;
				otherActor->OnCollisionStay(report);
			}
		}
	}
}

void CollisionReporter::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for(PxU32 i = 0; i < count; i++)
	{
		const PxTriggerPair& cp = pairs[i];

		GameObject* activator = NULL;
		GameObject* triggerActor = NULL;

		if(!cp.flags.isSet(PxTriggerPairFlag::eDELETED_SHAPE_OTHER))
			activator = static_cast<GameObject*>(cp.otherActor->userData);

		if(!cp.flags.isSet(PxTriggerPairFlag::eDELETED_SHAPE_TRIGGER))
			triggerActor = static_cast<GameObject*>(cp.triggerActor->userData);

		if(cp.status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			if(activator)
				activator->OnTriggerEnter(triggerActor, activator);
			if(triggerActor)
				triggerActor->OnTriggerEnter(triggerActor, activator);
		}

		if(cp.status == PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			if(activator)
				activator->OnTriggerLeave(triggerActor, activator);
			if(triggerActor)
				triggerActor->OnTriggerLeave(triggerActor, activator);
		}
	}
}
