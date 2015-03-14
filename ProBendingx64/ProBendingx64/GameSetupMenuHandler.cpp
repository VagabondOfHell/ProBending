#include "GameSetupMenuHandler.h"

#include "GUIManager.h"
#include "MenusScene.h"

#include "CEGUI/widgets/PushButton.h"

const std::string GameSetupMenuHandler::ROOT_WINDOW_NAME = "SetupMenuRoot";
const std::string GameSetupMenuHandler::PLAYER_OPTIONS_ROOT = "PlayerOptionsRoot";
const std::string GameSetupMenuHandler::GAME_OPTIONS_ROOT = "GameOptionsRoot";


GameSetupMenuHandler::GameSetupMenuHandler(IScene* scene)
	:MenuHandler(scene), p1ZoneIndex(0), p2ZoneIndex(0)
{
	GetRootWindows();
	DisableUnimplementedControls();
	SubscribeEvents();
	
	ChangeElement(true, ElementEnum::InvalidElement);
	ChangeElement(false, ElementEnum::InvalidElement);

	zoneListing[0] = ArenaData::RED_ZONE_3;
	zoneListing[1] = ArenaData::RED_ZONE_2;
	zoneListing[2] = ArenaData::RED_ZONE_1;
	zoneListing[3] = ArenaData::BLUE_ZONE_1;
	zoneListing[4] = ArenaData::BLUE_ZONE_2;
	zoneListing[5] = ArenaData::BLUE_ZONE_3;
	
	SetTeamColours(ArenaData::RED_TEAM);

	p1ZoneIndex = 2;
	p2ZoneIndex = 3;

	SetZoneData();
}


GameSetupMenuHandler::~GameSetupMenuHandler(void)
{
}

void GameSetupMenuHandler::GetRootWindows()
{
	rootWindow = GetWindow(MW_ROOT_WINDOW);
	gameOptionsRoot = GetWindow(MW_GAME_OPTIONS_ROOT);
	playerOptionsRoot = GetWindow(MW_PLAYER_OPTIONS_ROOT);

}

void GameSetupMenuHandler::DisableUnimplementedControls()
{
	GetWindow(MW_PLAYER_COLOUR_LEFT_SEL)->disable();
	GetWindow(MW_PLAYER_COLOUR_RIGHT_SEL)->disable();
	GetWindow(MW_PLAYER_COLOUR_LEFT_SEL, false)->disable();
	GetWindow(MW_PLAYER_COLOUR_RIGHT_SEL, false)->disable();

	GetWindow(MW_MODE_LEFT_SEL)->disable();
	GetWindow(MW_MODE_RIGHT_SEL)->disable();
}

void GameSetupMenuHandler::SubscribeEvents()
{
	MenusScene* menu = (MenusScene*)scene;

	GetWindow(MW_BEGIN_GAME_BTN)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::StartGameBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_BEGIN_GAME_BTN));

	GetWindow(MW_MODE_LEFT_SEL)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::GameModeSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_MODE_LEFT_SEL));

	GetWindow(MW_MODE_RIGHT_SEL)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::GameModeSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_MODE_RIGHT_SEL));

	GetWindow(MW_PLAYER_ELEMENT_LEFT_SEL)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::ElementSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_ELEMENT_LEFT_SEL));

	GetWindow(MW_PLAYER_ELEMENT_RIGHT_SEL)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::ElementSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_ELEMENT_RIGHT_SEL));

	GetWindow(MW_PLAYER_ELEMENT_LEFT_SEL, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::ElementSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_ELEMENT_LEFT_SEL, false));

	GetWindow(MW_PLAYER_ELEMENT_RIGHT_SEL, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::ElementSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_ELEMENT_RIGHT_SEL, false));

	GetWindow(MW_PLAYER_TEAM_COLOUR_LEFT_SEL)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::TeamSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_TEAM_COLOUR_LEFT_SEL));

	GetWindow(MW_PLAYER_TEAM_COLOUR_RIGHT_SEL)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::TeamSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_TEAM_COLOUR_RIGHT_SEL));

	GetWindow(MW_PLAYER_TEAM_COLOUR_LEFT_SEL, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::TeamSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_TEAM_COLOUR_LEFT_SEL, false));

	GetWindow(MW_PLAYER_TEAM_COLOUR_RIGHT_SEL, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::TeamSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_TEAM_COLOUR_RIGHT_SEL, false));

	GetWindow(MW_PLAYER_ZONE_LEFT_SEL)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::ZoneSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_ZONE_LEFT_SEL));

	GetWindow(MW_PLAYER_ZONE_RIGHT_SEL)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::ZoneSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_ZONE_RIGHT_SEL));

	GetWindow(MW_PLAYER_ZONE_LEFT_SEL, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::ZoneSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_ZONE_LEFT_SEL, false));

	GetWindow(MW_PLAYER_ZONE_RIGHT_SEL, false)->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber
		(&GameSetupMenuHandler::ZoneSelBtnClickEvent,this));
	menu->RegisterHoverEvents(GetWindow(MW_PLAYER_ZONE_RIGHT_SEL, false));
}

