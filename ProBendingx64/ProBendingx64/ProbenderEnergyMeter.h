#pragma once
#include <string>

namespace CEGUI
{
	class Window;
};

class GUIManager;

class ProbenderEnergyMeter
{
private:
	CEGUI::Window* healthBar;

public:
	ProbenderEnergyMeter(void);
	~ProbenderEnergyMeter(void);

	void Initialize(GUIManager* guiManager, unsigned short contestantID);

	void SetValue(float currVal, float maxVal = 100.0f);

};

