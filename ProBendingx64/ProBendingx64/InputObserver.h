#pragma once
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class InputObserver
{
	friend class InputNotifier;

public:
	//True to be notified of events, false to not. This is useful for 
	//temporarily disabling input notification without removing the listener.
	bool Enabled;

	InputObserver(){Enabled = true;}
	virtual ~InputObserver(void) {};

protected:
	virtual void keyDown(const OIS::KeyEvent &arg){}
	virtual void keyPressed( const OIS::KeyEvent &arg ){}
	virtual void keyReleased( const OIS::KeyEvent &arg ){}
	virtual void mouseMoved( const OIS::MouseEvent &arg ){}
	virtual void mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){}
	virtual void mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){}
};

