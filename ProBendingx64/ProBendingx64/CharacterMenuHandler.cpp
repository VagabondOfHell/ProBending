#include "CharacterMenuHandler.h"

#include "GUIManager.h"
#include "CEGUI/Window.h"
#include "CEGUI/widgets/PushButton.h"

#include "MenusScene.h"

const std::string CharacterMenuHandler::ROOT_WINDOW_NAME = "CharacterSetupRoot";
const std::string CharacterMenuHandler::STAT_DIALOG_ROOT = "StatsRootWindow";
const std::string CharacterMenuHandler::LOAD_DIALOG_ROOT = "LoadDialog";
const std::string CharacterMenuHandler::MAIN_DIALOG_ROOT = "LoadOrCreateRootWindow";

const std::string CharacterMenuHandler::P2_PREFIX = "P2"; const std::string CharacterMenuHandler::P1_PREFIX = "P1";

const std::string CharacterMenuHandler::ATTRIBUTE_SUFFIX = "AttributeRoot";
const std::string CharacterMenuHandler::MINUS_BTN_SUFFIX = "MinusButton";
const std::string CharacterMenuHandler::ADD_BTN_SUFFIX = "AddButton";
const std::string CharacterMenuHandler::VALUE_SUFFIX = "Value";


CharacterMenuHandler::CharacterMenuHandler(IScene* scene)
	:MenuHandler(scene), p1CurrentStage(Stage::MainDialog), p2CurrentStage(Stage::MainDialog)
{
	MenusScene* menu = (MenusScene*)scene;

	p1PointsAvailable = p2PointsAvailable = 0;
	p1ValidCharacter = p2ValidCharacter = false;

	p1ModdedData.MainElement = ElementEnum::Earth;
	p2ModdedData.MainElement = ElementEnum::Earth;

	GatherRootWindows();

	DisableUnimplementedControls();

	ChangeElement(true, menu->Player1Data.MainElement);
	ChangeElement(false, menu->Player2Data.MainElement);

	SetStage(true, p1CurrentStage);
	SetStage(false, p2CurrentStage);

	SubscribeToEvents();
}

CharacterMenuHandler::~CharacterMenuHandler(void)
{
}

void CharacterMenuHandler::DisableUnimplementedControls()
{
	GetWindow(MW_MAIN_LOAD_CHAR_BTN, true)->disable();
	GetWindow(MW_MAIN_LOAD_CHAR_BTN, false)->disable();

	GetWindow(MW_MAIN_EDIT_CHAR_BTN, true)->disable();
	GetWindow(MW_MAIN_EDIT_CHAR_BTN, false)->disable();

}

void CharacterMenuHandler::GatherRootWindows()
{
	rootWindow = GetWindow(MW_ROOT);

	p1MainDialog = GetWindow(MW_MAIN_DLG_ROOT);	p2MainDialog = GetWindow(MW_MAIN_DLG_ROOT, false);

	p1LoadDialog = GetWindow(MW_LOAD_DLG_ROOT);	p2LoadDialog = GetWindow(MW_LOAD_DLG_ROOT, false);

	p1StatsDialog = GetWindow(MW_STAT_DLG_ROOT); p2StatsDialog = GetWindow(MW_STAT_DLG_ROOT, false);

	p1NameBacking = GetWindow(MW_NAME_BACKING); p2NameBacking = GetWindow(MW_NAME_BACKING, false);
}

