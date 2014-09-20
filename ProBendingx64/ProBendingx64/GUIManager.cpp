#include "GUIManager.h"

static GUIManager* instance;

GUIManager::GUIManager(void)
{
}


GUIManager::~GUIManager(void)
{
	mRenderer->destroySystem();
}

GUIManager* GUIManager::GetInstance()
{
	if(!instance)
		instance = new GUIManager();

	return instance;
}

void GUIManager::DestroySingleton()
{
	if(instance)
	{
		delete instance;
		instance = nullptr;
	}
}

bool GUIManager::InitializeGUI()
{
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
 
    CEGUI::ImageManager::setImagesetDefaultResourceGroup("ImageSets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
 
    CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
 
    CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
 
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");
 
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);

	//If successful, this will return true
	if(mRenderer)
		return true;
	else
		return false;
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