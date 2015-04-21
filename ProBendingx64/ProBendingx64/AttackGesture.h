#pragma once
#include "KinectBody.h"
#include "KinectGestureReader.h"
#include "ProbenderData.h"

#include <functional>
#include <vector>
#include "HelperFunctions.h"
#include "GestureEnums.h"

class Probender;
class GUIManager;

struct AttackData
{
	Probender*  _Probender;
	const BodyDimensions* _BodyDimensions;
	const CompleteData* CurrentData;
	const CompleteData* PreviousData;
	std::vector<KinectGestureResult>* DiscreteGestureResults;
	std::vector<KinectGestureResult>* ContinuousGestureResults;

	AttackData()
		:_Probender(NULL), _BodyDimensions(NULL), CurrentData(NULL), PreviousData(NULL),
		DiscreteGestureResults(NULL), ContinuousGestureResults(NULL)
	{

	}
};

static const unsigned short MAX_GESTURE_NAME_LENGTH = 31;

struct DiscreteEvaluator
{
	//Allow gesture names up to 30 characters
	wchar_t GestureName[MAX_GESTURE_NAME_LENGTH];
	//If true, this evaluator is true only if this is the first frame the gesture has been detected, otherwise it is true
	//as soon as the gesture is true
	bool TrueIfFirstDetected;
	//The value that the Kinect needs to be greater than for valid recognition of the gesture
	float ConfidenceToExceed;
	//True to append left or right to the gesture
	bool AppendSide;
	//True to require the gesture to become false before allowing another true
	bool RequireFalseReset;
	//The body side to return if this gesture is successful. If append side is true, use either
	GestureEnums::BodySide ReturnResult;
};

struct ContinuousEvaluator
{
	wchar_t GestureName[MAX_GESTURE_NAME_LENGTH];
	//The number between 0.0f and 1.0f that the gesture should exceed to be registered as true
	float ProgressToExceed;
};

struct ExtraCustomData
{
	void* CustomData;
	GestureEnums::BodySide Side;

	ExtraCustomData()
		:CustomData(NULL), Side(GestureEnums::BODYSIDE_INVALID)
	{}
};

class GestureEvaluator
{
public:
	enum EvaluatorType{ET_NONE, ET_CUSTOM, ET_DISCRETE, ET_CONTINUOUS};
	ExtraCustomData extraCustomData;

	//How this gesture should transition from the last completed transition
	GestureEnums::TransitionRules TransitionFromLast;

	bool falseResultRecd;//True if the gesture was false recently

	std::string guiImageName;
	GestureEnums::GUIGestureSlot guiGestureSlot;

	GUIManager* guiManager;

private:
	//A union of the possible types of function pointers
	union Evaluators
	{
		GestureEnums::BodySide (*CustomEvaluator)(const Probender*, 
			const BodyDimensions&, const CompleteData&, const CompleteData&, const ExtraCustomData&);
		DiscreteEvaluator discreteEvaluator;
		ContinuousEvaluator continuousEvaluator;
	};

	EvaluatorType evaluatorType;
	Evaluators evaluator;