void CharacterMenuHandler::SubscribeToEvents()
{
	MenusScene* menu = (MenusScene*)scene;

	GetWindow(MW_ELEMENT_SEL_LEFT)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::L_ElementButtonClickEvent,this));
	GetWindow(MW_ELEMENT_SEL_RIGHT)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::R_ElementButtonClickEvent,this));
	GetWindow(MW_ELEMENT_SEL_LEFT, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::L_ElementButtonClickEvent,this));
	GetWindow(MW_ELEMENT_SEL_RIGHT, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::R_ElementButtonClickEvent,this));
	
	menu->RegisterHoverBeginEvent(GetWindow(MW_ELEMENT_SEL_LEFT));
	menu->RegisterHoverBeginEvent(GetWindow(MW_ELEMENT_SEL_RIGHT));
	menu->RegisterHoverEndEvent(GetWindow(MW_ELEMENT_SEL_LEFT));
	menu->RegisterHoverEndEvent(GetWindow(MW_ELEMENT_SEL_RIGHT));
	
	menu->RegisterHoverBeginEvent(GetWindow(MW_ELEMENT_SEL_LEFT, false));
	menu->RegisterHoverBeginEvent(GetWindow(MW_ELEMENT_SEL_RIGHT, false));
	menu->RegisterHoverEndEvent(GetWindow(MW_ELEMENT_SEL_LEFT, false));
	menu->RegisterHoverEndEvent(GetWindow(MW_ELEMENT_SEL_RIGHT, false));

	GetWindow(MW_MAIN_CREATE_CHAR_BTN)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::MainDlgCreateCharBtnClickEvent, this));
	GetWindow(MW_MAIN_EDIT_CHAR_BTN)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::MainDlgEditCharBtnClickEvent, this));
	GetWindow(MW_MAIN_LOAD_CHAR_BTN)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::MainDlgLoadCharBtnClickEvent, this));
	GetWindow(MW_MAIN_DONE_BTN)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::MainDlgDoneBtnClickEvent, this));

	menu->RegisterHoverBeginEvent(GetWindow(MW_MAIN_CREATE_CHAR_BTN));


	GetWindow(MW_MAIN_CREATE_CHAR_BTN, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::MainDlgCreateCharBtnClickEvent, this));
	GetWindow(MW_MAIN_EDIT_CHAR_BTN, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::MainDlgEditCharBtnClickEvent, this));
	GetWindow(MW_MAIN_LOAD_CHAR_BTN, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::MainDlgLoadCharBtnClickEvent, this));
	GetWindow(MW_MAIN_DONE_BTN, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::MainDlgDoneBtnClickEvent, this));

	//This works if there are no changes between these values. Used to cheat so I don't have to write all of these out for
	//each player
	int innerIndex = 0;
	for (int i = MW_STATS_ENDUR_MINUS; i < MW_STATS_LUCK_VAL; i++)
	{
		if(innerIndex % 2 == 0)
		{
			GetWindow((MenuWindows)i)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
				(&CharacterMenuHandler::StatsMinusBtnClickEvent, this));
			GetWindow((MenuWindows)i, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
				(&CharacterMenuHandler::StatsMinusBtnClickEvent, this));
		}
		else
		{
			GetWindow((MenuWindows)i)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
				(&CharacterMenuHandler::StatsAddBtnClickEvent, this));
			GetWindow((MenuWindows)i, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
				(&CharacterMenuHandler::StatsAddBtnClickEvent, this));
			
			++i;//Skip VAL
			innerIndex++;
		}

		innerIndex = (innerIndex + 1) % 3;
	}

	GetWindow(MW_STATS_OK_BTN)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::StatsOKBtnClickEvent, this));
	GetWindow(MW_STATS_CANCEL_BTN)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::StatsCancelBtnClickEvent, this));
	GetWindow(MW_STATS_OK_BTN, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::StatsOKBtnClickEvent, this));
	GetWindow(MW_STATS_CANCEL_BTN, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&CharacterMenuHandler::StatsCancelBtnClickEvent, this));
}

void CharacterMenuHandler::Update(float gameTime)
{
}

void CharacterMenuHandler::Show()
{
	MenuHandler::Show();
	SetStage(true, MainDialog);
	SetStage(false, MainDialog);
}

void CharacterMenuHandler::Hide()
{
	MenuHandler::Hide();
}

