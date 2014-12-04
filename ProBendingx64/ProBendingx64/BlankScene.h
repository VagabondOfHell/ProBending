#pragma once
#include "IScene.h"
#include "KinectAudioListener.h"

class GameObject;

namespace CEGUI
{
	class EventArgs;
};

class BlankScene :
	public IScene, public KinectAudioListener
{
public:
	GameObject* object;

	BlankScene(void);

	BlankScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName);

	~BlankScene(void);

	virtual void Initialize();

	virtual void Start();

	virtual bool Update(float gameTime);
	
	virtual void AudioDataReceived(AudioData* audioData);

	virtual void Close();

	bool ButtonClick(const CEGUI::EventArgs &e);
};

