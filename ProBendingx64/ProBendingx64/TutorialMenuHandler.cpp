#include "TutorialMenuHandler.h"

#include "GUIManager.h"
#include "MenusScene.h"

#include "CEGUI/widgets/PushButton.h"

TutorialMenuHandler::TutorialMenuHandler(IScene* scene)
	: MenuHandler(scene), currentPage(NULL)
{
	GetRootWindows();
	RegisterButtonEvents();

	CreateTutorialPages();
	
	currDirectory = General;

	GetWindow(MW_TEXT)->setProperty("NormalTextColour","FFFFFF96");

	UpdateDisplay();
}

TutorialMenuHandler::~TutorialMenuHandler(void)
{

}

void TutorialMenuHandler::CreateTutorialPages()
{
	generalPages.reserve(8);

	//GENERAL PAGES SECTION
	unsigned int currPageIndex = 0;
	generalPages.push_back(TutorialPage());
	generalPages[currPageIndex].Text = "The following techniques are available to Probenders of any element.";

	//JAB PAGE
	generalPages.push_back(TutorialPage());
	generalPages[currPageIndex].Next = &generalPages[currPageIndex + 1];
	currPageIndex++;
	generalPages[currPageIndex].Previous = &generalPages[currPageIndex - 1];
	generalPages[currPageIndex].Text = "JAB\n Simply punch your arm straight out in front of you, while "
		"feeling the energy of the element course through your body and release out of your fist";
	
	//BLOCK PAGE
	generalPages.push_back(TutorialPage());
	generalPages[currPageIndex].Next = &generalPages[currPageIndex + 1];
	currPageIndex++;
	generalPages[currPageIndex].Previous = &generalPages[currPageIndex - 1];
	generalPages[currPageIndex].Text = "BRACE\n Place both arms vertically in front of your face to brace against "
		"oncoming enemy attacks. This prevents you from being pushed back as far, but takes more energy when hit";
	
	//DODGE PAGE
	generalPages.push_back(TutorialPage());
	generalPages[currPageIndex].Next = &generalPages[currPageIndex + 1];
	currPageIndex++;
	generalPages[currPageIndex].Previous = &generalPages[currPageIndex - 1];
	generalPages[currPageIndex].Text = "DODGE\n Lean in the desired direction to avoid danger";

	//JUMP PAGE
	generalPages.push_back(TutorialPage());
	generalPages[currPageIndex].Next = &generalPages[currPageIndex + 1];
	currPageIndex++;
	generalPages[currPageIndex].Previous = &generalPages[currPageIndex - 1];
	generalPages[currPageIndex].Text = "JUMP\n Avoid low-targeted attacks by jumping. Lift your feet as high as they can go";

	//EARTH PAGES SECTION
	currPageIndex = 0;
	earthPages.push_back(TutorialPage());
	earthPages[currPageIndex].Text = "These pages outline the movements performed by Earth-Benders to use their more powerful techniques";
	
	//EARTH COIN PAGE
	earthPages.push_back(TutorialPage());
	earthPages[currPageIndex].Next = &earthPages[currPageIndex + 1];
	currPageIndex++;
	earthPages[currPageIndex].Previous = &earthPages[currPageIndex - 1];
	earthPages[currPageIndex].Text = "KNEE STOMP\n Lift a knee above your waist and bring it back down to the ground. "
		"The earth coin will go to the hand on the same side as the knee that was lifted. Punch with the opposite hand to launch";

	//FIRE PAGES SECTION
	currPageIndex = 0;
	firePages.push_back(TutorialPage());
	firePages[currPageIndex].Text = "These pages outline the movements performed by Fire-Benders to use their more powerful techniques";

	//FIRE BLAST PAGE
	firePages.push_back(TutorialPage());
	firePages[currPageIndex].Next = &firePages[currPageIndex + 1];
	currPageIndex++;
	firePages[currPageIndex].Previous = &firePages[currPageIndex - 1];
	firePages[currPageIndex].Text = "FIRE BLAST\n Extend your arms out horizontally, and bring them back in front of your chest."
		"Keep your hands together as you move the fireball around. Launch by punching with both arms together";

	currPageIndex = 0;
	waterPages.push_back(TutorialPage());
	waterPages[currPageIndex].Text = "These pages outline the movements performed by Water-Benders to use their more powerful techniques";

}