void CharacterMenuHandler::SetStage(bool player1, Stage newStage)
{
	switch (newStage)
	{
	case CharacterMenuHandler::Done:
	case CharacterMenuHandler::MainDialog:
		DeactivateElementChoices(player1);
		if(player1)
		{
			p1MainDialog->setVisible(true);
			p1LoadDialog->setVisible(false);
			p1StatsDialog->setVisible(false);
		}
		else
		{
			p2MainDialog->setVisible(true);
			p2LoadDialog->setVisible(false);
			p2StatsDialog->setVisible(false);
		}
		break;
	case CharacterMenuHandler::EditStats:
		ActivateElementChoices(player1);

		if(player1)
		{
			p1MainDialog->setVisible(false);
			p1LoadDialog->setVisible(false);
			p1StatsDialog->setVisible(true);
		}
		else
		{
			p2MainDialog->setVisible(false);
			p2LoadDialog->setVisible(false);
			p2StatsDialog->setVisible(true);
		}

		UpdateAttributeScores(player1);
		break;
	case CharacterMenuHandler::CreateStats:
		CreateCharacter(player1);
		ActivateElementChoices(player1);
		if(player1)
		{
			p1MainDialog->setVisible(false);
			p1LoadDialog->setVisible(false);
			p1StatsDialog->setVisible(true);
		}
		else
		{
			p2MainDialog->setVisible(false);
			p2LoadDialog->setVisible(false);
			p2StatsDialog->setVisible(true);
		}

		ChangeElement(player1, ElementEnum::InvalidElement);
		UpdateAttributeScores(player1);

		break;
	case CharacterMenuHandler::LoadDialog:
		DeactivateElementChoices(player1);
		if(player1)
		{
			p1MainDialog->setVisible(false);
			p1LoadDialog->setVisible(true);
			p1StatsDialog->setVisible(false);
		}
		else
		{
			p2MainDialog->setVisible(false);
			p2LoadDialog->setVisible(true);
			p2StatsDialog->setVisible(false);
		}
		break;
	}

	if(player1)
		p1CurrentStage = newStage;
	else
		p2CurrentStage = newStage;

	ChangeElement(player1, ElementEnum::InvalidElement);
}

void CharacterMenuHandler::ChangeElement(bool player1, ElementEnum::Element newElement)
{
	MenusScene* menuScene = (MenusScene*)scene;

	if(newElement == ElementEnum::InvalidElement)
	{
		if(player1)
			newElement = p1ModdedData.MainElement;
		else
			newElement = p2ModdedData.MainElement;
	}

	//Get the stage for the specified player
	Stage currentStage;
	if(player1)
		currentStage = p1CurrentStage;
	else
		currentStage = p2CurrentStage;

	//Update the visible buttons
	SetSkinsForGroup(player1, currentStage, newElement);

	//Get the element prefix for accessing controls
	std::string elementPrefix = GetElementPrefix(newElement);
	GetWindow(MW_ELEMENT_TEXT, player1)->setText(ElementEnum::EnumToString(newElement));
	//Set image for the name backing
	GetWindow(MW_NAME_BACKING, player1)->setProperty("Image", "MenuControls/"+elementPrefix+"Lrg_Frame");

	//Set the image for the two element controls
	SetButtonImage(GetWindow(MW_ELEMENT_SEL_LEFT, player1), BTN_SKIN_LEFT_BTN_HORZ, newElement);
	SetButtonImage(GetWindow(MW_ELEMENT_SEL_RIGHT, player1), BTN_SKIN_RIGHT_BTN_HORZ, newElement);
}

