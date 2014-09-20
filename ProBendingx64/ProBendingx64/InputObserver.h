#pragma once
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class InputObserver
{
public:
	InputObserver(){}
	virtual ~InputObserver(void) {};

	virtual bool keyDown(const OIS::KeyEvent &arg){return true;}
	virtual bool keyPressed( const OIS::KeyEvent &arg ){return true;}
	virtual bool keyReleased( const OIS::KeyEvent &arg ){return true;}
	virtual bool mouseMoved( const OIS::MouseEvent &arg ){return true;}
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){return true;}
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){return true;}
};

