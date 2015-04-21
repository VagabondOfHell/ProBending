#include "ProbenderEnergyMeter.h"

#include "GUIManager.h"
#include "CEGUI/Window.h"

ProbenderEnergyMeter::ProbenderEnergyMeter(void)
{
	healthBar = nullptr;
}

ProbenderEnergyMeter::~ProbenderEnergyMeter(void)
{
}

void ProbenderEnergyMeter::Initialize(GUIManager* guiManager, unsigned short contestantID)
{
	if(contestantID == 0)
	{
		healthBar = guiManager->GetChildWindow("InGameGUIRoot/P1HealthRoot/HealthParent");
	}
	else
	{
		healthBar = guiManager->GetChildWindow("InGameGUIRoot/P2HealthRoot/HealthParent");
	}
}

void ProbenderEnergyMeter::SetValue(float currVal, float maxVal /*= 100.0f*/)
{
	if(healthBar)
	{
		healthBar->setSize(CEGUI::USize(CEGUI::UDim(currVal / maxVal, 0.0f), CEGUI::UDim(1.0f, 0.0f)));
		healthBar->getChild("HealthImage")->setSize(CEGUI::USize(CEGUI::UDim(0.0f, 350.0f), CEGUI::UDim(1.0f, 0.0f)));
	}
}

