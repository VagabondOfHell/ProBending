#pragma once
#include "KinectAudioListener.h"
#include "KinectBodyListener.h"

#include "InputManager.h"
#include "InputNotifier.h"

#include "Attack.h"

#include "ProbenderData.h"
#include "ProbenderFlags.h"
#include "ProbenderOptions.h"

class Probender;

struct ConfigurationLayout
{
	typedef unsigned int KeyboardKey;

	KeyboardKey AttackButton;
	KeyboardKey JumpButton;
	KeyboardKey StopListeningButton;

	ConfigurationLayout(KeyboardKey attackButton = OIS::KC_SPACE, KeyboardKey jumpButton = OIS::KC_J, 
		KeyboardKey stopListening = OIS::KC_1)
		:AttackButton(attackButton), JumpButton(jumpButton), StopListeningButton(stopListening)
	{

	}
};

class ProbenderInputHandler :
	public KinectBodyListener, public InputObserver, public KinectAudioListener
{
public:
	enum ProbenderStances{UnknownStance, OffenseStance, DefenceStance};
	BodyDimensions bodyDimensions;
	
private:
	static const float LEAN_RESET_DISTANCE;
	static const float ATTACK_PAUSE; //A very short pause to prevent multiple gestures from acting at the same time

	Probender* probender;
	ProbenderStances currentStance;
	
	float attackBreather;

	bool canLean;

	std::vector<Attack> mainElementGestures;
	std::vector<Attack> subElementGestures;

	Attack* activeAttack;

	void GenerateGestures();

	void PopulateWithGestures(std::vector<Attack>& elementVector, ElementEnum::Element element);

	///<summary>Updates the Probender Mesh to match the Kinect Input</summary>
	///<param name="currentData">The current data of the frame</param>
	void UpdateDisplay(const CompleteData& currentData);

	///<summary>Checks current and previous lean values and fires the Dodge Movement on the probender if valid</summary>
	///<param name="currentData">Data of the current body frame</param>
	///<param name="previousData">Data of the last body frame</param>
	void CheckLean(const CompleteData& currentData, const CompleteData& previousData);

	void CheckJump(const CompleteData& currentData, const CompleteData& previousData);

	void HandleAttacks(const AttackData& attackData);

public:
	ConfigurationLayout keysLayout;
	ProbenderOptions controlOptions;

	bool ManageStance; //True to allow the input handler to set Stances based on foot position. False to require input on stances
	
	ProbenderInputHandler(Probender* _probenderToHandle = NULL, bool manageStance = true,
		ConfigurationLayout keyLayout = ConfigurationLayout());

	virtual ~ProbenderInputHandler(void);

	void SetProbenderToHandle(Probender* _probenderToHandle);

	bool ListenToBody(short bodyIndex);

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
	inline void BeginListeningToKinectBody(){
		if(!IsListening())
			InputManager::GetInstance()->RegisterListenerToNewBody(this); 
		KinectBodyListener::Enabled = true;
	}

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
	virtual void keyDown(const OIS::KeyEvent &arg);
	virtual void keyPressed( const OIS::KeyEvent &arg );
	virtual void keyReleased( const OIS::KeyEvent &arg );
	virtual void mouseMoved( const OIS::MouseEvent &arg );
	virtual void mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual void mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	virtual void BodyAcquired();

	virtual void BodyLost(const CompleteData& currentData, const CompleteData& previousData);

#pragma endregion
};

