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
	
	static void BootstrapSystem(Ogre::RenderTarget* renderTarget);
	static void DestroySystem();

	void InitializeGUI();	
	void Update(float gameTime);

	inline CEGUI::Window* GetRootWindow()const {return rootWindow;}

	///<summary>Adds a scheme to the Scheme Manager for reference</summary>
	///<param name="schemeFileName">The filename of the scheme to load, including the extension</param>
	///<returns>True if successful, false if not</returns>
	bool AddScheme(const CEGUI::String& schemeFileName);

	///<summary>Adds a window to the GUI Manager</summary>
	///<param name="layoutFileName">The filename of the layout to load, including the extension</param>
	///<param name="windowName">A name to assign to the window. If none are specified, it uses the name listed in the 
	///xml file that the layout is being loaded from</param>
	///<param name="schemeFileName">The scheme file name to load from. This is for convenience, allowing layout and scheme
	///to be loaded at the same time</param>
	///<returns>True if successful, false if not</returns>
	bool LoadLayout(const CEGUI::String& layoutFileName, const CEGUI::String& windowName = "", const CEGUI::String& schemeFileName = "");
	

	///<summary>Creates a push button as a child of the root window</summary>
	///<param name="style">The style to use. An Example is "TaharezLook/Button". This is passed to the CEGUI Window Factory</param>
	///<param name="buttonName">The name to apply to the button. Must be unique. If NULL, CEGUI Generates a name for it</param>
	///<param name="buttonText">The text for the button to display</param>
	///<param name="position">The position of the button</param>
	///<param name="size">The size of the button</param>
	///<returns>The CEGUI Push Button if successful, or NULL if failed</returns>
	CEGUI::PushButton* const CreateGUIButton(const CEGUI::String& style, const CEGUI::String& buttonName, 
		const CEGUI::String& buttonText, const CEGUI::UVector2& position, const CEGUI::USize& size);

	///<summary>Removes and deletes the specified window</summary>
	///<param name="windowName">The name of the window to delete</param>
	///<returns>True if successful, false if not</returns>
	bool DestroyWindow(const CEGUI::String& windowName);

	///<summary>Gets the element of the specified path</summary>
	///<param name="elementPath">The path of the element to get</param>
	///<returns>The element if found, NULL if not</returns>
	CEGUI::NamedElement* GetChildItem(const CEGUI::String& elementPath);

	///<summary>Removes a scheme from CEGUI. Unfortunately there is no way to know if this 
	///was successful or not from CEGUI's end. Though there aren't too many instances that it would
	///be necessary to call this function anyways</summary>
	///<param name="schemeName">The name of the scheme to remove</param>
	void RemoveScheme(const CEGUI::String& schemeName);

	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
};