void GameSetupMenuHandler::UnsubscribeEvents()
{
	MenusScene* menu = (MenusScene*)scene;

	menu->UnregisterHoverEvents(GetWindow(MW_BEGIN_GAME_BTN));

	menu->UnregisterHoverEvents(GetWindow(MW_MODE_LEFT_SEL));

	menu->UnregisterHoverEvents(GetWindow(MW_MODE_RIGHT_SEL));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_ELEMENT_LEFT_SEL));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_ELEMENT_RIGHT_SEL));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_ELEMENT_LEFT_SEL, false));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_ELEMENT_RIGHT_SEL, false));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_TEAM_COLOUR_LEFT_SEL));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_TEAM_COLOUR_RIGHT_SEL));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_TEAM_COLOUR_LEFT_SEL, false));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_TEAM_COLOUR_RIGHT_SEL, false));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_ZONE_LEFT_SEL));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_ZONE_RIGHT_SEL));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_ZONE_LEFT_SEL, false));

	menu->UnregisterHoverEvents(GetWindow(MW_PLAYER_ZONE_RIGHT_SEL, false));
}

CEGUI::Window* GameSetupMenuHandler::GetWindow(MenuWindows windowToGet, bool player1 /*= true*/)
{
	switch (windowToGet)
	{
	case GameSetupMenuHandler::MW_ROOT_WINDOW:
		return guiManager->GetChildWindow(ROOT_WINDOW_NAME);
		break;
	case GameSetupMenuHandler::MW_GAME_OPTIONS_ROOT:
		return guiManager->GetChildWindow(ROOT_WINDOW_NAME + "/" + GAME_OPTIONS_ROOT);
		break;
	case GameSetupMenuHandler::MW_PLAYER_OPTIONS_ROOT:
		return guiManager->GetChildWindow(ROOT_WINDOW_NAME + "/" + PLAYER_OPTIONS_ROOT);
		break;
	case GameSetupMenuHandler::MW_PLAYER_ELEMENT_ROOT:
		return guiManager->GetChildWindow(playerOptionsRoot, "ProbenderElementLabelRoot");
		break;
	case GameSetupMenuHandler::MW_PLAYER_TEAM_ROOT:
		return guiManager->GetChildWindow(playerOptionsRoot, "TeamLabelRoot");
		break;
	case GameSetupMenuHandler::MW_PLAYER_COLOUR_ROOT:
		return guiManager->GetChildWindow(playerOptionsRoot, "CharColourLabelRoot");
		break;
	case GameSetupMenuHandler::MW_PLAYER_ZONE_ROOT:
		return guiManager->GetChildWindow(playerOptionsRoot, "StartZoneLabelRoot");
		break;

	case GameSetupMenuHandler::MW_MODE_LEFT_SEL:
		return guiManager->GetChildWindow(gameOptionsRoot, "GameModeSelLButton");
		break;
	case GameSetupMenuHandler::MW_MODE_RIGHT_SEL:
		return guiManager->GetChildWindow(gameOptionsRoot, "GameModeSelRButton");
		break;
	case GameSetupMenuHandler::MW_MODE_VAL:
		return guiManager->GetChildWindow(gameOptionsRoot, "GameModeLabel");
		break;

	case GameSetupMenuHandler::MW_PLAYER_ID_BACKING:
		if(player1)
			return guiManager->GetChildWindow(playerOptionsRoot, "Player1Backing");
		else
			return guiManager->GetChildWindow(playerOptionsRoot, "Player2Backing");
		break;

	case GameSetupMenuHandler::MW_PLAYER_ELEMENT_LEFT_SEL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ELEMENT_ROOT), "P1ElementSelLeft");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ELEMENT_ROOT), "P2ElementSelLeft");
		break;
	case GameSetupMenuHandler::MW_PLAYER_ELEMENT_RIGHT_SEL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ELEMENT_ROOT), "P1ElementSelRight");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ELEMENT_ROOT), "P2ElementSelRight");
		break;
	case GameSetupMenuHandler::MW_PLAYER_ELEMENT_VAL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ELEMENT_ROOT), "P1Element");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ELEMENT_ROOT), "P2Element");
		break;
		
	case GameSetupMenuHandler::MW_PLAYER_TEAM_COLOUR_LEFT_SEL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_TEAM_ROOT), "P1TeamSelLeft");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_TEAM_ROOT), "P2TeamSelLeft");
		break;
	case GameSetupMenuHandler::MW_PLAYER_TEAM_COLOUR_RIGHT_SEL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_TEAM_ROOT), "P1TeamSelRight");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_TEAM_ROOT), "P2TeamSelRight");
		break;
	case GameSetupMenuHandler::MW_PLAYER_TEAM_COLOUR_VAL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_TEAM_ROOT), "P1TeamColour");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_TEAM_ROOT), "P2TeamColour");
		break;

	case GameSetupMenuHandler::MW_PLAYER_COLOUR_LEFT_SEL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_COLOUR_ROOT), "P1ColourSelLeft");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_COLOUR_ROOT), "P2ColourSelLeft");
		break;
	case GameSetupMenuHandler::MW_PLAYER_COLOUR_RIGHT_SEL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_COLOUR_ROOT), "P1ColourSelRight");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_COLOUR_ROOT), "P2ColourSelRight");
		break;
	case GameSetupMenuHandler::MW_PLAYER_COLOUR_VAL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_COLOUR_ROOT), "P1CharColour");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_COLOUR_ROOT), "P2CharColour");
		break;

	case GameSetupMenuHandler::MW_PLAYER_ZONE_LEFT_SEL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ZONE_ROOT), "P1SelLeft");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ZONE_ROOT), "P2SelLeft");
		break;
	case GameSetupMenuHandler::MW_PLAYER_ZONE_RIGHT_SEL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ZONE_ROOT), "P1SelRight");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ZONE_ROOT), "P2SelRight");
		break;
	case GameSetupMenuHandler::MW_PLAYER_ZONE_COLOUR_VAL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ZONE_ROOT), "P1ZoneColour");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ZONE_ROOT), "P2ZoneColour");
		break;
	case GameSetupMenuHandler::MW_PLAYER_ZONE_POS_VAL:
		if(player1)
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ZONE_ROOT), "P1ZonePosition");
		else
			return guiManager->GetChildWindow(GetWindow(MW_PLAYER_ZONE_ROOT), "P2ZonePosition");
		break;

	case GameSetupMenuHandler::MW_BEGIN_GAME_BTN:
		return guiManager->GetChildWindow(rootWindow, "BeginGameButton");
		break;

	case GameSetupMenuHandler::MW_CHARACTER_SELECT_BTN:
		return guiManager->GetChildWindow(rootWindow, "CharacterSetupButton");
		break;
	default:
		return NULL;
		break;
	}
}

