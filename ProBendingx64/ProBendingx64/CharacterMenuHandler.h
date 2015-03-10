#pragma once
#include "MenuHandler.h"
#include "ProbenderFlags.h"
#include "ProbenderData.h"

class CharacterMenuHandler: public MenuHandler
{
public:
	enum ButtonSkinType{BTN_SKIN_PUSH_BUTTON, BTN_SKIN_MINUS_BTN_HORZ, BTN_SKIN_ADD_BTN_HORZ, BTN_SKIN_LEFT_BTN_HORZ, BTN_SKIN_RIGHT_BTN_HORZ};
	
protected:
	static const std::string P1_PREFIX; static const std::string P2_PREFIX;

	static const std::string ROOT_WINDOW_NAME;
	static const std::string MAIN_DIALOG_ROOT; static const std::string LOAD_DIALOG_ROOT; static const std::string STAT_DIALOG_ROOT;
	static const std::string ATTRIBUTE_SUFFIX; static const std::string MINUS_BTN_SUFFIX; static const std::string ADD_BTN_SUFFIX;
	static const std::string VALUE_SUFFIX;

	struct ButtonSkinListing
	{
		std::string ClickImage, HoverImage, DisabledImage, NormalImage;
	};

	//Root Windows
	CEGUI::Window *p1MainDialog, *p1LoadDialog, *p1StatsDialog;	CEGUI::Window* p2MainDialog, *p2LoadDialog, *p2StatsDialog;
	CEGUI::Window* p1NameBacking, *p2NameBacking;

	enum Stage{MainDialog, CreateStats, EditStats, LoadDialog, Done};
	
	enum MenuWindows{MW_ROOT, MW_MAIN_DLG_ROOT, MW_LOAD_DLG_ROOT, MW_STAT_DLG_ROOT, MW_NAME_BACKING,
		MW_ELEMENT_SEL_LEFT, MW_ELEMENT_SEL_RIGHT, MW_ELEMENT_TEXT, MW_NAME_TEXT,
		MW_MAIN_CREATE_CHAR_BTN, MW_MAIN_EDIT_CHAR_BTN, MW_MAIN_LOAD_CHAR_BTN, MW_MAIN_DONE_BTN,
		MW_STATS_ENDUR_MINUS, MW_STATS_ENDUR_ADD, MW_STATS_ENDUR_VAL,  MW_STATS_RECO_MINUS, MW_STATS_RECO_ADD, MW_STATS_RECO_VAL,
			MW_STATS_STR_MINUS, MW_STATS_STR_ADD, MW_STATS_STR_VAL, MW_STATS_DEF_MINUS, MW_STATS_DEF_ADD, MW_STATS_DEF_VAL,
			MW_STATS_AGI_MINUS, MW_STATS_AGI_ADD, MW_STATS_AGI_VAL, MW_STATS_LUCK_MINUS, MW_STATS_LUCK_ADD, MW_STATS_LUCK_VAL,
			MW_STATS_OK_BTN, MW_STATS_CANCEL_BTN, MW_STATS_AVAIL_POINT_VAL,
		MW_LOAD_FILE1_BTN, MW_LOAD_FILE2_BTN, MW_LOAD_FILE3_BTN, MW_LOAD_PAGE_RIGHT_BTN, MW_LOAD_PAGE_LEFT_BTN, MW_LOAD_CANCEL_BUTTON};

	Stage p1CurrentStage, p2CurrentStage;

	unsigned int p1PointsAvailable, p2PointsAvailable;
	ProbenderData p1ModdedData, p2ModdedData; //The data values before hitting OK to allow undo

	bool p1ValidCharacter, p2ValidCharacter;//True if the player has a valid character, false if not

	void CreateCharacter(bool player1);

	void ApplyModdedData(bool player1);

	void UpdateAttributeScores(bool player1);

	void ChangeElement(bool player1, ElementEnum::Element newElement);

	void SetSkinsForGroup(bool player1, Stage stage, ElementEnum::Element elementSkin);

	void SetButtonImage(CEGUI::Window* button, ButtonSkinType skinType, ElementEnum::Element newElement);

	std::string GetElementPrefix(ElementEnum::Element newElement);
	void GetButtonSkinSuffix(ButtonSkinType skinType, ButtonSkinListing& outVal);

	void GatherRootWindows();

	void SubscribeToEvents();

	void DisableUnimplementedControls();

	bool IsPlayer1(const CEGUI::WindowEventArgs& e);
	ProbenderAttributes::Attributes GetAttributeFromWindowName(CEGUI::Window* window);

	CEGUI::Window* GetWindow(MenuWindows windowToGet, bool player1 = true);

public:
	CharacterMenuHandler(IScene* scene);
	virtual ~CharacterMenuHandler(void);

	virtual void Update(float gameTime);

	virtual void Show();

	virtual void Hide();

	void SetStage(bool player1, Stage newStage);

#pragma region ButtonEvents

	bool L_ElementButtonClickEvent(const CEGUI::EventArgs& e);
	bool R_ElementButtonClickEvent(const CEGUI::EventArgs& e);

	bool MainDlgCreateCharBtnClickEvent(const CEGUI::EventArgs& e);
	bool MainDlgEditCharBtnClickEvent(const CEGUI::EventArgs& e);
	bool MainDlgLoadCharBtnClickEvent(const CEGUI::EventArgs& e);
	bool MainDlgDoneBtnClickEvent(const CEGUI::EventArgs& e);

	bool StatsMinusBtnClickEvent(const CEGUI::EventArgs& e);
	bool StatsAddBtnClickEvent(const CEGUI::EventArgs& e);
	bool StatsOKBtnClickEvent(const CEGUI::EventArgs& e);
	bool StatsCancelBtnClickEvent(const CEGUI::EventArgs& e);

#pragma endregion

};

