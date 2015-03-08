#pragma once
#include "KinectBodyListener.h"
#include "InputObserver.h"

class MenusScene;

class MenuNavigator :
	public KinectBodyListener, public InputObserver
{
private:
	MenusScene* menuScene;

public:
	MenuNavigator(MenusScene* menuScene = NULL);
	~MenuNavigator(void);

	virtual void BodyLost(const CompleteData& currentData, const CompleteData& previousData);
	
	virtual void BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData);

	virtual void mouseMoved(const OIS::MouseEvent &arg);

	virtual void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	virtual void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

};