bool GameSetupMenuHandler::IsPlayer1(const CEGUI::WindowEventArgs& windowEvent)
{
	CEGUI::String windowName = windowEvent.window->getName();

	return windowName[1] == '1';
}

bool GameSetupMenuHandler::StartGameBtnClickEvent(const CEGUI::EventArgs& e)
{
	MenusScene* menu = (MenusScene*)scene;

	menu->Player1Data.TeamDatas.CurrentZone = zoneListing[p1ZoneIndex];
	menu->Player2Data.TeamDatas.CurrentZone = zoneListing[p2ZoneIndex];

	menu->Player1Data.TeamDatas.PlayerColour = TeamData::RED;
	menu->Player2Data.TeamDatas.PlayerColour = TeamData::BLUE;

	menu->SwitchToGame();

	UnsubscribeEvents();

	return true;
}

bool GameSetupMenuHandler::GameModeSelBtnClickEvent(const CEGUI::EventArgs& e)
{
	CEGUI::WindowEventArgs& windowE = (CEGUI::WindowEventArgs&)e;
	CEGUI::Window* window = windowE.window;

	CEGUI::String windowName = window->getName();

	if(windowName[11] == 'L')
	{
		printf("Game Mode Left\n");
	}
	else if(windowName[11] == 'R')
	{
		printf("Game Mode Right\n");
	}

	return true;
}

bool GameSetupMenuHandler::ElementSelBtnClickEvent(const CEGUI::EventArgs& e)
{
	CEGUI::WindowEventArgs& windowE = (CEGUI::WindowEventArgs&)e;

	if(windowE.window->getName()[12] == 'L')
	{
		DecrementElement(IsPlayer1(windowE));
	}
	else
	{
		IncrementElement(IsPlayer1(windowE));
	}

	return true;
}