	inline GestureEnums::BodySide EvaluateDiscretes(const std::vector<KinectGestureResult>* gestureResults)
	{
		std::wstring currName = evaluator.discreteEvaluator.GestureName;
		std::wstring leftName, rightName;

		bool otherFound = false;//True if the other side result has been evaluated (whether left or right)
		bool otherFirstDetected = false;//Result of the others' first detected result
		float otherConfidence = 0.0f;

		bool sideDependent = evaluator.discreteEvaluator.AppendSide;//easy access to side independency

		if(sideDependent)//create left and right names
		{
			leftName = currName + L"_Left";
			rightName = currName + L"_Right";
		}

		for (int i = 0; i < gestureResults->size(); i++)
		{
			bool nameMatch = false;//used to check if the current result matches the name of this evaluator
			const KinectGestureResult& currResult = (*gestureResults)[i];//quick access to current result

			bool leftSide = true;//if there is a name match for side dependent evaluator, we need to know the side later

			if(sideDependent)//if the gesture is side-sensitive
			{
				nameMatch = currResult.gestureName == leftName;//check if matches left name

				if(!nameMatch)//otherwise check right name
				{
					nameMatch = currResult.gestureName == rightName;
					leftSide = false;
				}
			}
			else//otherwise, just use base name
				nameMatch = currResult.gestureName == currName;

			if(!nameMatch)//names don't match, so move to the next one
				continue;

			//printf("Confidence: %f\n", currResult.discreteConfidence);

			//if confidence isn't good enough, return other side result, break, or continue, based
			//on description of this evaluator
			if(currResult.discreteConfidence < evaluator.discreteEvaluator.ConfidenceToExceed)
			{
				if(!sideDependent)//if we are side independant, we have found our gesture and can break early
				{
					falseResultRecd = true;
					break;
				}
				else
				{
					if(!otherFound)//if the other hasn't been found yet
					{
						otherFound = true;//indicate it was found
						otherConfidence = 0.0f;//and indicate it's confidence is not valid
						continue;
					}
					else//if the other has been found
					{
						if(otherConfidence > 0.0f)//if the other is valid
						{
							falseResultRecd = false;
							if(leftSide)//return the side
								return GestureEnums::BODYSIDE_RIGHT;
							else
								return GestureEnums::BODYSIDE_LEFT;
						}
						else//if the other is invalid, return invalid
						{
							falseResultRecd = true;
							return GestureEnums::BODYSIDE_INVALID;
						}
					}
				}
			}

			if(evaluator.discreteEvaluator.RequireFalseReset)
				if(!falseResultRecd)
					break;

			//if confidence is high enough, check what to do with results based on side dependency
			if(sideDependent)
			{
				if(otherFound)//if the other gesture has been found already
				{
					falseResultRecd = false;

					//if this confidence exceeds other, return this side
					if(currResult.discreteConfidence > otherConfidence)
					{
						if(leftSide)
							return GestureEnums::BODYSIDE_LEFT;
						else
							return GestureEnums::BODYSIDE_RIGHT;
					}
					else//otherwise return the other side
					{
						if(leftSide)
							return GestureEnums::BODYSIDE_RIGHT;
						else
							return GestureEnums::BODYSIDE_LEFT;
					}
				}
				else//if the other hasn't been found, we store these results as the others
				{
					otherFound = true;
					otherConfidence = currResult.discreteConfidence;
					otherFirstDetected = currResult.discreteFirstFrameDetected != false;
				}
			}
			else //if not side dependent
			{
				if(evaluator.discreteEvaluator.TrueIfFirstDetected)//check if first frame detected is a requirement
				{
					if(currResult.discreteFirstFrameDetected != false)
					{
						falseResultRecd = false;
						return evaluator.discreteEvaluator.ReturnResult;
					}
				}
				else
				{
					falseResultRecd = false;
					return evaluator.discreteEvaluator.ReturnResult;
				}
			}
		}
			
		return GestureEnums::BODYSIDE_INVALID;
	}

	inline GestureEnums::BodySide EvaluateContinuous(const std::vector<KinectGestureResult>* gestureResults)
	{
		for (int i = 0; i < gestureResults->size(); i++)
		{
			if((*gestureResults)[i].gestureName.c_str() == evaluator.continuousEvaluator.GestureName )
			{
				if((*gestureResults)[i].continuousProgress >= evaluator.continuousEvaluator.ProgressToExceed)
					return GestureEnums::BODYSIDE_EITHER;
			}
		}

		return GestureEnums::BODYSIDE_INVALID;
	}

public:
	//The amount of time that is allowed to pass before the gesture should be considered reset
	float TimeToComplete;

	GestureEvaluator(const std::string& _guiImageName, GestureEnums::GUIGestureSlot _guiSlot = GestureEnums::INVALID_GESTURE_SLOT,
		float _timeToComplete = 0.0f, GestureEnums::TransitionRules transitionFromLast = GestureEnums::TRANRULE_NONE)
		:guiImageName(_guiImageName), guiGestureSlot(_guiSlot), evaluatorType(ET_NONE),
		TimeToComplete(_timeToComplete), extraCustomData(ExtraCustomData()), TransitionFromLast(transitionFromLast)
	{}

	~GestureEvaluator(){}

	inline void SetCustomEvaluator(GestureEnums::BodySide(*customEvaluator)(const Probender*, const BodyDimensions&, 
		const CompleteData&, const CompleteData&, const ExtraCustomData&), ExtraCustomData extraData = ExtraCustomData())
	{
		evaluator.CustomEvaluator = customEvaluator;
		extraCustomData = extraData;
		evaluatorType = ET_CUSTOM;
	}

	inline void SetCustomEvaluator(GestureEnums::BodySide(*customEvaluator)(const Probender*, const BodyDimensions&, 
		const CompleteData&, const CompleteData&, const ExtraCustomData&), 
		GestureEnums::BodySide bodySide = GestureEnums::BODYSIDE_EITHER, void* customData = NULL)
	{
		ExtraCustomData extraData = ExtraCustomData();
		extraData.Side = bodySide;
		extraData.CustomData = customData;
		SetCustomEvaluator(customEvaluator, extraData);
	}

	inline void SetDiscreteEvaluator(const DiscreteEvaluator& discreteEvaluator)
	{

		evaluator.discreteEvaluator = discreteEvaluator;
		evaluatorType = ET_DISCRETE;
	}
	
