#pragma once
#include "KinectAudioListener.h"
#include "KinectBodyListener.h"
#include "InputManager.h"
#include "InputNotifier.h"

class Probender;

struct ConfigurationLayout
{
	typedef unsigned int KeyboardKey;

	KeyboardKey AttackButton;

	ConfigurationLayout(KeyboardKey attackButton = OIS::KC_SPACE)
		:AttackButton(attackButton)
	{

	}
};

class ProbenderInputHandler :
	public KinectBodyListener, public InputObserver, public KinectAudioListener
{
public:
	enum ProbenderStances{UnknownStance, OffenseStance, DefenceStance};

private:
	Probender* probender;
	ProbenderStances currentStance;

public:
	ConfigurationLayout keysLayout;

	bool ManageStance; //True to allow the input handler to set Stances based on foot position. False to require input on stances

	ProbenderInputHandler(Probender* _probenderToHandle = NULL, bool manageStance = true,
		ConfigurationLayout keyLayout = ConfigurationLayout());

	virtual ~ProbenderInputHandler(void);

	void SetProbenderToHandle(Probender* _probenderToHandle);

	///<summary>Overrides the current stance used by the input handler and sets Manage Stance to false</summary>
	///<param name="newStance">The new stance to be used. Setting to Unknown Stance will
	///result in no gestures being registered until ManageStance is set to true or user overrides
	///with a valid stance</param>
	inline void OverrideProbenderStance(ProbenderStances newStance){currentStance = newStance; ManageStance = false;}

	///<summary>Gets the stance that is currently utilized by the character</summary>
	///<returns>The current probender stance. If Unknown, no gestures are being registered. Make sure
	///ManageStance is true or assign a valid stance</returns>
	inline ProbenderStances GetProbenderStance()const{return currentStance;}

	///<summary>Tells the input handler to begin listening to all forms of input</summary>
	void BeginListeningToAll();

	///<summary>Begins listening to Kinect Body input</summary>
	inline void BeginListeningToKinectBody(){InputManager::GetInstance()->RegisterListenerToNewBody(this); KinectBodyListener::Enabled = true;}

	///<summary>Begins listening to Kinect Speech input</summary>
	inline void BeginListeningToKinectSpeech(){InputManager::GetInstance()->RegisterAudioListener(this); KinectAudioListener::Enabled = true;}

	///<summary>Begins listening for OIS Input</summary>
	inline void BeginListeningToOISInput(){	InputNotifier::GetInstance()->AddObserver(this); InputObserver::Enabled = true;}

	///<summary>Stops listening for Kinect Body input</summary>
	inline void StopListeningToKinectBody(){InputManager::GetInstance()->UnregisterBodyListener(this);}

	///<summary>Stops listening for Kinect Speech input</summary>
	inline void StopListeningToKinectSpeech(){InputManager::GetInstance()->UnregisterAudioListener(this);}

	///<summary>Stops listening for OIS input</summary>
	inline void StopListeningToOISInput(){InputNotifier::GetInstance()->RemoveObserver(this);}

	///<summary>Tells the input handler to stop listening to all forms of input</summary>
	void StopListeningToAll();

	///<summary>Pauses listening for the Kinect Body input</summary>
	inline void PauseKinectBodyListening(){KinectBodyListener::Enabled = false;}

	///<summary>Pauses listening for the Kinect Speech input</summary>
	inline void PauseKinectSpeechListening(){KinectAudioListener::Enabled = false;}

	///<summary>Pauses listening for the Kinect Body input</summary>
	inline void PauseOISInputListening(){InputObserver::Enabled = false;}

	///<summary>Pauses listening to all input</summary>
	void PauseListeningToAll();

	///<summary>Resumes listening for the Kinect Body input</summary>
	inline void ResumeKinectBodyListening(){KinectBodyListener::Enabled = true;}

	///<summary>Resumes listening for the Kinect Speech input</summary>
	inline void ResumeKinectSpeechListening(){KinectAudioListener::Enabled = true;}

	///<summary>Resumes listening for the Kinect Body input</summary>
	inline void ResumeOISInputListening(){InputObserver::Enabled = true;}

	///<summary>Resumes listening to all input</summary>
	void ResumeListeningToAll();

	void Update(const float gameTime);

	//Protected section for all inherited listener methods
protected:

#pragma region Kinect Input

	virtual void LeanTrackingStateChanged(const CompleteData& currentData, const CompleteData& previousData);
	
	virtual void HandTrackingStateChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData);
	virtual void HandConfidenceChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData);

	virtual void BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData);

	virtual void DiscreteGesturesAcquired(const std::vector<KinectGestureResult>discreteGestureResults);
	virtual void ContinuousGesturesAcquired(const std::vector<KinectGestureResult>continuousGestureResults);
#pragma endregion

#pragma region Speech Input
	virtual void AudioDataReceived(AudioData* audioData);
#pragma endregion

#pragma region Mouse and Key Input
	virtual bool keyDown(const OIS::KeyEvent &arg);
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	virtual void BodyAcquired();

	virtual void BodyLost(const CompleteData& currentData, const CompleteData& previousData);

#pragma endregion
};

