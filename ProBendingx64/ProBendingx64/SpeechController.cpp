#include "SpeechController.h"
#include <iostream>

KinectSpeechReader* reader;

SpeechController::SpeechController(KinectSpeechReader* red)
{
	reader = red;
}


SpeechController::~SpeechController(void)
{
}

void SpeechController::AudioDataReceived(AudioData* audioData)
{
	if(audioData)
	{
		std::wstring data = L"Data" + audioData->CommandValue;
		wprintf(data.c_str());
		std::wstring child = L"Child Data: " + audioData->ChildData->CommandValue;
		wprintf(child.c_str());

	std::wcout << L"Data: " << audioData->CommandValue.c_str() << std::endl;
	std::wcout << L"Child Data: " << audioData->ChildData->CommandValue.c_str() << std::endl;
	
	if(audioData->ChildData->CommandValue == L"FRUIT")
	{
		reader->LoadGrammarFile("SpeechBasics-D2D.grxml");
		
	}

	if(audioData->ChildData->CommandValue == L"RIGHT")
	{
		
		reader->LoadGrammarFile("fruitgrammar.grxml");
	}
	//if(!reader->GetIsPaused())
	//	reader->Pause();
	}
	//*quit = true;
}