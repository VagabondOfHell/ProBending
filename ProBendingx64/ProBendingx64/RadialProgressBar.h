#pragma once

#include <string>

namespace CEGUI
{
	class Window;
};

class RadialProgressBar
{
private:

	CEGUI::Window* window;
	unsigned int progress;

	std::string currImageName;

	void UpdateWindow();

public:
	RadialProgressBar(void);
	RadialProgressBar(CEGUI::Window* _window);

	~RadialProgressBar(void);

	void SetWindow(CEGUI::Window* newWindow){window = newWindow;}
	CEGUI::Window* GetWindow()const{return window;}

	void Show();
	void Hide();

	void IncrementProgress(unsigned int value);
	void DecrementProgress(unsigned int value);
	void SetProgress(unsigned int value);

	void SetAbsolutePosition(float x, float y);

	void Reset();
};

