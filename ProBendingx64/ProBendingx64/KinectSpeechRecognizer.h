#pragma once
#include "KinectAudioStream.h"
#include <Kinect.h>
#include <string>
#include <sapi.h>
#include <sphelper.h>
#include <uuids.h>

class KinectReader;
class KinectSpeechReader;

class KinectSpeechRecognizer
{
private:
	KinectSpeechReader* speechReader;

    // A single audio beam off the Kinect sensor.
    IAudioBeam*             audioBeam;

    // An IStream derived from the audio beam, used to read audio samples
    IStream*                audioStream;

    // Stream given to speech recognition engine
    ISpStream*              speechStream;

    // Speech recognizer
    ISpRecognizer*          speechRecognizer;

    // Speech recognizer context
    ISpRecoContext*         speechContext;

    // Speech grammar
    ISpRecoGrammar*         speechGrammar;
	
    // Stream for converting 32bit Audio provided by Kinect to 16bit required by speeck
    KinectAudioStream*     audioStream16Bit;

	//True if it has been opened
	bool isOpen;

	//True if paused
	bool isPaused;

	///Opens the audio streams of the Kinect to receive data for Speech Recognition
	HRESULT OpenAudioStreams();
	//Opens the speech streams of SAPI to process data for Speech Recognition
	HRESULT OpenSpeechStreams();
	//Finalizes the creation of the Speech Recognizer object
	HRESULT CreateRecognizer();

public:
	KinectSpeechRecognizer(KinectSpeechReader* _speechReader);
	~KinectSpeechRecognizer(void);

	ISpRecoContext* GetRecognitionContext()const;

	bool Initialize();
	
	//Loads the grammar file at the specified filename
	bool LoadGrammar(std::string fileName);

	bool StartRecognition();

	bool GetIsOpened()const;
	bool GetPausedState()const;
	void Pause();
	void Resume();
};