void CharacterMenuHandler::SetSkinsForGroup(bool player1, Stage stage, ElementEnum::Element elementSkin)
{
	MenusScene* menuScene = (MenusScene*)scene;

	switch (stage)
	{
	case CharacterMenuHandler::Done:
	case CharacterMenuHandler::MainDialog:
		{
			//Set button images for each of the windows in the Main Dialog Window
			SetButtonImage(GetWindow(MW_MAIN_CREATE_CHAR_BTN, player1), BTN_SKIN_PUSH_BUTTON, elementSkin);
			SetButtonImage(GetWindow(MW_MAIN_EDIT_CHAR_BTN, player1), BTN_SKIN_PUSH_BUTTON, elementSkin);
			SetButtonImage(GetWindow(MW_MAIN_LOAD_CHAR_BTN, player1), BTN_SKIN_PUSH_BUTTON, elementSkin);
			SetButtonImage(GetWindow(MW_MAIN_DONE_BTN, player1), BTN_SKIN_PUSH_BUTTON, elementSkin);
		}
		break;
	case CharacterMenuHandler::CreateStats:
	case CharacterMenuHandler::EditStats:
		{
			SetButtonImage(GetWindow(MW_STATS_ENDUR_ADD, player1), BTN_SKIN_ADD_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_ENDUR_MINUS, player1), BTN_SKIN_MINUS_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_RECO_ADD, player1), BTN_SKIN_ADD_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_RECO_MINUS, player1), BTN_SKIN_MINUS_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_STR_ADD, player1), BTN_SKIN_ADD_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_STR_MINUS, player1), BTN_SKIN_MINUS_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_DEF_ADD, player1), BTN_SKIN_ADD_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_DEF_MINUS, player1), BTN_SKIN_MINUS_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_AGI_ADD, player1), BTN_SKIN_ADD_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_AGI_MINUS, player1), BTN_SKIN_MINUS_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_LUCK_ADD, player1), BTN_SKIN_ADD_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_LUCK_MINUS, player1), BTN_SKIN_MINUS_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_OK_BTN, player1), BTN_SKIN_PUSH_BUTTON, elementSkin);
			SetButtonImage(GetWindow(MW_STATS_CANCEL_BTN, player1), BTN_SKIN_PUSH_BUTTON, elementSkin);
		}
		break;
	case CharacterMenuHandler::LoadDialog:
		{
			SetButtonImage(GetWindow(MW_LOAD_FILE1_BTN, player1), BTN_SKIN_PUSH_BUTTON, elementSkin);
			SetButtonImage(GetWindow(MW_LOAD_FILE2_BTN, player1), BTN_SKIN_PUSH_BUTTON, elementSkin);
			SetButtonImage(GetWindow(MW_LOAD_FILE3_BTN, player1), BTN_SKIN_PUSH_BUTTON, elementSkin);
			SetButtonImage(GetWindow(MW_LOAD_PAGE_LEFT_BTN, player1), BTN_SKIN_LEFT_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_LOAD_PAGE_RIGHT_BTN, player1), BTN_SKIN_RIGHT_BTN_HORZ, elementSkin);
			SetButtonImage(GetWindow(MW_LOAD_CANCEL_BUTTON, player1), BTN_SKIN_PUSH_BUTTON, elementSkin);
		}
		break;
	default:
		break;
	}
}

void CharacterMenuHandler::CreateCharacter(bool player1)
{
	ProbenderData* playerData = NULL;
	CharacterAttributes* attr = NULL;

	if(player1)
	{
		playerData = &p1ModdedData;
		p1ValidCharacter = true;
	}
	else
	{
		playerData = &p2ModdedData;
		p2ValidCharacter = true;
	}

	unsigned short default_val = 5;


	attr = &playerData->BaseAttributes;
	
	playerData->MainElement = ElementEnum::Earth;

	attr->SetAttribute(ProbenderAttributes::Endurance, default_val);
	attr->SetAttribute(ProbenderAttributes::Recovery, default_val);
	attr->SetAttribute(ProbenderAttributes::Strength, default_val);
	attr->SetAttribute(ProbenderAttributes::Defense, default_val);
	attr->SetAttribute(ProbenderAttributes::Agility, default_val);
	attr->SetAttribute(ProbenderAttributes::Luck, default_val);
}

void CharacterMenuHandler::ApplyModdedData(bool player1)
{
	MenusScene* menu = (MenusScene*)scene;

	if(player1)
	{
		menu->Player1Data.MainElement = p1ModdedData.MainElement;
		menu->Player1Data.BaseAttributes = p1ModdedData.BaseAttributes;
	}
	else
	{
		menu->Player2Data.MainElement = p2ModdedData.MainElement;
		menu->Player2Data.BaseAttributes = p2ModdedData.BaseAttributes;
	}
}

