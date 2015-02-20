#pragma once
#include "KinectBody.h"
#include <functional>
#include <vector>
#include "KinectGestureReader.h"
#include "ProbenderData.h"

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

class GestureEvaluator
{
public:
	enum EvaluatorType{ET_NONE, ET_CUSTOM, ET_DISCRETE, ET_CONTINUOUS};

private:
	//A union of the possible types of function pointers
	union Evaluators
	{
		bool (*CustomEvaluator)(const Probender*, const BodyDimensions&, const CompleteData&, const CompleteData&);
		bool (*DiscreteEvaluator)(const std::vector<KinectGestureResult>&);
		bool (*ContinuousEvaluator)(const std::vector<KinectGestureResult>&);
	};

	EvaluatorType evaluatorType;
	Evaluators evaluator;

public:
	//The amount of time that is allowed to pass before the gesture should be considered reset
	float TimeToComplete;

	GestureEvaluator(float _timeToComplete = 0.0f):evaluatorType(ET_NONE), TimeToComplete(_timeToComplete)
	{}

	~GestureEvaluator(){}

	inline void SetCustomEvaluator(bool(*customEvaluator)(const Probender*, const BodyDimensions&, 
		const CompleteData&, const CompleteData&))
	{
		evaluator.CustomEvaluator = customEvaluator;
		evaluatorType = ET_CUSTOM;
	}

	inline void SetDiscreteEvaluator(bool (*discreteEvaluator)(const std::vector<KinectGestureResult>&))
	{
		evaluator.DiscreteEvaluator = discreteEvaluator;
		evaluatorType = ET_DISCRETE;
	}

	inline void SetContinuousEvaluator(bool (*continuousEvaluator)(const std::vector<KinectGestureResult>&))
	{
		evaluator.ContinuousEvaluator = continuousEvaluator;
		evaluatorType = ET_CONTINUOUS;
	}

	///<summary>Evaluates the passed data using the function that was given on creation</summary>
	///<param name="attackData">The attack data to evaluate</param>
	///<returns>The result of the stored functions analysis of the data, or false if the data wasn't of the required type</returns>
	bool Evaluate(const AttackData& attackData)
	{
		switch (evaluatorType)
		{
		case GestureEvaluator::ET_NONE:
			return false;
			break;
		case GestureEvaluator::ET_CUSTOM:
			if(attackData.CurrentData == NULL || attackData.PreviousData == NULL)
				return false;
			return evaluator.CustomEvaluator(attackData._Probender, 
				attackData._BodyDimensions, *attackData.CurrentData, *attackData.PreviousData);
			break;
		case GestureEvaluator::ET_DISCRETE:
			if(attackData.DiscreteGestureResults == NULL)
				return false;
			return evaluator.DiscreteEvaluator(*attackData.DiscreteGestureResults);
			break;
		case GestureEvaluator::ET_CONTINUOUS:
			if(attackData.ContinuousGestureResults == NULL)
				return false;
			return evaluator.ContinuousEvaluator(*attackData.ContinuousGestureResults);
			break;
		default:
			return false;
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

public:
	AttackGesture(void);
	~AttackGesture(void);

	inline void AddEvaluator(GestureEvaluator& evaluator){gestureEvaluators.push_back(evaluator);}
	
	inline void AddCustomEvaluator(float timeToComplete, bool (*CustomEvaluator)
		(const Probender*, const BodyDimensions&, const CompleteData&, const CompleteData&))
	{
		GestureEvaluator eval = GestureEvaluator(timeToComplete);
		eval.SetCustomEvaluator(CustomEvaluator);

		gestureEvaluators.push_back(eval);
	}

	inline void AddDiscreteEvaluator(float timeToComplete, bool (*DiscreteEvaluator)(const std::vector<KinectGestureResult>&))
	{
		GestureEvaluator eval = GestureEvaluator(timeToComplete);
		eval.SetDiscreteEvaluator(DiscreteEvaluator);

		gestureEvaluators.push_back(eval);
	}

	inline void AddContinuousEvaluator(float timeToComplete, bool (*ContinuousEvaluator)(const std::vector<KinectGestureResult>&))
	{
		GestureEvaluator eval = GestureEvaluator(timeToComplete);
		eval.SetContinuousEvaluator(ContinuousEvaluator);

		gestureEvaluators.push_back(eval);
	}

	bool Evaluate(const AttackData& attackData);
};

