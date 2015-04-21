#include "RadialProgressBar.h"

#include "CEGUI/Window.h"

RadialProgressBar::RadialProgressBar(void)
	: window(nullptr), progress(0)
{
}

RadialProgressBar::RadialProgressBar(CEGUI::Window* _window)
	: window(_window), progress(0)
{

}

RadialProgressBar::~RadialProgressBar(void)
{

}

void RadialProgressBar::UpdateWindow()
{
	if(window)
	{
		std::string imageName = "";

		if(progress <= 11)
			imageName = "MenuItems/Radial_Bar_1";
		else if(progress > 11 && progress <= 22)
			imageName = "MenuItems/Radial_Bar_2";
		else if(progress > 22 && progress <= 33)
			imageName = "MenuItems/Radial_Bar_3";
		else if(progress > 33 && progress <= 44)
			imageName = "MenuItems/Radial_Bar_4";
		else if(progress > 44 && progress <= 55)
			imageName = "MenuItems/Radial_Bar_5";
		else if(progress > 55 && progress <= 66)
			imageName = "MenuItems/Radial_Bar_6";
		else if(progress > 66 && progress <= 77)
			imageName = "MenuItems/Radial_Bar_7";
		else if(progress > 77 && progress <= 89)
			imageName = "MenuItems/Radial_Bar_8";
		else if(progress > 89)
			imageName = "MenuItems/Radial_Bar_9";

		if(!currImageName.empty())
		{
			if(currImageName[21] != imageName[21])
				window->setProperty("Image", imageName);
		}
		else
			window->setProperty("Image", imageName);

		currImageName = imageName;
	}
}

void RadialProgressBar::IncrementProgress(unsigned int value)
{
	progress += value;
	if(progress > 100)
		progress = 100;

	UpdateWindow();
}

void RadialProgressBar::DecrementProgress(unsigned int value)
{
	if(value >= progress)
		progress = 0;
	else
		progress -= value;

	UpdateWindow();
}

void RadialProgressBar::SetProgress(unsigned int value)
{
	if(value > 100)
		progress = 100;
	else
		progress = value;

	UpdateWindow();
}

void RadialProgressBar::Reset()
{
	progress = 0;
	UpdateWindow();
}

void RadialProgressBar::Show()
{
	if(window)
		window->setVisible(true);
}

void RadialProgressBar::Hide()
{
	if(window)
		window->setVisible(false);
}

void RadialProgressBar::SetAbsolutePosition(float x, float y)
{
	if(window)
	{
		window->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, x), CEGUI::UDim(0.0f, y)));
	}
}