void GameSetupMenuHandler::IncrementElement(bool incrementPlayer1)
{
	MenusScene* menu = (MenusScene*)scene;

	if(incrementPlayer1)
	{
		menu->Player1Data.MainElement = (ElementEnum::Element)(menu->Player1Data.MainElement + 1);

		if(menu->Player1Data.MainElement > ElementEnum::Water)
			menu->Player1Data.MainElement = ElementEnum::Air;

		ChangeElement(true, menu->Player1Data.MainElement);
	}
	else
	{
		menu->Player2Data.MainElement = (ElementEnum::Element)(menu->Player2Data.MainElement + 1);

		if(menu->Player2Data.MainElement > ElementEnum::Water)
			menu->Player2Data.MainElement = ElementEnum::Air;

		ChangeElement(false, menu->Player2Data.MainElement);
	}
}

void GameSetupMenuHandler::DecrementElement(bool incrementPlayer1)
{
	MenusScene* menu = (MenusScene*)scene;

	if(incrementPlayer1)
	{
		menu->Player1Data.MainElement = (ElementEnum::Element)(menu->Player1Data.MainElement - 1);

		if(menu->Player1Data.MainElement < ElementEnum::Air)
			menu->Player1Data.MainElement = ElementEnum::Water;

		ChangeElement(true, menu->Player1Data.MainElement);
	}
	else
	{
		menu->Player2Data.MainElement = (ElementEnum::Element)(menu->Player2Data.MainElement - 1);

		if(menu->Player2Data.MainElement < ElementEnum::Air)
			menu->Player2Data.MainElement = ElementEnum::Water;

		ChangeElement(false, menu->Player2Data.MainElement);
	}
}

void GameSetupMenuHandler::ChangeElement(bool player1, ElementEnum::Element newElement)
{
	MenusScene* menu = (MenusScene*)scene;

	if(newElement == ElementEnum::InvalidElement)
	{
		if(player1)
			newElement = menu->Player2Data.MainElement;
		else
			newElement = menu->Player2Data.MainElement;
	}
	
	//Update the visible buttons
	SetControlSkins(player1, newElement);


}

void GameSetupMenuHandler::SetControlSkins(bool player1, ElementEnum::Element elementSkin)
{
	MenusScene* menuScene = (MenusScene*)scene;

	//Get the element prefix for accessing controls
	std::string elementPrefix = GetElementPrefix(elementSkin);
	GetWindow(MW_PLAYER_ELEMENT_VAL, player1)->setText(ElementEnum::EnumToString(elementSkin));
	//Set image for the name backing
	GetWindow(MW_PLAYER_ID_BACKING, player1)->setProperty("Image", "MenuControls/"+elementPrefix+"Lrg_Frame");

	SetButtonImage(GetWindow(MW_PLAYER_ELEMENT_LEFT_SEL, player1), BTN_SKIN_LEFT_BTN_HORZ, elementSkin);
	SetButtonImage(GetWindow(MW_PLAYER_ELEMENT_RIGHT_SEL, player1), BTN_SKIN_RIGHT_BTN_HORZ, elementSkin);
	
	SetButtonImage(GetWindow(MW_PLAYER_TEAM_COLOUR_LEFT_SEL, player1), BTN_SKIN_LEFT_BTN_HORZ, elementSkin);
	SetButtonImage(GetWindow(MW_PLAYER_TEAM_COLOUR_RIGHT_SEL, player1), BTN_SKIN_RIGHT_BTN_HORZ, elementSkin);

	SetButtonImage(GetWindow(MW_PLAYER_ZONE_LEFT_SEL, player1), BTN_SKIN_LEFT_BTN_HORZ, elementSkin);
	SetButtonImage(GetWindow(MW_PLAYER_ZONE_RIGHT_SEL, player1), BTN_SKIN_RIGHT_BTN_HORZ, elementSkin);

	SetButtonImage(GetWindow(MW_PLAYER_COLOUR_LEFT_SEL, player1), BTN_SKIN_LEFT_BTN_HORZ, elementSkin);
	SetButtonImage(GetWindow(MW_PLAYER_COLOUR_RIGHT_SEL, player1), BTN_SKIN_RIGHT_BTN_HORZ, elementSkin);
}


