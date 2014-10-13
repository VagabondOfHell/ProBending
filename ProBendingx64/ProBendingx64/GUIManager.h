#pragma once
#include <CEGUI\CEGUI.h>
#include <CEGUI\RendererModules\Ogre\Renderer.h>

#include "InputObserver.h"

class GUIManager: public InputObserver
{
private:
	static CEGUI::OgreRenderer* mRenderer;

	CEGUI::MouseButton ConvertButton(OIS::MouseButtonID buttonID);

	CEGUI::Window* rootWindow;

public:
	////////////////////////TODO///////////////////////////////////
	///Register to listen to CEGUI control events when pushing a window

	GUIManager(void);
	virtual ~GUIManager(void);
	
	static void BootstrapSystem();
	static void DestroySystem();

	void InitializeGUI();	
	void Update(float gameTime);

	///<summary>Adds a scheme to the Scheme Manager for reference</summary>
	///<param "schemeFileName">The filename of the scheme to load, including the extension</param>
	///<returns>True if successful, false if not</returns>
	bool AddScheme(const CEGUI::String& schemeFileName);

	///<summary>Adds a window to the GUI Manager</summary>
	///<param "layoutFileName">The filename of the layout to load, including the extension</param>
	///<param "windowName">A name to assign to the window. If none are specified, it uses the name listed in the 
	///xml file that the layout is being loaded from</param>
	///<param "schemeFileName">The scheme file name to load from. This is for convenience, allowing layout and scheme
	///to be loaded at the same time</param>
	///<returns>True if successful, false if not</returns>
	bool AddWindow(const CEGUI::String& layoutFileName, const CEGUI::String& windowName = "", const CEGUI::String& schemeFileName = "");
	
	///<summary>Removes and deletes the specified window</summary>
	///<param "windowName">The name of the window to delete</param>
	///<returns>True if successful, false if not</returns>
	bool DestroyWindow(const CEGUI::String& windowName);

	///<summary>Gets the element of the specified path</summary>
	///<param "elementPath">The path of the element to get</param>
	///<returns>The element if found, NULL if not</returns>
	CEGUI::NamedElement* GetChildItem(const CEGUI::String& elementPath);

	///<summary>Removes a scheme from CEGUI. Unfortunately there is no way to know if this 
	///was successful or not from CEGUI's end. Though there aren't too many instances that it would
	///be necessary to call this function anyways</summary>
	///<param "schemeName">The name of the scheme to remove</param>
	void RemoveScheme(const CEGUI::String& schemeName);

	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
};

