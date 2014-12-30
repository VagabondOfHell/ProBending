#include "Arena.h"
#include "Probender.h"
#include "GUIManager.h"
#include "IScene.h"
#include "AbilityPrototypeDatabase.h"
#include "AbilityManager.h"
#include "ProjectileManager.h"
#include "XMLWriterTest.h"
#include "XMLReaderTest.h"

Arena::Arena(IScene* _owningScene, std::string _arenaName)
{
	owningScene = _owningScene;
}

Arena::~Arena(void)
{
	if(contestants)
	{
		delete[] contestants;
	}

	if(projectileManager)
		delete projectileManager;

	if(abilityManager)
		delete abilityManager;
}

void Arena::Initialize(const std::vector<ProbenderData> contestantData)
{
	contestantCount = contestantData.size();
	contestants = new Probender[contestantCount];
	
	ElementFlags::ElementFlag elementsToLoad = 0; 

	//Loop and initialize each character
	for (int i = 0; i < contestantCount; i++)
	{
		contestants[i] = Probender(i, this);
		contestants[i].CreateInGameData(contestantData[i]);

		elementsToLoad |= ElementFlags::EnumToFlags(contestantData[i].Attributes.MainElement);
	}

	elementsToLoad |= ElementFlags::Earth;//Add for prototype demo

	projectileManager = new ProjectileManager(owningScene);
	
	abilityManager = new AbilityManager();

	//Load all required element abilities for the database
	abilityManager->Initialize(elementsToLoad);

}

void Arena::Start()
{
	for (int i = 0; i < contestantCount; ++i)
	{
		contestants[i].AttachToScene(owningScene);
		contestants[i].SetInputState(Probender::Listen);
	}

	using namespace CEGUI;

	label = owningScene->GetGUIManager()->CreateGUIButton("TaharezLook/Button", "ElementDisplay", 
		ElementEnum::EnumToString(contestants[0].GetInGameData().GetMainElement()), 
		UVector2(UDim(0, 0), UDim(0, 0)), USize(UDim(0.1, 0.0), UDim(0.05, 0.0)));

	/*XMLWriterTest test;
	test.RunTest();*/

	XMLReaderTest test;
	test.RunTest();
}

bool Arena::Update(const float gameTime)
{
	for (int i = 0; i < contestantCount; i++)
	{
		contestants[i].Update(gameTime);
	}

	label->setText(ElementEnum::EnumToString(contestants[0].GetCurrentElement()));

	//Update the projectile manager
	projectileManager->Update(gameTime);

	//Update the ability manager
	abilityManager->Update(gameTime);

	return true;
}