void CharacterMenuHandler::UpdateAttributeScores(bool player1)
{
	CharacterAttributes attrToUse;
	unsigned int availPoints;

	if(player1)
	{
		attrToUse = p1ModdedData.BaseAttributes;
		availPoints = p1PointsAvailable;
	}
	else
	{
		attrToUse = p2ModdedData.BaseAttributes;
		availPoints = p2PointsAvailable;
	}

	GetWindow(MW_STATS_AVAIL_POINT_VAL, player1)->setText(std::to_string(availPoints));
	GetWindow(MW_STATS_ENDUR_VAL, player1)->setText(std::to_string(attrToUse.GetAttribute(ProbenderAttributes::Endurance)));
	GetWindow(MW_STATS_RECO_VAL, player1)->setText(std::to_string(attrToUse.GetAttribute(ProbenderAttributes::Recovery)));
	GetWindow(MW_STATS_STR_VAL, player1)->setText(std::to_string(attrToUse.GetAttribute(ProbenderAttributes::Strength)));
	GetWindow(MW_STATS_DEF_VAL, player1)->setText(std::to_string(attrToUse.GetAttribute(ProbenderAttributes::Defense)));
	GetWindow(MW_STATS_AGI_VAL, player1)->setText(std::to_string(attrToUse.GetAttribute(ProbenderAttributes::Agility)));
	GetWindow(MW_STATS_LUCK_VAL, player1)->setText(std::to_string(attrToUse.GetAttribute(ProbenderAttributes::Luck)));
}

