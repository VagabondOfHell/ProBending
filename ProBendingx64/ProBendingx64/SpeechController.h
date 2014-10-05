#pragma once
#include "KinectAudioListener.h"
#include "KinectSpeechReader.h"

class SpeechController : public KinectAudioListener
{
public:
	SpeechController(KinectSpeechReader* red);
	~SpeechController(void);

	void AudioDataReceived(AudioData* audioData);
};

