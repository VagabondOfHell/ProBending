#pragma once
#include <CEGUI\CEGUI.h>
#include <CEGUI\RendererModules\Ogre\Renderer.h>

#include "InputObserver.h"

class GUIManager: public InputObserver
{
private:
	GUIManager(void);
	CEGUI::OgreRenderer* mRenderer;
	CEGUI::MouseButton ConvertButton(OIS::MouseButtonID buttonID);

public:

	virtual ~GUIManager(void);

	static GUIManager* GetInstance();
	static void DestroySingleton();
	
	bool InitializeGUI();	

	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
};

