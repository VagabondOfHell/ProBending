#pragma once

struct AudioData;

class KinectAudioListener
{
public:
	KinectAudioListener(void){}
	virtual ~KinectAudioListener(void){}

	virtual void AudioDataReceived(AudioData* audioData){}
};

