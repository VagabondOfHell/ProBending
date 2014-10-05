#include "KinectSpeechRecognizer.h"
#include "KinectReader.h"

KinectSpeechRecognizer::KinectSpeechRecognizer(KinectSpeechReader* _speechReader)
{
	speechReader = _speechReader;
	isOpen = false;
	isPaused = true;

	audioBeam = NULL;
    audioStream = NULL;
	speechStream = NULL;
	speechRecognizer = NULL;
	speechContext = NULL;
	speechGrammar = NULL;
	audioStream16Bit = NULL;
}


KinectSpeechRecognizer::~KinectSpeechRecognizer(void)
{
	speechRecognizer->SetRecoState(SPRST_INACTIVE_WITH_PURGE);

	audioStream16Bit->SetSpeechState(false);

	if(speechStream)
	{
		speechStream->Release();
		speechStream = NULL;
	}

	if(speechRecognizer)
	{
		speechRecognizer->Release();
		speechRecognizer = NULL;
	}

	if(speechContext)
	{
		speechContext->Release();
		speechContext = NULL;
	}

	if(speechGrammar)
	{
		speechGrammar->Release();
		speechGrammar = NULL;
	}

	if(audioStream16Bit)
	{
		delete audioStream16Bit;
		audioStream16Bit = NULL;
	}

	if(audioStream)
	{
		audioStream->Release();
		audioStream = NULL;
	}

	if(audioBeam)
	{
		audioBeam->Release();
		audioBeam = NULL;
	}
}

ISpRecoContext* KinectSpeechRecognizer::GetRecognitionContext()const
{
	return speechContext;
}

bool KinectSpeechRecognizer::Initialize()
{
	if(!isOpen)
	{
		HRESULT hr = OpenAudioStreams();   

		if(SUCCEEDED(hr))
		{
			hr = OpenSpeechStreams();
		}
	
		if(SUCCEEDED(hr))
		{
			hr = CreateRecognizer();
		}

		if(SUCCEEDED(hr))
		{
			isOpen = true;
			return true;
		}
	}
		
	return false;
}

HRESULT KinectSpeechRecognizer::OpenAudioStreams()
{
	 IAudioSource* audioSource = NULL;
    IAudioBeamList* audioBeamList = NULL;

	HRESULT hr = speechReader->GetKinectReader()->GetKinectSensor()->get_AudioSource(&audioSource);

	if(SUCCEEDED(hr))
	{
		hr = audioSource->get_AudioBeams(&audioBeamList);
	}

	if(SUCCEEDED(hr))
	{
		hr = audioBeamList->OpenAudioBeam(0, &audioBeam);
	}

	if(SUCCEEDED(hr))
	{
		hr = audioBeam->OpenInputStream(&audioStream);
		audioStream16Bit = new KinectAudioStream(audioStream);
	}

	//These are no longer needed
	if(audioSource)
	{
		audioSource->Release();
		audioSource = NULL;
	}
	if(audioBeamList)
	{
		audioBeamList->Release();
		audioBeamList = NULL;
	}

	return hr;
}

HRESULT KinectSpeechRecognizer::OpenSpeechStreams()
{
	//Create an instance of the Speech Stream in a singleton manner
	HRESULT hr = CoCreateInstance(CLSID_SpStream, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpStream), (void**)&speechStream);

	//Initializer List for Wave Format
	WAVEFORMATEX wfxOut = {WAVE_FORMAT_PCM, //Audio Format
		1, //Audio Channels
		16000, //Audio Samples Per Second
		32000, //Audio Average Bytes Per Second 
		2, //Audio Block Align
		16, //Audio Bits Per Sample
		0};

	if(SUCCEEDED(hr))
	{
		audioStream16Bit->SetSpeechState(true);
		hr = speechStream->SetBaseStream(audioStream16Bit, SPDFID_WaveFormatEx, &wfxOut);
	}

	return hr;
}

HRESULT KinectSpeechRecognizer::CreateRecognizer()
{
	ISpObjectToken *engineToken = NULL;

	//Create singleton instance of Speech Recognizer
	HRESULT hr = CoCreateInstance(CLSID_SpInprocRecognizer, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpRecognizer), (void**)&speechRecognizer);

	if (SUCCEEDED(hr))
	{
		speechRecognizer->SetInput(speechStream, TRUE);

		// If this fails here, you have not installed the acoustic models for Kinect
		hr = SpFindBestToken(SPCAT_RECOGNIZERS, L"Language=409;Kinect=True", NULL, &engineToken);

		if (SUCCEEDED(hr))
		{
			speechRecognizer->SetRecognizer(engineToken);
			hr = speechRecognizer->CreateRecoContext(&speechContext);

			// For long recognition sessions (a few hours or more), it may be beneficial to turn off adaptation of the acoustic model. 
			// This will prevent recognition accuracy from degrading over time.
			if (SUCCEEDED(hr))
			{
				hr = speechRecognizer->SetPropertyNum(L"AdaptationOn", 0);                
			}
		}
	}

	 //Release the engine token
	 if(engineToken)
	 {
		 engineToken->Release();
		 engineToken = NULL;
	 }
    
    return hr;
}

bool KinectSpeechRecognizer::LoadGrammar(std::string fileName)
{	
	if(speechGrammar)
	{
		speechGrammar->Release();
		speechGrammar = NULL;
	}

	HRESULT hr = speechContext->CreateGrammar(1, &speechGrammar);
	
    if (SUCCEEDED(hr))
    {
		Pause();

		std::wstring wideFileName = std::wstring(fileName.begin(), fileName.end());

        // Populate recognition grammar from file
		hr = speechGrammar->LoadCmdFromFile(wideFileName.c_str(), SPLO_STATIC);
		 if (SUCCEEDED(hr))
		 {
		// Specify that all top level rules in grammar are now active
		 hr = speechGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);
		}
	
		Resume();
    }

	if(SUCCEEDED(hr))
		return true;
	else
	{
		//If failed, release the current grammar if it has anything
		if(speechGrammar)
		{
			speechGrammar->Release();
			speechGrammar = NULL;
		}
		return false;
	}
}

bool KinectSpeechRecognizer::StartRecognition()
{
	 HRESULT hr = S_OK;
	 
    // Specify that engine should always be reading audio
	 hr = speechRecognizer->SetRecoState(SPRST_ACTIVE);
	if (!SUCCEEDED(hr))
    {
        return false;
    }

    // Specify that we're only interested in receiving recognition events
	hr = speechContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
    if (FAILED(hr))
    {
        return false;
    }

    // Ensure that engine is recognizing speech and not in paused state
	hr = speechContext->Resume(0);
    if (FAILED(hr))
    {
        return false;
    }

	if(SUCCEEDED(hr))
	{
		isPaused = false;
		return true;
	}
	else 
		return false;
}

bool KinectSpeechRecognizer::GetIsOpened()const
{
	return isOpen;
}
bool KinectSpeechRecognizer::GetPausedState()const
{
	return isPaused;
}

void KinectSpeechRecognizer::Pause()
{
	speechContext->Pause(NULL);
	audioStream16Bit->SetSpeechState(false);
	isPaused = true;
}

void KinectSpeechRecognizer::Resume()
{
	speechContext->Resume(NULL);
	audioStream16Bit->SetSpeechState(true);
	isPaused = false;
}