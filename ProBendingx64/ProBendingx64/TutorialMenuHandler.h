#pragma once
#include "menuhandler.h"

#include <vector>

class TutorialMenuHandler :
	public MenuHandler
{
	struct TutorialPage
	{
		static const int NUM_IMAGES = 5;
		std::string ImagesetName;
		std::string Images[NUM_IMAGES];
		std::string Text;
		TutorialPage* Previous, *Next;

		TutorialPage():Previous(NULL), Next(NULL)
		{

		}
	};

protected:
	enum CurrentPageDirectory{General, Fire, Earth, Water};

	std::vector<TutorialPage> generalPages;
	std::vector<TutorialPage> firePages;
	std::vector<TutorialPage> earthPages;
	std::vector<TutorialPage> waterPages;

	TutorialPage *currentPage;
	CurrentPageDirectory currDirectory;
	bool atEnd;

	CEGUI::Window *DialogRoot;

	enum MenuWindows{
		MW_ROOT_WINDOW, MW_DIALOG_ROOT, 
		MW_IMAGE_1, MW_IMAGE_2, MW_IMAGE_3, MW_IMAGE_4, MW_IMAGE_5,
		MW_TEXT, MW_NEXT, MW_PREV, MW_GENERAL, MW_EARTH, MW_FIRE, MW_WATER, MW_EXIT
	};

	void CreateTutorialPages();

	void GetRootWindows();

	CEGUI::Window* GetWindow(const MenuWindows window);

	void UpdateDisplay();

	void RegisterButtonEvents();
	void UnregisterButtonEvents();

public:
	TutorialMenuHandler(IScene* scene);
	virtual ~TutorialMenuHandler(void);

	virtual void Update(float gameTime);

	bool NextBtnClickEvent(const CEGUI::EventArgs& e);
	bool PrevBtnClickEvent(const CEGUI::EventArgs& e);
	bool ExitBtnClickEvent(const CEGUI::EventArgs& e);
	bool PageBtnClickEvent(const CEGUI::EventArgs& e);

	virtual void ReceiveAudioInput(const AudioData* audioText);

	virtual void Show();

};