	inline void SetDiscreteEvaluator(const std::wstring& gestureName, bool trueFirstFrame, float confidence,
		bool appendSide, bool requireFalseReset)
	{
		DiscreteEvaluator evaluator;

		size_t strLength = gestureName.size();
		if(strLength >= MAX_GESTURE_NAME_LENGTH - 1)
			strLength = MAX_GESTURE_NAME_LENGTH - 1;

		for (size_t i = 0; i < strLength; i++)
		{
			evaluator.GestureName[i] = gestureName[i];
		}
		evaluator.GestureName[strLength] = '\0';
		evaluator.TrueIfFirstDetected = trueFirstFrame;
		evaluator.ConfidenceToExceed = confidence;
		evaluator.AppendSide = appendSide;
		evaluator.RequireFalseReset = requireFalseReset;

		SetDiscreteEvaluator(evaluator);
	}

	inline void SetDiscreteEvaluator(const std::string& gestureName, bool trueFirstFrame, 
		float confidence, bool appendSide, bool requireFalseReset)
	{
		SetDiscreteEvaluator(HelperFunctions::StringToWideString(gestureName), trueFirstFrame, 
			confidence, appendSide, requireFalseReset);
	}

	inline void SetContinuousEvaluator(const ContinuousEvaluator& continuousEvaluator)
	{
		evaluator.continuousEvaluator = continuousEvaluator;
		evaluatorType = ET_CONTINUOUS;
	}

	inline void SetContinuousEvaluator(const std::wstring& gestureName, float gestureProgress)
	{
		ContinuousEvaluator evaluator;
		for (int i = 0; i < MAX_GESTURE_NAME_LENGTH; i++)
		{
			evaluator.GestureName[i] = gestureName[i];
		}
		evaluator.GestureName[MAX_GESTURE_NAME_LENGTH - 1] = '\0';
		evaluator.ProgressToExceed = gestureProgress;

		SetContinuousEvaluator(evaluator);
	}

	inline void SetContinuousEvaluator(const std::string& gestureName, float gestureProgress)
	{
		SetContinuousEvaluator(HelperFunctions::StringToWideString(gestureName), gestureProgress);
	}

	///<summary>Evaluates the passed data using the function that was given on creation</summary>
	///<param name="attackData">The attack data to evaluate</param>
	///<returns>The result of the stored functions analysis of the data, or false if the data wasn't of the required type</returns>
	GestureEnums::BodySide Evaluate(const AttackData& attackData)
	{
		switch (evaluatorType)
		{
		case GestureEvaluator::ET_NONE:
			return GestureEnums::BODYSIDE_INVALID;
			break;
		case GestureEvaluator::ET_CUSTOM:
			if(attackData.CurrentData == NULL || attackData.PreviousData == NULL)
				return GestureEnums::BODYSIDE_INVALID;
			return evaluator.CustomEvaluator(attackData._Probender, 
				*attackData._BodyDimensions, *attackData.CurrentData, *attackData.PreviousData, extraCustomData);
			break;
		case GestureEvaluator::ET_DISCRETE:
			if(attackData.DiscreteGestureResults == NULL)
				return GestureEnums::BODYSIDE_INVALID;
			return EvaluateDiscretes(attackData.DiscreteGestureResults);
			break;
		case GestureEvaluator::ET_CONTINUOUS:
			if(attackData.ContinuousGestureResults == NULL)
				return GestureEnums::BODYSIDE_INVALID;
			return EvaluateContinuous(attackData.ContinuousGestureResults);
			break;
		default:
			return GestureEnums::BODYSIDE_INVALID;
			break;
		}
	}
};

class AttackGesture
{
private:
	float timePassed;
	unsigned int currentIndex;

	std::vector<GestureEvaluator> gestureEvaluators;

	inline void SetNextTransitionData(GestureEnums::BodySide result, unsigned int currentIndex, unsigned int newIndex);

	GUIManager* guiManager;
	
	void SetGUIGestureSlot(const std::string imageName, const GestureEnums::GUIGestureSlot gestureSlot);

public:
	bool UpdateGUI;

	AttackGesture(GUIManager* _guiManager);
	~AttackGesture(void);

	///<summary>Adds an evaluator to this attack gesture</summary>
	///<param name="evaluator">The evaluator to add</param>
	inline void AddEvaluator(GestureEvaluator& evaluator)
	{
		gestureEvaluators.push_back(evaluator);
		if(gestureEvaluators.size() == 1)
			SetGUIGestureSlot(gestureEvaluators[0].guiImageName, gestureEvaluators[0].guiGestureSlot);
	}
	
