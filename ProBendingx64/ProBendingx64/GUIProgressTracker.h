#pragma once

class Probender;

namespace CEGUI
{
	class Window;
};

class GUIProgressTracker
{
private:
	CEGUI::Window* trackerWindow;
	Probender* benderToTrack;

	bool LocateTrackerWindow();

	static const float ARENA_HALF_LENGTH;//half length of the arena on the X-axis

public:
	GUIProgressTracker(void);
	~GUIProgressTracker(void);

	bool Initialize(Probender* _benderToTrack);

	void Update(const float gameTime);
};