CEGUI::Window* CharacterMenuHandler::GetWindow(MenuWindows windowToGet, bool player1 /*= true*/)
{
	switch (windowToGet)
	{
	case CharacterMenuHandler::MW_ROOT:
		return guiManager->GetChildWindow(ROOT_WINDOW_NAME);
		break;
	case CharacterMenuHandler::MW_MAIN_DLG_ROOT:
		if(player1)
			return guiManager->GetChildWindow(ROOT_WINDOW_NAME + "/" + P1_PREFIX + MAIN_DIALOG_ROOT);
		else
			return guiManager->GetChildWindow(ROOT_WINDOW_NAME + "/" + P2_PREFIX + MAIN_DIALOG_ROOT);
		break;
	case CharacterMenuHandler::MW_LOAD_DLG_ROOT:
		if(player1)
			return guiManager->GetChildWindow(ROOT_WINDOW_NAME + "/" + P1_PREFIX + LOAD_DIALOG_ROOT);
		else
			return guiManager->GetChildWindow(ROOT_WINDOW_NAME + "/" + P2_PREFIX + LOAD_DIALOG_ROOT);
		break;
	case CharacterMenuHandler::MW_STAT_DLG_ROOT:
		if(player1)
			return guiManager->GetChildWindow(ROOT_WINDOW_NAME + "/" + P1_PREFIX + STAT_DIALOG_ROOT);
		else
			return guiManager->GetChildWindow(ROOT_WINDOW_NAME + "/" + P2_PREFIX + STAT_DIALOG_ROOT);
		break;

	case CharacterMenuHandler::MW_NAME_BACKING:
		if(player1)
			return guiManager->GetChildWindow(ROOT_WINDOW_NAME + "/P1NameBacking");
		else
			return guiManager->GetChildWindow(ROOT_WINDOW_NAME + "/P2NameBacking");
		break;
	case CharacterMenuHandler::MW_ELEMENT_TEXT:
		if(player1)
			return guiManager->GetChildWindow(p1NameBacking, "ProbenderElement");
		else
			return guiManager->GetChildWindow(p2NameBacking, "ProbenderElement");
		break;
	case CharacterMenuHandler::MW_NAME_TEXT:
		if(player1)
			return guiManager->GetChildWindow(p1NameBacking, "ProbenderName");
		else
			return guiManager->GetChildWindow(p2NameBacking, "ProbenderName");
		break;
	case CharacterMenuHandler::MW_ELEMENT_SEL_LEFT:
		if(player1)
			return guiManager->GetChildWindow(p1NameBacking, "L_Element_Btn");
		else
			return guiManager->GetChildWindow(p2NameBacking, "L_Element_Btn");
		break;
	case CharacterMenuHandler::MW_ELEMENT_SEL_RIGHT:
		if(player1)
			return guiManager->GetChildWindow(p1NameBacking, "R_Element_Btn");
		else
			return guiManager->GetChildWindow(p2NameBacking, "R_Element_Btn");
		break;

	case CharacterMenuHandler::MW_MAIN_CREATE_CHAR_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1MainDialog, "CreateCharButton");
		else
			return guiManager->GetChildWindow(p2MainDialog, "CreateCharButton");
		break;
	case CharacterMenuHandler::MW_MAIN_EDIT_CHAR_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1MainDialog, "EditCharButton");
		else
			return guiManager->GetChildWindow(p2MainDialog, "EditCharButton");
		break;
		break;
	case CharacterMenuHandler::MW_MAIN_LOAD_CHAR_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1MainDialog, "LoadCharButton");
		else
			return guiManager->GetChildWindow(p2MainDialog, "LoadCharButton");
		break;
	case CharacterMenuHandler::MW_MAIN_DONE_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1MainDialog, "DoneButton");
		else
			return guiManager->GetChildWindow(p2MainDialog, "DoneButton");
		break;

	case CharacterMenuHandler::MW_STATS_ENDUR_MINUS:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Endurance" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Endurance" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_ENDUR_ADD:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Endurance" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Endurance" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_ENDUR_VAL:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Endurance" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Endurance" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_RECO_MINUS:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Recovery" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Recovery" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_RECO_ADD:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Recovery" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Recovery" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_RECO_VAL:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Recovery" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Recovery" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_STR_MINUS:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Strength" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Strength" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_STR_ADD:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Strength" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Strength" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_STR_VAL:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Strength" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Strength" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_DEF_MINUS:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Defense" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Defense" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_DEF_ADD:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Defense" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Defense" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_DEF_VAL:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Defense" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Defense" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_AGI_MINUS:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Agility" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Agility" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_AGI_ADD:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Agility" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Agility" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_AGI_VAL:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Agility" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Agility" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_LUCK_MINUS:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Luck" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Luck" + ATTRIBUTE_SUFFIX + "/" + MINUS_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_LUCK_ADD:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Luck" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Luck" + ATTRIBUTE_SUFFIX + "/" + ADD_BTN_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_LUCK_VAL:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "Luck" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		else
			return guiManager->GetChildWindow(p2StatsDialog, "Luck" + ATTRIBUTE_SUFFIX + "/" + VALUE_SUFFIX);
		break;
	case CharacterMenuHandler::MW_STATS_OK_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "OKButton");
		else
			return guiManager->GetChildWindow(p2StatsDialog, "OKButton");
		break;
	case CharacterMenuHandler::MW_STATS_CANCEL_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "CancelButton");
		else
			return guiManager->GetChildWindow(p2StatsDialog, "CancelButton");
		break;
	case MW_STATS_AVAIL_POINT_VAL:
		if(player1)
			return guiManager->GetChildWindow(p1StatsDialog, "AvailablePointsRoot/Value");
		else
			return guiManager->GetChildWindow(p2StatsDialog, "AvailablePointsRoot/Value");
		break;

	case CharacterMenuHandler::MW_LOAD_FILE1_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1LoadDialog, "FileButton1");
		else
			return guiManager->GetChildWindow(p2LoadDialog, "FileButton1");
		break;
	case CharacterMenuHandler::MW_LOAD_FILE2_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1LoadDialog, "FileButton2");
		else
			return guiManager->GetChildWindow(p2LoadDialog, "FileButton2");
		break;
	case CharacterMenuHandler::MW_LOAD_FILE3_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1LoadDialog, "FileButton3");
		else
			return guiManager->GetChildWindow(p2LoadDialog, "FileButton3");
		break;
	case CharacterMenuHandler::MW_LOAD_PAGE_RIGHT_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1LoadDialog, "FilePageRight");
		else
			return guiManager->GetChildWindow(p2LoadDialog, "FilePageRight");
		break;
	case CharacterMenuHandler::MW_LOAD_PAGE_LEFT_BTN:
		if(player1)
			return guiManager->GetChildWindow(p1LoadDialog, "FilePageLeft");
		else
			return guiManager->GetChildWindow(p2LoadDialog, "FilePageLeft");
		break;
	case CharacterMenuHandler::MW_LOAD_CANCEL_BUTTON:
		if(player1)
			return guiManager->GetChildWindow(p1LoadDialog, "CancelButton");
		else
			return guiManager->GetChildWindow(p2LoadDialog, "CancelButton");
		break;

	default:
		return NULL;
		break;
	}

}