	///<summary>Adds an evaluator that uses a custom defined method to execute instructions on the received body data</summary>
	///<param name="timeToComplete">The time to complete the evaluator when it is the active one</param>
	///<param name="CustomEvaluator">The pointer to the static method that will evaluate the body data</param>
	inline void AddCustomEvaluator(float timeToComplete, GestureEnums::BodySide (*CustomEvaluator)
		(const Probender*, const BodyDimensions&, const CompleteData&, const CompleteData&, const ExtraCustomData& extraCustomData),
		GestureEnums::BodySide bodySide, const std::string& _guiImageName, GestureEnums::GUIGestureSlot _guiSlot = GestureEnums::INVALID_GESTURE_SLOT,
		GestureEnums::TransitionRules transitionFromPrevious = GestureEnums::TRANRULE_NONE, void* extraData = NULL)
	{
		GestureEvaluator eval = GestureEvaluator(_guiImageName, _guiSlot, timeToComplete, transitionFromPrevious);
		eval.SetCustomEvaluator(CustomEvaluator, bodySide, extraData);
		
		gestureEvaluators.push_back(eval);

		if(gestureEvaluators.size() == 1)
			SetGUIGestureSlot(gestureEvaluators[0].guiImageName, gestureEvaluators[0].guiGestureSlot);
	}

	///<summary>Adds an evaluator that queries the Discrete Gesture data</summary>
	///<param name="timeToComplete">The time to complete the evaluator when it is the active one</param>
	///<param name="gestureName">The name of the gesture that the evaluator pertains to</param>
	///<param name="trueIfFirstFrame">Only true if this is the first frame the gesture has been detected, false if
	///any frame it can be true</param>
	///<param name="confidence">How confident the Kinect must be to register the gesture, between 0.0f and 1.0f</param>
	inline void AddDiscreteEvaluator(float timeToComplete, const std::string& gestureName, 
		const std::string& _guiImageName, GestureEnums::GUIGestureSlot _guiSlot,
		bool trueIfFirstFrame, float confidence, bool appendSide, bool requireFalseReset,
		GestureEnums::TransitionRules transitionFromPrevious = GestureEnums::TRANRULE_NONE)
	{
		GestureEvaluator eval = GestureEvaluator(_guiImageName, _guiSlot, timeToComplete, transitionFromPrevious);
		eval.SetDiscreteEvaluator(gestureName, trueIfFirstFrame, confidence, appendSide, requireFalseReset);
		gestureEvaluators.push_back(eval);

		if(gestureEvaluators.size() == 1)
			SetGUIGestureSlot(gestureEvaluators[0].guiImageName, gestureEvaluators[0].guiGestureSlot);
	}

	///<summary>Adds an evaluator that queries the Discrete Gesture data</summary>
	///<param name="timeToComplete">The time to complete the evaluator when it is the active one</param>
	///<param name="gestureName">The name of the gesture that the evaluator pertains to</param>
	///<param name="progress">The progress through the gesture that must be reached before the evaluator registers true
	///Must be between 0.0f and 1.0f</param>
	inline void AddContinuousEvaluator(float timeToComplete, const std::string& gestureName, float progress,
		const std::string& _guiImageName, GestureEnums::GUIGestureSlot _guiSlot,
		GestureEnums::TransitionRules transitionFromPrevious = GestureEnums::TRANRULE_NONE)
	{
		GestureEvaluator eval = GestureEvaluator(_guiImageName, _guiSlot, timeToComplete, transitionFromPrevious);
		eval.SetContinuousEvaluator(gestureName, progress);
		gestureEvaluators.push_back(eval);

		if(gestureEvaluators.size() == 1)
			SetGUIGestureSlot(gestureEvaluators[0].guiImageName, gestureEvaluators[0].guiGestureSlot);
	}

	void Update(float gameTime);

	///<summary>Evaluates the kinect data and returns true if all gestures have returned true</summary>
	///<param name="attackData">The attack data to evaluate with</param>
	///<returns>True if completed the last requirement, false if not</returns>
	GestureEnums::BodySide Evaluate(const AttackData& attackData);

	///<summary>Use this to transition from another gesture. It sets the appropriate side for the first
	///evaluator of this gesture based on the evaluators rules for transition</summary>
	///<param name="result">The result of the previous gesture</param>
	void TransitionFromGesture(GestureEnums::BodySide result);

	void ShowImage()
	{
		SetGUIGestureSlot(gestureEvaluators[currentIndex].guiImageName, gestureEvaluators[currentIndex].guiGestureSlot);
	}

	inline void Reset(){currentIndex = 0; timePassed = 0.0f; 
		if(gestureEvaluators.size())
			SetGUIGestureSlot(gestureEvaluators[0].guiImageName, gestureEvaluators[0].guiGestureSlot);}
};

