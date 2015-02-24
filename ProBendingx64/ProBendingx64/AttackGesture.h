#pragma once
#include "KinectBody.h"
#include "KinectGestureReader.h"
#include "ProbenderData.h"

#include <functional>
#include <vector>
#include "HelperFunctions.h"
#include "GestureEnums.h"

class Probender;

struct AttackData
{
	Probender* const _Probender;
	const BodyDimensions& _BodyDimensions;
	const CompleteData* const CurrentData;
	const CompleteData* const PreviousData;
	const std::vector<KinectGestureResult>* const DiscreteGestureResults;
	const std::vector<KinectGestureResult>* const ContinuousGestureResults;
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
		for (int i = 0; i < gestureResults->size(); i++)
		{
			if((*gestureResults)[i].gestureName.c_str() == evaluator.discreteEvaluator.GestureName )
			{
				if((*gestureResults)[i].discreteConfidence >= evaluator.discreteEvaluator.ConfidenceToExceed)
				{
					if(evaluator.discreteEvaluator.TrueIfFirstDetected)
						if((*gestureResults)[i].discreteFirstFrameDetected != false)
						{
							return GestureEnums::BODYSIDE_EITHER;
						}
					else
						if((*gestureResults)[i].discreteDetected != false)
							return GestureEnums::BODYSIDE_EITHER;
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

	GestureEvaluator(float _timeToComplete = 0.0f, GestureEnums::TransitionRules transitionFromLast = GestureEnums::TRANRULE_NONE):evaluatorType(ET_NONE),
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
	
	inline void SetDiscreteEvaluator(const std::wstring& gestureName, bool trueFirstFrame, float confidence)
	{
		DiscreteEvaluator evaluator;
		for (int i = 0; i < MAX_GESTURE_NAME_LENGTH; i++)
		{
			evaluator.GestureName[i] = gestureName[i];
		}
		evaluator.GestureName[MAX_GESTURE_NAME_LENGTH - 1] = '\0';
		evaluator.TrueIfFirstDetected = trueFirstFrame;
		evaluator.ConfidenceToExceed = confidence;

		SetDiscreteEvaluator(evaluator);
	}

	inline void SetDiscreteEvaluator(const std::string& gestureName, bool trueFirstFrame, float confidence)
	{
		SetDiscreteEvaluator(HelperFunctions::StringToWideString(gestureName), trueFirstFrame, confidence);
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
				attackData._BodyDimensions, *attackData.CurrentData, *attackData.PreviousData, extraCustomData);
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

public:
	AttackGesture(void);
	~AttackGesture(void);

	///<summary>Adds an evaluator to this attack gesture</summary>
	///<param name="evaluator">The evaluator to add</param>
	inline void AddEvaluator(GestureEvaluator& evaluator){gestureEvaluators.push_back(evaluator);}
	
	///<summary>Adds an evaluator that uses a custom defined method to execute instructions on the received body data</summary>
	///<param name="timeToComplete">The time to complete the evaluator when it is the active one</param>
	///<param name="CustomEvaluator">The pointer to the static method that will evaluate the body data</param>
	inline void AddCustomEvaluator(float timeToComplete, GestureEnums::BodySide (*CustomEvaluator)
		(const Probender*, const BodyDimensions&, const CompleteData&, const CompleteData&, const ExtraCustomData& extraCustomData),
		GestureEnums::BodySide bodySide, GestureEnums::TransitionRules transitionFromPrevious = 
		GestureEnums::TRANRULE_NONE, void* extraData = NULL)
	{
		GestureEvaluator eval = GestureEvaluator(timeToComplete);
		eval.SetCustomEvaluator(CustomEvaluator, bodySide, extraData);

		gestureEvaluators.push_back(eval);
	}

	///<summary>Adds an evaluator that queries the Discrete Gesture data</summary>
	///<param name="timeToComplete">The time to complete the evaluator when it is the active one</param>
	///<param name="gestureName">The name of the gesture that the evaluator pertains to</param>
	///<param name="trueIfFirstFrame">Only true if this is the first frame the gesture has been detected, false if
	///any frame it can be true</param>
	///<param name="confidence">How confident the Kinect must be to register the gesture, between 0.0f and 1.0f</param>
	inline void AddDiscreteEvaluator(float timeToComplete, const std::string& gestureName, bool trueIfFirstFrame, float confidence )
	{
		GestureEvaluator eval = GestureEvaluator(timeToComplete);
		eval.SetDiscreteEvaluator(gestureName, trueIfFirstFrame, confidence);
		gestureEvaluators.push_back(eval);
	}

	///<summary>Adds an evaluator that queries the Discrete Gesture data</summary>
	///<param name="timeToComplete">The time to complete the evaluator when it is the active one</param>
	///<param name="gestureName">The name of the gesture that the evaluator pertains to</param>
	///<param name="progress">The progress through the gesture that must be reached before the evaluator registers true
	///Must be between 0.0f and 1.0f</param>
	inline void AddContinuousEvaluator(float timeToComplete, const std::string& gestureName, float progress)
	{
		GestureEvaluator eval = GestureEvaluator(timeToComplete);
		eval.SetContinuousEvaluator(gestureName, progress);
		gestureEvaluators.push_back(eval);
	}

	void Update(float gameTime);

	///<summary>Evaluates the kinect data and returns true if all gestures have returned true</summary>
	///<param name="attackData">The attack data to evaluate with</param>
	///<returns>True if completed the last requirement, false if not</returns>
	GestureEnums::BodySide Evaluate(const AttackData& attackData);

	inline void Reset(){currentIndex = 0; timePassed = 0.0f;}
};

