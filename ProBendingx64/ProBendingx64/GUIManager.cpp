#include "GUIManager.h"

#include <CEGUI\CEGUI.h>

CEGUI::OgreRenderer* GUIManager::mRenderer;

GUIManager::GUIManager(void)
{
	rootWindow = NULL;
	mRenderer->beginRendering();
}


GUIManager::~GUIManager(void)
{
	for (int i = 0; i < rootWindow->getChildCount(); i++)
	{
		rootWindow->getChildAtIdx(i)->removeAllEvents();

		CEGUI::WindowManager::getSingleton().destroyWindow(rootWindow->getChildAtIdx(i));
	}
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
	defaultContext = &CEGUI::System::getSingleton().getDefaultGUIContext();

    defaultContext->getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
	
	CEGUI::WindowManager* wmgr = CEGUI::WindowManager::getSingletonPtr();
	
	rootWindow = wmgr->createWindow("DefaultWindow", "_MasterRoot");
	defaultContext->setRootWindow(rootWindow);
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

CEGUI::Window* GUIManager::LoadLayoutFile(const CEGUI::String& layoutFileName)
{
	CEGUI::Window* retVal;

	try
	{
		retVal = CEGUI::WindowManager::getSingleton().loadLayoutFromFile(layoutFileName);
	}
	catch(CEGUI::Exception e)
	{
		printf(e.getMessage().c_str());
		return NULL;
	}

	return retVal;
}

bool GUIManager::LoadLayout(const CEGUI::String& layoutFileName, const CEGUI::String& windowName, const CEGUI::String& schemeFileName)
{
	CEGUI::Window* newWindow = NULL;
	try
	{
		if(!schemeFileName.empty())
			CEGUI::SchemeManager::getSingleton().createFromFile(schemeFileName);
		
		newWindow = LoadLayoutFile(layoutFileName);
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

bool GUIManager::LoadLayout(const CEGUI::String& layoutFileName, bool setAsRoot /*= true*/)
{
	CEGUI::Window* newWindow = LoadLayoutFile(layoutFileName);

	if(newWindow)
	{
		if(setAsRoot)
			defaultContext->setRootWindow(newWindow);
		else
			defaultContext->getRootWindow()->addChild(newWindow);

		return true;
	}

	return false;
}

bool GUIManager::LoadLayout(const CEGUI::String& layoutFileName, CEGUI::Window* parentWindow)
{
	CEGUI::Window* newWindow = LoadLayoutFile(layoutFileName);

	if(newWindow)
	{
		if(parentWindow)
			parentWindow->addChild(newWindow);
		else
			defaultContext->getRootWindow()->addChild(newWindow);

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

CEGUI::Window* GUIManager::GetChildWindow(const CEGUI::String& windowPath)
{
	CEGUI::Window* returnElement = NULL;

	try
	{
		returnElement = rootWindow->getChild(windowPath);
	}
	catch(CEGUI::Exception e)
	{
		printf(e.what());
		printf(e.getMessage().c_str());

		returnElement = NULL;
	}

	return returnElement;
}

CEGUI::Window* GUIManager::GetChildWindow(const CEGUI::Window* const searchStartWindow, const CEGUI::String& pathFromPassedWindow)
{
	CEGUI::Window* returnElement = NULL;

	try
	{
		returnElement = searchStartWindow->getChild(pathFromPassedWindow);
	}
	catch(CEGUI::Exception e)
	{
		printf(e.what());
		printf(e.getMessage().c_str());

		returnElement = NULL;
	}

	return returnElement;
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
		printf(e.what());
		printf(e.getMessage().c_str());

		returnElement = NULL;
	}
	
	return returnElement;
}

void GUIManager::RemoveScheme(const CEGUI::String& schemeName)
{
	CEGUI::SchemeManager::getSingleton().destroy(schemeName);
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

CEGUI::PushButton* const GUIManager::CreateGUIButton(const CEGUI::String& style, 
		const CEGUI::String& buttonName, const CEGUI::String& buttonText, const CEGUI::UVector2& position, const CEGUI::USize& size)
{
	CEGUI::Window* button = CEGUI::WindowManager::getSingletonPtr()->createWindow(style, buttonName);
	
	if(button)
	{
		button->setText(buttonText);
		button->setSize(size);
		button->setPosition(position);
		rootWindow->addChild(button);

		return static_cast<CEGUI::PushButton*>(button);
	}
	
	return NULL;
}

void GUIManager::InjectMousePosition(float x, float y)
{
	defaultContext->injectMousePosition(x, y);
}

void GUIManager::InjectMouseMove(float x, float y)
{
	defaultContext->injectMouseMove(x, y);
}

void GUIManager::InjectMouseClick(OIS::MouseButtonID button)
{
	if(!defaultContext->injectMouseButtonClick(ConvertButton(button)))
		printf("NO HANDLE\n");
	else
		printf("HANDLED\n");
}

void GUIManager::InjectMouseButtonDown(OIS::MouseButtonID button)
{
	defaultContext->injectMouseButtonDown(ConvertButton(button));
}

void GUIManager::InjectMouseButtonUp(OIS::MouseButtonID button)
{
	defaultContext->injectMouseButtonUp(ConvertButton(button));
}
