#include "BlankScene.h"
#include "KinectAudioEventNotifier.h"
#include "InputNotifier.h"
#include "GameObject.h"
#include "OgreManualObject.h"
#include "OgreEntity.h"
#include <CEGUI\EventArgs.h>
#include "SceneManager.h"
#include "OgreSceneManager.h"
#include "GUIManager.h"
#include "OgreRenderWindow.h"
#include "PxPhysics.h"
#include "PxRigidActor.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"
#include "PxScene.h"
#include "extensions\PxSimpleFactory.h"

BlankScene::BlankScene(void)
	:IScene(NULL, NULL, "", "")
{
}

BlankScene::BlankScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName)
	:IScene(_owningManager, root, _sceneName, _resourceGroupName)
{
	
}

BlankScene::~BlankScene(void)
{
	if(object)
	{
		delete object;
		object = NULL;
	}

	InputNotifier::GetInstance()->RemoveObserver(guiManager);
}

void BlankScene::Initialize()
{
	IScene::Initialize();
}

void BlankScene::Start()
{
	if(!started)
	{
		InitializePhysics(physx::PxVec3(0.0f, -9.8f, 0.0f));

		mainOgreCamera = ogreSceneManager->createCamera("MainCamera");

		owningManager->GetRenderWindow()->removeAllViewports();

		Ogre::Viewport* viewport = owningManager->GetRenderWindow()->addViewport(mainOgreCamera);
		viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

		mainOgreCamera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

		mainOgreCamera->setPosition(0, 0, 180.0f);
		mainOgreCamera->lookAt(0, 0, 0);
		mainOgreCamera->setNearClipDistance(5);
		mainOgreCamera->setFarClipDistance(1000);
		
		Ogre::Light* light = ogreSceneManager->createLight();
		light->setType(Ogre::Light::LightTypes::LT_DIRECTIONAL);
		light->setAttenuation(10000, 1.0, 1, 1);
		light->setPosition(0, 0, 20.0f);

		KinectAudioEventNotifier::GetInstance()->RegisterAudioListener(this);


		started = true;

		InputNotifier::GetInstance()->AddObserver(guiManager);

		guiManager->AddScheme("VanillaSkin.scheme");
		guiManager->AddWindow("VanillaConsole.layout", "window1");
		guiManager->AddWindow("EffectsDemo.layout");

		guiManager->GetChildItem("EffectsDemoRoot/EffectsFrameWindow/MultiLineEditbox1");
		
		object = new GameObject(this);

		object->LoadModel("Rock_01.mesh");

		object->gameObjectNode->scale(0.5f, 0.5f, 0.5f);
		
		physx::PxReal density = 1.0f;
		physx::PxTransform transform(physx::PxVec3(0.0f), physx::PxQuat::createIdentity());
		physx::PxBoxGeometry geometry;

		object->ConstructBoxFromEntity(geometry);

		object->rigidBody = physx::PxCreateDynamic(PxGetPhysics(), transform, geometry, *PxGetPhysics().createMaterial(0.5,0.5,0.5), density);

		object->GetDynamicRigidBody()->setAngularDamping(0.75);
		object->GetDynamicRigidBody()->setLinearVelocity(physx::PxVec3(10.0f,0,0)); 
		
		//physicsWorld->addActor(*object->rigidBody);
		
		object->CreatePhysXDebug();

		CEGUI::Window* window = (CEGUI::Window*)guiManager->GetChildItem("EffectsDemoRoot");
		//window->hide();

		CEGUI::Window* submitButton = (CEGUI::Window*)guiManager->GetChildItem("window1/Submit");
		
		if(submitButton)
		{
			//submitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&BlankScene::ButtonClick, this));
			submitButton->subscribeEvent(CEGUI::PushButton::EventMouseEntersSurface, CEGUI::Event::Subscriber(&BlankScene::ButtonClick, this));
		}
	}
}

bool BlankScene::ButtonClick(const CEGUI::EventArgs &e)
{
	printf("Button is clicked");

	return true;
}

bool BlankScene::Update(float gameTime)
{
	guiManager->Update(gameTime);
	object->gameObjectNode->translate(Ogre::Vector3(0.0f, 0.0f, 0.0f));

	physicsWorld->simulate(gameTime);

	physicsWorld->fetchResults(true);

	object->Update(gameTime);

	return true;
}

void BlankScene::AudioDataReceived(AudioData* audioData)
{
	if(audioData->ChildData->CommandValue == L"RED")
	{
		mainOgreCamera->getViewport()->setBackgroundColour(Ogre::ColourValue::Red);
	}
	else if(audioData->ChildData->CommandValue == L"GREEN")
	{
		mainOgreCamera->getViewport()->setBackgroundColour(Ogre::ColourValue::Green);
	}
	else if(audioData->ChildData->CommandValue == L"BLUE")
	{
		mainOgreCamera->getViewport()->setBackgroundColour(Ogre::ColourValue::Blue);
	}
	else if(audioData->ChildData->CommandValue == L"BLACK")
	{
		mainOgreCamera->getViewport()->setBackgroundColour(Ogre::ColourValue::Black);
	}
}

void BlankScene::Close()
{
	KinectAudioEventNotifier::GetInstance()->UnregisterAudioListener(this);
}