void GameSetupMenuHandler::SetTeamColours(ArenaData::Team player1)
{
	CEGUI::Window* player1TeamWindow = GetWindow(MW_PLAYER_TEAM_COLOUR_VAL, true);
	CEGUI::Window* player2TeamWindow = GetWindow(MW_PLAYER_TEAM_COLOUR_VAL, false);

	std::string player1String, player2String;

	MenusScene* menu = (MenusScene*)scene;

	menu->Player1Data.TeamDatas.Team = player1;

	if(player1 == ArenaData::BLUE_TEAM)
	{
		menu->Player2Data.TeamDatas.Team = ArenaData::RED_TEAM;
		player1String = "W_Lrg_Frame";
		player2String = "F_Lrg_Frame";
	}
	else if(player1 == ArenaData::RED_TEAM)
	{
		menu->Player2Data.TeamDatas.Team = ArenaData::BLUE_TEAM;
		player1String = "F_Lrg_Frame";
		player2String = "W_Lrg_Frame";
	}

	//Swap zones if team swaps (this way blue team falls off blue zone and red team falls off red zone and cant get inverted)
	SwapZones();

	SetZoneData();

	player1TeamWindow->setProperty("Image", "MenuControls/" + player1String);
	player2TeamWindow->setProperty("Image", "MenuControls/" + player2String);
}

bool GameSetupMenuHandler::TeamSelBtnClickEvent(const CEGUI::EventArgs& e)
{
	CEGUI::WindowEventArgs& windowE = (CEGUI::WindowEventArgs&)e;

	ArenaData::Team p1Team;
	
	MenusScene* menu = (MenusScene*)scene;

	if(IsPlayer1(windowE))
	{
		if(menu->Player1Data.TeamDatas.Team != ArenaData::BLUE_TEAM)
			p1Team = ArenaData::BLUE_TEAM;
		else
			p1Team = ArenaData::RED_TEAM;
	}
	else
	{
		if(menu->Player2Data.TeamDatas.Team != ArenaData::BLUE_TEAM)
			p1Team = ArenaData::RED_TEAM;
		else
			p1Team = ArenaData::BLUE_TEAM;

	}

	SetTeamColours(p1Team);

	return true;
}

void GameSetupMenuHandler::SetZoneData()
{
	CEGUI::Window* p1ZoneCol, *p2ZoneCol;
	CEGUI::Window* p1ZonePos, *p2ZonePos;

	p1ZoneCol = GetWindow(MW_PLAYER_ZONE_COLOUR_VAL); p1ZonePos = GetWindow(MW_PLAYER_ZONE_POS_VAL);
	p2ZoneCol = GetWindow(MW_PLAYER_ZONE_COLOUR_VAL, false); p2ZonePos = GetWindow(MW_PLAYER_ZONE_POS_VAL, false);

	p1ZoneCol->setText(GetZoneColourText(zoneListing[p1ZoneIndex])); p1ZonePos->setText(GetZonePositionText(zoneListing[p1ZoneIndex]));
	p2ZoneCol->setText(GetZoneColourText(zoneListing[p2ZoneIndex])); p2ZonePos->setText(GetZonePositionText(zoneListing[p2ZoneIndex]));
}

void GameSetupMenuHandler::IncrementZone(bool incrementPlayer1)
{
	unsigned int* val;
	unsigned int* otherVal;

	if(incrementPlayer1)
	{
		val = &p1ZoneIndex;
		otherVal = &p2ZoneIndex;
	}
	else
	{
		val = &p2ZoneIndex;
		otherVal = &p1ZoneIndex;
	}

	if(*val < 5)
		*val = *val + 1;

	if(*val == *otherVal)
		if(*otherVal < 5)
			*otherVal = *otherVal + 1;
		else
			*val = *val - 1;
}

void GameSetupMenuHandler::DecrementZone(bool decrementPlayer1)
{
	unsigned int* val;
	unsigned int* otherVal;

	if(decrementPlayer1)
	{
		val = &p1ZoneIndex;
		otherVal = &p2ZoneIndex;
	}
	else
	{
		val = &p2ZoneIndex;
		otherVal = &p1ZoneIndex;
	}

	if(*val > 0)
		*val = *val - 1;

	if(*val == *otherVal)
		if(*otherVal > 0)
			*otherVal = *otherVal - 1;
		else
			*val = *val + 1;
}

bool GameSetupMenuHandler::ZoneSelBtnClickEvent(const CEGUI::EventArgs& e)
{
	CEGUI::WindowEventArgs& windowE = (CEGUI::WindowEventArgs&)e;

	MenusScene* menu = (MenusScene*)scene;
	
	if(windowE.window->getName()[5] == 'L')
		DecrementZone(IsPlayer1(windowE));
	else
		IncrementZone(IsPlayer1(windowE));

	SetZoneData();

	return true;
}

bool GameSetupMenuHandler::CharColourSelBtnClickEvent(const CEGUI::EventArgs& e)
{


	return true;
}


