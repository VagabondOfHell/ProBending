#include "KinectSpeechReader.h"
#include "KinectAudioEventNotifier.h"
#include <vector>

KinectSpeechReader::KinectSpeechReader(KinectReader* _kinectSensor)
{
	kinectSensor = _kinectSensor;
	speechRecognizer = NULL;
}


KinectSpeechReader::~KinectSpeechReader(void)
{
	if(speechRecognizer)
	{
		delete speechRecognizer;
		speechRecognizer = NULL;
	}

	 CoUninitialize();
}

KinectReader* KinectSpeechReader::GetKinectReader()const
{
	return kinectSensor;
}

bool KinectSpeechReader::Initialize()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	speechRecognizer = new KinectSpeechRecognizer(this);

	return speechRecognizer->Initialize();
}

bool KinectSpeechReader::LoadGrammarFile(std::string grammarPath)
{
	if(speechRecognizer)
	{
		return speechRecognizer->LoadGrammar(grammarPath);
	}
	
	return false;
}

void KinectSpeechReader::SetConfidenceThreshold(float confidence)
{
	if(confidence > 1.0f)
		confidence = 1.0f;
	if(confidence < 0.0f)
		confidence = 0.0f;

	confidenceThreshold = confidence;
}

bool KinectSpeechReader::StartRecognition()
{
	return speechRecognizer->StartRecognition();
}

void KinectSpeechReader::Capture()
{
	//Initialize all events to an empty version
	SPEVENT curEvent[5] = {{SPEI_UNDEFINED, SPET_LPARAM_IS_UNDEFINED, 0, 0, 0, 0}};
    ULONG fetched = 0;
    HRESULT hr = S_OK;
	
	//Pull one at a time
	ISpRecoContext* speechContext = speechRecognizer->GetRecognitionContext();

	speechContext->GetEvents(5, &curEvent[0], &fetched);

	AudioData* audioData = NULL;

	for (int i = 0; i < fetched; i++)
	{
		switch (curEvent[i].eEventId)
		{
		case SPEI_RECOGNITION:
			if (SPET_LPARAM_IS_OBJECT == curEvent[i].elParamType)
            {
				 // this is an ISpRecoResult
                ISpRecoResult* result = reinterpret_cast<ISpRecoResult*>(curEvent[i].lParam);
                SPPHRASE* pPhrase = NULL;
				
                hr = result->GetPhrase(&pPhrase);
                if (SUCCEEDED(hr))
                {
					const SPPHRASEPROPERTY* currentPhraseProperty = pPhrase->pProperties;
                        
                    if (currentPhraseProperty != NULL)
                    {
						//Use recursive method to process the data and its children
						audioData = ProcessPhrase(currentPhraseProperty);

						if(audioData)
							//Inject frame data to the notifier
								KinectAudioEventNotifier::GetInstance()->InjectAudioFrameData(audioData);
                    }
                    ::CoTaskMemFree(pPhrase);
					result->Release();
					result = NULL;
                }
			}
			break;

		default:
			break;
		}
	}//End of For Loop

}

AudioData*  KinectSpeechReader::ProcessPhrase(const SPPHRASEPROPERTY* phrase)
{
	///RECURSION METHOD///
	///Tried to avoid this recursion, but unfortunately in this case it was the best way to go///

	///Use pointers to reduce Copying, as each phrase would have to copy its relatives
	if(phrase->SREngineConfidence >= confidenceThreshold ||
		phrase->Confidence >= SpeechEngineConfidence::SECNormalConfidence)
	{
		//Get the current data
		AudioData* currentPhrase = new AudioData();

		if(phrase->pszName != NULL)
			currentPhrase->CommandName = phrase->pszName;

		if(phrase->pszValue != NULL)
			currentPhrase->CommandValue = phrase->pszValue;

		currentPhrase->ConfidenceValue = phrase->SREngineConfidence;
		currentPhrase->EngineConfidence = (SpeechEngineConfidence)phrase->Confidence;
		
		//If there is a child, process it
		if(phrase->pFirstChild != NULL)
			currentPhrase->ChildData = ProcessPhrase(phrase->pFirstChild);

		//If there is a sibling, process it
		if(phrase->pNextSibling != NULL)
			currentPhrase->SiblingData =  ProcessPhrase(phrase->pNextSibling);

		//If no parent data, we are back at the root and can add it to the vector
		return currentPhrase;
	}

	return NULL;
}

bool KinectSpeechReader::GetIsPaused()const
{
	return speechRecognizer->GetPausedState();
}

void KinectSpeechReader::Pause()
{
	speechRecognizer->Pause();
}

void KinectSpeechReader::Resume()
{
	speechRecognizer->Resume();
}