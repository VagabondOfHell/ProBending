#pragma once
#include "MenuHandler.h"
#include "ArenaData.h"

class GameSetupMenuHandler: public MenuHandler
{
protected:
	CEGUI::Window *gameOptionsRoot, *playerOptionsRoot;

	static const std::string ROOT_WINDOW_NAME;
	static const std::string GAME_OPTIONS_ROOT; static const std::string PLAYER_OPTIONS_ROOT;

	ArenaData::Zones zoneListing[6];
	unsigned int p1ZoneIndex, p2ZoneIndex;

	enum MenuWindows{
		MW_ROOT_WINDOW, MW_GAME_OPTIONS_ROOT, MW_PLAYER_OPTIONS_ROOT,
		MW_PLAYER_ELEMENT_ROOT, MW_PLAYER_TEAM_ROOT, MW_PLAYER_COLOUR_ROOT, MW_PLAYER_ZONE_ROOT,
		MW_MODE_LEFT_SEL, MW_MODE_RIGHT_SEL, MW_MODE_VAL,
		MW_PLAYER_ID_BACKING, 
		MW_PLAYER_ELEMENT_LEFT_SEL, MW_PLAYER_ELEMENT_RIGHT_SEL, MW_PLAYER_ELEMENT_VAL, 
		MW_PLAYER_TEAM_COLOUR_LEFT_SEL, MW_PLAYER_TEAM_COLOUR_RIGHT_SEL, MW_PLAYER_TEAM_COLOUR_VAL, 
		MW_PLAYER_COLOUR_LEFT_SEL, MW_PLAYER_COLOUR_RIGHT_SEL, MW_PLAYER_COLOUR_VAL,
		MW_PLAYER_ZONE_LEFT_SEL, MW_PLAYER_ZONE_RIGHT_SEL, MW_PLAYER_ZONE_COLOUR_VAL, MW_PLAYER_ZONE_POS_VAL,
		MW_BEGIN_GAME_BTN, MW_CHARACTER_SELECT_BTN};

	void GetRootWindows();

	void SubscribeEvents();
	void UnsubscribeEvents();

	void DisableUnimplementedControls();

	bool IsPlayer1(const CEGUI::WindowEventArgs& windowEvent);

	void SetTeamColours(ArenaData::Team player1);

	void SetControlSkins(bool player1, ElementEnum::Element elementSkin);
	void ChangeElement(bool player1, ElementEnum::Element newElement);

	std::string GetZoneColourText(ArenaData::Zones zone)
	{
		switch (zone)
		{
		case ArenaData::RED_ZONE_1:
		case ArenaData::RED_ZONE_2:
		case ArenaData::RED_ZONE_3:
			return "RED";
			break;
		case ArenaData::BLUE_ZONE_1:
		case ArenaData::BLUE_ZONE_2:
		case ArenaData::BLUE_ZONE_3:
			return "BLUE";
			break;
		default:
			return "";
			break;
		}
	}
	std::string GetZonePositionText(ArenaData::Zones zone)
	{
		switch (zone)
		{
		case ArenaData::RED_ZONE_1:
		case ArenaData::BLUE_ZONE_1:
			return "ZONE 1";
			break;

		case ArenaData::RED_ZONE_2:
		case ArenaData::BLUE_ZONE_2:
			return "ZONE 2";
			break;
		case ArenaData::RED_ZONE_3:
		case ArenaData::BLUE_ZONE_3:
			return "ZONE 3";
			break;
		default:
			return "";
			break;
		}
	}

	void SwapZones()
	{
		if(p1ZoneIndex == 0)
			p1ZoneIndex = 5;
		else if(p1ZoneIndex == 1)
			p1ZoneIndex = 4;
		else if(p1ZoneIndex == 2)
			p1ZoneIndex = 3;
		else if(p1ZoneIndex == 3)
			p1ZoneIndex = 2;
		else if(p1ZoneIndex == 4)
			p1ZoneIndex = 1;
		else if(p1ZoneIndex == 5)
			p1ZoneIndex = 0;

		if(p2ZoneIndex == 0)
			p2ZoneIndex = 5;
		else if(p2ZoneIndex == 1)
			p2ZoneIndex = 4;
		else if(p2ZoneIndex == 2)
			p2ZoneIndex = 3;
		else if(p2ZoneIndex == 3)
			p2ZoneIndex = 2;
		else if(p2ZoneIndex == 4)
			p2ZoneIndex = 1;
		else if(p2ZoneIndex == 5)
			p2ZoneIndex = 0;
	}

	void IncrementZone(bool incrementPlayer1);
	void DecrementZone(bool decrementPlayer1);

	void IncrementElement(bool incrementPlayer1);
	void DecrementElement(bool incrementPlayer1);

	void SetZoneData();

	CEGUI::Window* GetWindow(MenuWindows windowToGet, bool player1 = true);

public:
	GameSetupMenuHandler(IScene* scene);
	virtual ~GameSetupMenuHandler(void);

	bool StartGameBtnClickEvent(const CEGUI::EventArgs& e);
	bool GameModeSelBtnClickEvent(const CEGUI::EventArgs& e);

	bool ElementSelBtnClickEvent(const CEGUI::EventArgs& e);
	bool TeamSelBtnClickEvent(const CEGUI::EventArgs& e);
	bool ZoneSelBtnClickEvent(const CEGUI::EventArgs& e);
	bool CharColourSelBtnClickEvent(const CEGUI::EventArgs& e);

};

