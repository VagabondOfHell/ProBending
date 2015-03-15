#pragma once
#include "KinectAudioListener.h"

struct AudioData;

class KinectAudioEventNotifier
{
private:
	static KinectAudioEventNotifier* instance;

	KinectAudioEventNotifier(void);

public:
	
	~KinectAudioEventNotifier(void);

	inline static KinectAudioEventNotifier* GetInstance()
	{
		if(!instance)
			instance = new KinectAudioEventNotifier();

		return instance;
	}

	void DestroySingleton();

	bool RegisterAudioListener(KinectAudioListener* listener);
	bool UnregisterAudioListener(KinectAudioListener* listener);

	void ProcessEvents();
	
	///<summary>Takes audio frame data and notifies all listeners</summary>
	///<param "audioData">The audio data to process. Frees memory from audio data when finished</param>
	void InjectAudioFrameData(AudioData* audioData);

};

