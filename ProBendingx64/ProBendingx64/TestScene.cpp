#include "TestScene.h"
#include "OgreRenderWindow.h"
#include "DotSceneLoader.h"

TestScene::TestScene()
	:IScene(NULL, NULL, "", "")
{
}

TestScene::TestScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName)
	:IScene(_owningManager, root, _sceneName, _resourceGroupName)
{

}

TestScene::~TestScene(void)
{
}

void TestScene::Start()
{
	if(!started)
	{
		DotSceneLoader loader;
		loader.parseDotScene("SampleScene2.xml","General", ogreSceneManager);

		mainOgreCamera = ogreSceneManager->getCamera("intViewCamera1");
		
		owningManager->GetRenderWindow()->removeAllViewports();

		Ogre::Viewport* viewport = owningManager->GetRenderWindow()->addViewport(mainOgreCamera);
		viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));

		mainOgreCamera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

		started = true;
	}
}

bool TestScene::Update(float gameTime)
{
	return true;
}

void TestScene::Close()
{
}