void TutorialMenuHandler::UpdateDisplay()
{
	if(currentPage)
	{
		GetWindow(MW_TEXT)->setText(currentPage->Text);
		SetWindowImage(GetWindow(MW_IMAGE_1), currentPage->ImagesetName, currentPage->Images[0]);
		SetWindowImage(GetWindow(MW_IMAGE_2), currentPage->ImagesetName, currentPage->Images[1]);
		SetWindowImage(GetWindow(MW_IMAGE_3), currentPage->ImagesetName, currentPage->Images[2]);
		SetWindowImage(GetWindow(MW_IMAGE_4), currentPage->ImagesetName, currentPage->Images[3]);
		SetWindowImage(GetWindow(MW_IMAGE_5), currentPage->ImagesetName, currentPage->Images[4]);
	}

	GetWindow(MW_GENERAL)->getChild(0)->setProperty("NormalTextColour","FFFFFF96");
	GetWindow(MW_EARTH)->getChild(0)->setProperty("NormalTextColour","FFFFFF96");
	GetWindow(MW_FIRE)->getChild(0)->setProperty("NormalTextColour","FFFFFF96");
	GetWindow(MW_WATER)->getChild(0)->setProperty("NormalTextColour","FFFFFF96");

	if(currDirectory == General)
		GetWindow(MW_GENERAL)->getChild(0)->setProperty("NormalTextColour","FF00FF00");
	else if(currDirectory == Earth)
		GetWindow(MW_EARTH)->getChild(0)->setProperty("NormalTextColour","FF00FF00");
	else if(currDirectory == Fire)
		GetWindow(MW_FIRE)->getChild(0)->setProperty("NormalTextColour","FF00FF00");
	else if(currDirectory == Water)
		GetWindow(MW_WATER)->getChild(0)->setProperty("NormalTextColour","FF00FF00");

	if(atEnd)
		GetWindow(MW_NEXT)->getChild(0)->setText("Done");
	else
		GetWindow(MW_NEXT)->getChild(0)->setText("Next");
}

void TutorialMenuHandler::GetRootWindows()
{
	rootWindow = GetWindow(MW_ROOT_WINDOW);
	DialogRoot = GetWindow(MW_DIALOG_ROOT);
}

CEGUI::Window* TutorialMenuHandler::GetWindow(const MenuWindows window)
{
	switch (window)
	{
	case TutorialMenuHandler::MW_ROOT_WINDOW:
		return guiManager->GetChildWindow("TutorialRootWindow");
		break;
	case TutorialMenuHandler::MW_DIALOG_ROOT:
		return guiManager->GetChildWindow(rootWindow, "DialogBacking");
		break;
	case TutorialMenuHandler::MW_IMAGE_1:
		return guiManager->GetChildWindow(DialogRoot, "Image1");
		break;
	case TutorialMenuHandler::MW_IMAGE_2:
		return guiManager->GetChildWindow(DialogRoot, "Image2");
		break;
	case TutorialMenuHandler::MW_IMAGE_3:
		return guiManager->GetChildWindow(DialogRoot, "Image3");
		break;
	case TutorialMenuHandler::MW_IMAGE_4:
		return guiManager->GetChildWindow(DialogRoot, "Image4");
		break;
	case TutorialMenuHandler::MW_IMAGE_5:
		return guiManager->GetChildWindow(DialogRoot, "Image5");
		break;
	case TutorialMenuHandler::MW_TEXT:
		return guiManager->GetChildWindow(DialogRoot, "Text");
		break;
	case TutorialMenuHandler::MW_NEXT:
		return guiManager->GetChildWindow(DialogRoot, "NextButton");
		break;
	case TutorialMenuHandler::MW_PREV:
		return guiManager->GetChildWindow(DialogRoot, "PreviousButton");
		break;
	case TutorialMenuHandler::MW_GENERAL:
		return guiManager->GetChildWindow(DialogRoot, "GeneralPage");
		break;
	case TutorialMenuHandler::MW_EARTH:
		return guiManager->GetChildWindow(DialogRoot, "EarthPage");
		break;
	case TutorialMenuHandler::MW_FIRE:
		return guiManager->GetChildWindow(DialogRoot, "FirePage");
		break;
	case TutorialMenuHandler::MW_WATER:
		return guiManager->GetChildWindow(DialogRoot, "WaterPage");
		break;
	case TutorialMenuHandler::MW_EXIT:
		return guiManager->GetChildWindow(DialogRoot, "ExitButton");
		break;
	default:
		return NULL;
		break;
	}
}

