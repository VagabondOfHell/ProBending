#include "BlankScene.h"
#include "OgreRenderWindow.h"
#include "KinectAudioEventNotifier.h"

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
}

void BlankScene::Start()
{
	if(!started)
	{
		mainOgreCamera = ogreSceneManager->createCamera("MainCamera");

		owningManager->GetRenderWindow()->removeAllViewports();

		Ogre::Viewport* viewport = owningManager->GetRenderWindow()->addViewport(mainOgreCamera);
		viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

		mainOgreCamera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

		KinectAudioEventNotifier::GetInstance()->RegisterAudioListener(this);

		started = true;
	}
}

bool BlankScene::Update(float gameTime)
{

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