#pragma region Button Events

bool CharacterMenuHandler::IsPlayer1(const CEGUI::WindowEventArgs& e)
{
	const CEGUI::WindowEventArgs& windowE = (const CEGUI::WindowEventArgs&)e;
	CEGUI::String parentName = windowE.window->getParent()->getName();
	return parentName[1] == '1';
}

bool CharacterMenuHandler::L_ElementButtonClickEvent(const CEGUI::EventArgs& e)
{
	MenusScene* menu = (MenusScene*)scene;

	if(IsPlayer1((const CEGUI::WindowEventArgs&)e))
	{
		p1ModdedData.MainElement = (ElementEnum::Element)(p1ModdedData.MainElement - 1);

		if(p1ModdedData.MainElement < ElementEnum::Air)
			p1ModdedData.MainElement = ElementEnum::Water;

		ChangeElement(true, p1ModdedData.MainElement);
	}
	else
	{
		p2ModdedData.MainElement = (ElementEnum::Element)(p2ModdedData.MainElement - 1);

		if(p2ModdedData.MainElement < ElementEnum::Air)
			p2ModdedData.MainElement = ElementEnum::Water;

		ChangeElement(false, p2ModdedData.MainElement);
	}

	return true;
}

bool CharacterMenuHandler::R_ElementButtonClickEvent(const CEGUI::EventArgs& e)
{
	MenusScene* menu = (MenusScene*)scene;

	if(IsPlayer1((const CEGUI::WindowEventArgs&)e))
	{
		p1ModdedData.MainElement = (ElementEnum::Element)(p1ModdedData.MainElement + 1);

		if(p1ModdedData.MainElement > ElementEnum::Water)
			p1ModdedData.MainElement = ElementEnum::Air;

		ChangeElement(true, p1ModdedData.MainElement);
	}
	else
	{
		p2ModdedData.MainElement = (ElementEnum::Element)(p2ModdedData.MainElement + 1);

		if(p2ModdedData.MainElement > ElementEnum::Water)
			p2ModdedData.MainElement = ElementEnum::Air;

		ChangeElement(false, p2ModdedData.MainElement);
	}

	return true;
}

bool CharacterMenuHandler::MainDlgCreateCharBtnClickEvent(const CEGUI::EventArgs& e)
{
	if(IsPlayer1((const CEGUI::WindowEventArgs&)e))
	{
		SetStage(true, CreateStats);
	}
	else
	{
		SetStage(false , CreateStats);
	}

	return true;
}

bool CharacterMenuHandler::MainDlgEditCharBtnClickEvent(const CEGUI::EventArgs& e)
{
	if(IsPlayer1((const CEGUI::WindowEventArgs&)e))
	{
		printf("Player1 Edit Button");
	}
	else
	{
		printf("Player2 Edit Button");
	}

	return true;
}

bool CharacterMenuHandler::MainDlgLoadCharBtnClickEvent(const CEGUI::EventArgs& e)
{
	if(IsPlayer1((const CEGUI::WindowEventArgs&)e))
	{
		SetStage(true, LoadDialog);
	}
	else
	{
		SetStage(false, LoadDialog);
	}

	return true;
}

bool CharacterMenuHandler::MainDlgDoneBtnClickEvent(const CEGUI::EventArgs& e)
{
	MenusScene* menu = (MenusScene*)scene;
	//will need fixing
	if(p1CurrentStage == Done && p2CurrentStage == Done)
	{
		menu->SetScreen(MenusScene::GameSetup);
	}

	return true;
}

ProbenderAttributes::Attributes CharacterMenuHandler::GetAttributeFromWindowName(CEGUI::Window* window)
{
	CEGUI::String name = window->getName();

	if(name[0] == 'E')
		return ProbenderAttributes::Endurance;
	else if(name[0] == 'R')
		return ProbenderAttributes::Recovery;
	else if(name[0] == 'S')
		return ProbenderAttributes::Strength;
	else if(name[0] == 'D')
		return ProbenderAttributes::Defense;
	else if(name[0] == 'A')
		return ProbenderAttributes::Agility;
	else if(name[0] == 'L')
		return ProbenderAttributes::Luck;
	else
		return ProbenderAttributes::InvalidAttribute;
}

