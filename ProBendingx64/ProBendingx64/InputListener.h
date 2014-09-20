#pragma once
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class InputListener:public OIS::MouseListener, public OIS::KeyListener
{
private:
	
public:
	InputListener()
	{}

	virtual ~InputListener(){}

	virtual bool keyPressed( const OIS::KeyEvent &arg ) = 0;
	virtual bool keyReleased( const OIS::KeyEvent &arg ) = 0;
	virtual bool mouseMoved( const OIS::MouseEvent &arg ) = 0;
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) = 0;
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) = 0;
};