#include "GUIManager.h"

CEGUI::OgreRenderer* GUIManager::mRenderer;

GUIManager::GUIManager(void)
{
	rootWindow = NULL;
}


GUIManager::~GUIManager(void)
{
}

void GUIManager::BootstrapSystem(Ogre::RenderTarget* renderTarget)
{
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem(*renderTarget);
}

void GUIManager::DestroySystem()
{
	mRenderer->destroySystem();	
}

void GUIManager::InitializeGUI()
{
    CEGUI::ImageManager::setImagesetDefaultResourceGroup("ImageSets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	
    CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
 
    CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
 
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    
	rootWindow = wmgr.createWindow("DefaultWindow", "RootWindow");

	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);

}

void GUIManager::Update(float gameTime)
{
	CEGUI::System::getSingleton().injectTimePulse(gameTime);
}

bool GUIManager::AddScheme(const CEGUI::String& schemeFileName)
{
	try
	{
		CEGUI::SchemeManager::getSingleton().createFromFile(schemeFileName);
		return true;
	}
	catch (CEGUI::Exception e)
	{
		printf(e.getMessage().c_str());
	}

	return false;
}

bool GUIManager::AddWindow(const CEGUI::String& layoutFileName, const CEGUI::String& windowName, const CEGUI::String& schemeFileName)
{
	CEGUI::Window* newWindow = NULL;
	try
	{
		if(!schemeFileName.empty())
			CEGUI::SchemeManager::getSingleton().createFromFile(schemeFileName);

		newWindow = CEGUI::WindowManager::getSingleton().loadLayoutFromFile(layoutFileName);
	}
	catch(CEGUI::Exception e)
	{
		printf(e.getMessage().c_str());
	}

	if(newWindow)
	{
		if(!windowName.empty())
			newWindow->setName(windowName);

		rootWindow->addChild(newWindow);

		return true;
	}

	return false;
}

bool GUIManager::DestroyWindow(const CEGUI::String& windowName)
{
	CEGUI::Window* removeWindow = NULL;

	removeWindow = rootWindow->getChild(windowName);
		
	if(removeWindow)
	{
		CEGUI::WindowManager::getSingleton().destroyWindow(removeWindow);
		return true;
	}

	return false;
}

CEGUI::NamedElement* GUIManager::GetChildItem(const CEGUI::String& elementPath)
{
	CEGUI::NamedElement* returnElement = NULL;
	
	try
	{
		returnElement = rootWindow->getChildElement(elementPath);
	}
	catch(CEGUI::Exception e)
	{
		returnElement = NULL;
	}
	
	return returnElement;
}

void GUIManager::RemoveScheme(const CEGUI::String& schemeName)
{
	CEGUI::SchemeManager::getSingleton().destroy(schemeName);
}

bool GUIManager::mouseMoved( const OIS::MouseEvent &arg )
{
	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	context.injectMouseMove(arg.state.X.rel, arg.state.Y.rel);

	if(arg.state.Z.rel)
		context.injectMouseWheelChange(arg.state.Z.rel / 120.0f);
	
	return true;
}

bool GUIManager::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(ConvertButton(id));
	
	return true;
}

bool GUIManager::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(ConvertButton(id));

	return true;
}

CEGUI::MouseButton GUIManager::ConvertButton(OIS::MouseButtonID buttonID)
{
	switch(buttonID)
	{
	case OIS::MB_Left:
		return CEGUI::LeftButton;

	case OIS::MB_Right:
		return CEGUI::RightButton;

	case OIS::MB_Middle:
		return CEGUI::MiddleButton;

	default:
		return CEGUI::LeftButton;
	}
}