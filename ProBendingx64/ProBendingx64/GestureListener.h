#pragma once
#include <Kinect.VisualGestureBuilder.h>

struct GestureData
{
	IGesture* Gesture;
	GestureType GestureType;
	union GestureResult
	{
		IDiscreteGestureResult* discreteGestureResult;
		IContinuousGestureResult* continuousGestureResult;
	};
};

class GestureListener
{
public:
	GestureListener(void);
	~GestureListener(void);
};