void TutorialMenuHandler::RegisterButtonEvents()
{
	MenusScene* menu = (MenusScene*)scene;

	GetWindow(MW_NEXT)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&TutorialMenuHandler::NextBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_NEXT));

	GetWindow(MW_PREV)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&TutorialMenuHandler::PrevBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PREV));

	GetWindow(MW_EXIT)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&TutorialMenuHandler::ExitBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_EXIT));

	GetWindow(MW_GENERAL)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&TutorialMenuHandler::PageBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_GENERAL));

	GetWindow(MW_EARTH)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&TutorialMenuHandler::PageBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_EARTH));

	GetWindow(MW_FIRE)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&TutorialMenuHandler::PageBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_FIRE));

	GetWindow(MW_WATER)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&TutorialMenuHandler::PageBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_WATER));
}

void TutorialMenuHandler::UnregisterButtonEvents()
{
	MenusScene* menu = (MenusScene*)scene;

	menu->UnregisterHoverEvents(GetWindow(MW_NEXT));
	menu->UnregisterHoverEvents(GetWindow(MW_PREV));
}

bool TutorialMenuHandler::NextBtnClickEvent(const CEGUI::EventArgs& e)
{
	CEGUI::WindowEventArgs& windowEvent = (CEGUI::WindowEventArgs&)e;
	
	if(atEnd)
		((MenusScene*)scene)->SetScreen(MenusScene::GameSetup);

	if(currentPage->Next)
		currentPage = currentPage->Next;

	atEnd = currentPage->Next == NULL;

	UpdateDisplay();

	return true;
}

bool TutorialMenuHandler::PrevBtnClickEvent(const CEGUI::EventArgs& e)
{
	CEGUI::WindowEventArgs& windowEvent = (CEGUI::WindowEventArgs&)e;

	if(currentPage->Previous)
	{
		currentPage = currentPage->Previous;
		atEnd = false;
	}
	
	UpdateDisplay();

	return true;
}

void TutorialMenuHandler::Update(float gameTime)
{
}

void TutorialMenuHandler::ReceiveAudioInput(const AudioData* audioText)
{
}

void TutorialMenuHandler::Show()
{
	MenuHandler::Show();

	atEnd = false;
	currentPage = &generalPages[0];

	UpdateDisplay();
}

bool TutorialMenuHandler::ExitBtnClickEvent(const CEGUI::EventArgs& e)
{
	((MenusScene*)scene)->SetScreen(MenusScene::GameSetup);
	return true;
}

bool TutorialMenuHandler::PageBtnClickEvent(const CEGUI::EventArgs& e)
{
	CEGUI::WindowEventArgs& w = (CEGUI::WindowEventArgs&)e;

	const CEGUI::String& windowName = w.window->getName();

	if(windowName == "GeneralPage")
	{
		currentPage = &generalPages[0];
		currDirectory = General;
	}
	else if(windowName == "EarthPage")
	{
		currentPage = &earthPages[0];
		currDirectory = Earth;
	}
	else if(windowName == "FirePage")
	{
		currentPage = &firePages[0];
		currDirectory = Fire;
	}
	else if(windowName == "WaterPage")
	{
		currentPage = &waterPages[0];
		currDirectory = Water;
	}

	atEnd = currentPage->Next == NULL;

	UpdateDisplay();

	return true;
}