bool CharacterMenuHandler::StatsMinusBtnClickEvent(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& windowE = (const CEGUI::WindowEventArgs&)e;
	CEGUI::String parentName = windowE.window->getParent()->getParent()->getName();
	bool isPlayer1 = parentName[1] == '1';

	ProbenderAttributes::Attributes attribute = GetAttributeFromWindowName(windowE.window->getParent());

	unsigned short currVal;

	if(isPlayer1)
	{
		currVal = p1ModdedData.BaseAttributes.GetAttribute(attribute);
		if(currVal > 1)
		{
			p1ModdedData.BaseAttributes.SetAttribute(attribute, currVal - 1);
			p1PointsAvailable++;
		}
	}
	else
	{
		currVal = p2ModdedData.BaseAttributes.GetAttribute(attribute);
		if(currVal > 1)
		{
			p2ModdedData.BaseAttributes.SetAttribute(attribute, currVal - 1);
			p2PointsAvailable++;
		}
	}

	UpdateAttributeScores(isPlayer1);
	return true;
}

bool CharacterMenuHandler::StatsAddBtnClickEvent(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& windowE = (const CEGUI::WindowEventArgs&)e;
	CEGUI::String parentName = windowE.window->getParent()->getParent()->getName();
	bool isPlayer1 = parentName[1] == '1';

	ProbenderAttributes::Attributes attribute = GetAttributeFromWindowName(windowE.window->getParent());

	unsigned short currVal;

	if(isPlayer1)
	{
		currVal = p1ModdedData.BaseAttributes.GetAttribute(attribute);
		if(currVal < ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED && p1PointsAvailable > 0)
		{
			p1ModdedData.BaseAttributes.SetAttribute(attribute, currVal + 1);
			p1PointsAvailable--;
		}

		if(p1PointsAvailable == 0)
			p1ValidCharacter = true;
		else
			p1ValidCharacter = false;
	}
	else
	{
		currVal = p2ModdedData.BaseAttributes.GetAttribute(attribute);
		if(currVal < ProbenderAttributes::MAX_ATTRIBUTE_POINTS_ALLOWED && p2PointsAvailable > 0)
		{
			p2ModdedData.BaseAttributes.SetAttribute(attribute, currVal + 1);
			p2PointsAvailable--;
		}

		if(p2PointsAvailable == 0)
			p2ValidCharacter = true;
		else
			p2ValidCharacter = false;
	}

	UpdateAttributeScores(isPlayer1);
	return true;
}

bool CharacterMenuHandler::StatsOKBtnClickEvent(const CEGUI::EventArgs& e)
{
	if(IsPlayer1((CEGUI::WindowEventArgs&)e))
	{
		if(p1ValidCharacter)
		{
			ApplyModdedData(true);
			SetStage(true, Done);
		}
	}
	else
	{
		if(p2ValidCharacter)
		{
			ApplyModdedData(false);
			SetStage(false, Done);
		}
	}

	return true;
}

bool CharacterMenuHandler::StatsCancelBtnClickEvent(const CEGUI::EventArgs& e)
{
	if(IsPlayer1((CEGUI::WindowEventArgs&)e))
	{
		p1PointsAvailable = 0;
		SetStage(true, MainDialog);
	}
	else
	{
		p2PointsAvailable = 0;
		SetStage(false, MainDialog);
	}

	return true;
}

void CharacterMenuHandler::ActivateElementChoices(bool player1)
{
	GetWindow(MW_ELEMENT_SEL_LEFT, player1)->enable();
	GetWindow(MW_ELEMENT_SEL_RIGHT, player1)->enable();
}

void CharacterMenuHandler::DeactivateElementChoices(bool player1)
{
	GetWindow(MW_ELEMENT_SEL_LEFT, player1)->disable();
	GetWindow(MW_ELEMENT_SEL_RIGHT, player1)->disable();
}

#pragma endregion