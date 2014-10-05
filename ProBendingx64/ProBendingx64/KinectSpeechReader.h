#pragma once
#include "KinectSpeechRecognizer.h"

class KinectReader;

struct AudioData
{
	//bool IsValid;
	SpeechEngineConfidence EngineConfidence;
	float ConfidenceValue;
	std::wstring CommandValue;
	std::wstring CommandName;
	AudioData* ChildData;
	AudioData* SiblingData;

	AudioData()
	{
		ChildData = NULL;
		SiblingData = NULL;
	}

	~AudioData()
	{
		if(ChildData)
		{
			delete ChildData;
			ChildData = NULL;
		}

		if(SiblingData)
		{
			delete SiblingData;
			SiblingData = NULL;
		}
	}
};

class KinectSpeechReader
{
private:
	KinectReader* kinectSensor;
	
	KinectSpeechRecognizer* speechRecognizer;
	
	float confidenceThreshold;

	AudioData* ProcessPhrase(const SPPHRASEPROPERTY* phrase);

public:
	KinectSpeechReader(KinectReader* kinectSensor);
	~KinectSpeechReader(void);

	KinectReader* GetKinectReader()const;
	
	bool Initialize();

	bool LoadGrammarFile(std::string grammarPath);

	void SetConfidenceThreshold(float confidence = 0.3f);

	bool StartRecognition();

	void Capture();

	bool GetIsPaused()const;

	void Pause();
	void Resume();
};

