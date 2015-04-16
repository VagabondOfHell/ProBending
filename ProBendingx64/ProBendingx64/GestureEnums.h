#pragma once

namespace GestureEnums
{
	enum TransitionRules{TRANRULE_NONE, TRANRULE_SAME, TRANRULE_OPPOSITE};
	enum BodySide{BODYSIDE_INVALID, BODYSIDE_LEFT, BODYSIDE_RIGHT, BODYSIDE_EITHER, BODYSIDE_BOTH};
	enum GUIGestureSlot{P1_GESTURE_SLOT_1, P1_GESTURE_SLOT_2, P1_GESTURE_SLOT_3, 
		P2_GESTURE_SLOT_1, P2_GESTURE_SLOT_2, P2_GESTURE_SLOT_3, INVALID_GESTURE_SLOT};

	static const std::string PROBEND_ARENA_GUI_PATH = "ProbendArenaGUI/";
	static const std::string PROBEND_GUI_ROOT_PATH = "InGameGUIRoot/";

	static const std::string EARTH_STOMP_BEGIN_IMAGE = PROBEND_ARENA_GUI_PATH + "EarthStompBegin";
	static const std::string EARTH_STOMP_END_IMAGE = PROBEND_ARENA_GUI_PATH + "EarthStompEnd";
	static const std::string EARTH_STOMP_LAUNCH_IMAGE = PROBEND_ARENA_GUI_PATH + "EarthStompLaunch";

	static const std::string FIRE_BLAST_BEGIN_IMAGE = PROBEND_ARENA_GUI_PATH + "FireBlastBegin";
	static const std::string FIRE_BLAST_END_IMAGE = PROBEND_ARENA_GUI_PATH + "FireBlastEnd";
	static const std::string FIRE_BLAST_LAUNCH_IMAGE = PROBEND_ARENA_GUI_PATH + "FireBlastLaunch";

	static const std::string WATER_RISE_BEGIN_IMAGE = PROBEND_ARENA_GUI_PATH + "WaterRiseBegin";
	static const std::string WATER_RISE_END_IMAGE = PROBEND_ARENA_GUI_PATH + "WaterRiseEnd";
	static const std::string WATER_RISE_LAUNCH_IMAGE = PROBEND_ARENA_GUI_PATH + "WaterRiseLaunch";

	static const std::string GUI_GESTURE_SLOT_PATHS[] = {
		PROBEND_GUI_ROOT_PATH + "P1GestureRoot/Gesture1",
		PROBEND_GUI_ROOT_PATH + "P1GestureRoot/Gesture2",
		PROBEND_GUI_ROOT_PATH + "P1GestureRoot/Gesture3",
		PROBEND_GUI_ROOT_PATH + "P2GestureRoot/Gesture1",
		PROBEND_GUI_ROOT_PATH + "P2GestureRoot/Gesture2",
		PROBEND_GUI_ROOT_PATH + "P2GestureRoot/Gesture3",
	};
};