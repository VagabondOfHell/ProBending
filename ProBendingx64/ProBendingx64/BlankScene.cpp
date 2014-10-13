#include "BlankScene.h"
#include "OgreRenderWindow.h"
#include "KinectAudioEventNotifier.h"
#include "InputNotifier.h"

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
	InputNotifier::GetInstance()->RemoveObserver(guiManager);
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

		InputNotifier::GetInstance()->AddObserver(guiManager);

		guiManager->AddScheme("VanillaSkin.scheme");
		guiManager->AddWindow("VanillaConsole.layout", "window1");
		guiManager->AddWindow("EffectsDemo.layout");

		guiManager->GetChildItem("EffectsDemoRoot/EffectsFrameWindow/MultiLineEditbox1");
		
		CEGUI::Window* window = (CEGUI::Window*)guiManager->GetChildItem("EffectsDemoRoot");
		window->hide();

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
