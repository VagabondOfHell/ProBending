#include "ProbenderInputHandler.h"
#include "InputNotifier.h"
#include "InputManager.h"
#include "Probender.h"

ProbenderInputHandler::ProbenderInputHandler(Probender* _probenderToHandle, bool manageStance)
{
	probender = _probenderToHandle;
	ManageStance = manageStance;
}


ProbenderInputHandler::~ProbenderInputHandler(void)
{
	//On destruction, indicate we want to stop listening to events.
	//We assume this will be destroyed before the input manager (which it should be)
	StopListeningToAll();
}

void ProbenderInputHandler::SetProbenderToHandle(Probender* _probenderToHandle)
{
	probender = _probenderToHandle;
}

void ProbenderInputHandler::BeginListeningToAll()
{
	BeginListeningToKinectBody();
	BeginListeningToKinectSpeech();
	BeginListeningToOISInput();
}

void ProbenderInputHandler::StopListeningToAll()
{
	StopListeningToKinectBody();
	StopListeningToKinectSpeech();
	StopListeningToOISInput();
}

void ProbenderInputHandler::PauseListeningToAll()
{
	PauseKinectBodyListening();
	PauseKinectSpeechListening();
	PauseOISInputListening();
}

void ProbenderInputHandler::ResumeListeningToAll()
{
	ResumeKinectBodyListening();
	ResumeKinectSpeechListening();
	ResumeOISInputListening();
}

#pragma region Kinect Input

void ProbenderInputHandler::LeanChanged(const CompleteData& currentData, const CompleteData& previousData)
{

}

void ProbenderInputHandler::LeanTrackingStateChanged(const CompleteData& currentData, const CompleteData& previousData)
{

}
	
void ProbenderInputHandler::HandTrackingStateChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData)
{

}

void ProbenderInputHandler::HandConfidenceChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData)
{
	
}

void ProbenderInputHandler::BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData)
{

}

void ProbenderInputHandler::DiscreteGesturesAcquired(const std::vector<KinectGestureResult>discreteGestureResults)
{
	///BE SURE TO IMPLEMENT MANAGE STANCE////
}

void ProbenderInputHandler::ContinuousGesturesAcquired(const std::vector<KinectGestureResult>continuousGestureResults)
{
	///BE SURE TO IMPLEMENT MANAGE STANCE////
}

#pragma endregion

#pragma region Speech Input

void ProbenderInputHandler::AudioDataReceived(AudioData* audioData)
{
	
}

#pragma endregion

#pragma region Mouse and Keyboard Input

bool ProbenderInputHandler::keyDown(const OIS::KeyEvent &arg)
{
	return true;
}

bool ProbenderInputHandler::keyPressed( const OIS::KeyEvent &arg )
{
	return true;
}

bool ProbenderInputHandler::keyReleased( const OIS::KeyEvent &arg )
{
	return true;
}

bool ProbenderInputHandler::mouseMoved( const OIS::MouseEvent &arg )
{
	return true;
}

bool ProbenderInputHandler::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return true;
}

bool ProbenderInputHandler::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return true;
}

#pragma endregion