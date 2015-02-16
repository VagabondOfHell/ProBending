#include "GUIProgressTracker.h"
#include "Probender.h"
#include "IScene.h"
#include "GUIManager.h"

#include "CEGUI/Window.h"

const float GUIProgressTracker::ARENA_HALF_LENGTH = 13.0f;

GUIProgressTracker::GUIProgressTracker(void)
{
	
}

GUIProgressTracker::~GUIProgressTracker(void)
{
}

bool GUIProgressTracker::Initialize(Probender* _benderToTrack)
{
	benderToTrack = _benderToTrack;

	return LocateTrackerWindow();
}

bool GUIProgressTracker::LocateTrackerWindow()
{
	if(benderToTrack)
	{
		TeamData::ContestantColour colour = benderToTrack->GetColour();

		if(colour == TeamData::INVALID_COLOUR)
			return false;

		std::string trackerString = TeamData::EnumToString(colour) + "ProgressTracker";
		
		trackerWindow = (CEGUI::Window*)benderToTrack->GetOwningScene()->
			GetGUIManager()->GetChildItem("InGameGUIRoot/ProgressBar/" + trackerString);

		if(trackerWindow)
			trackerWindow->setVisible(true);

		return trackerWindow != NULL;
	}

	return false;
}

void GUIProgressTracker::Update(const float gameTime)
{
	if(trackerWindow && benderToTrack)
	{
		Ogre::Vector3 pos = benderToTrack->GetWorldPosition();
		
		float trackerPosX = 0.5f * (pos.x / ARENA_HALF_LENGTH);
	
		trackerWindow->setXPosition(CEGUI::UDim(trackerPosX, 0.0f));
	}
}
