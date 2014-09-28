#pragma once 
#include <Kinect.VisualGestureBuilder.h>
#include "KinectGestureReader.h"
#include <string>
#include <vector>
#include <map>
#include <locale>
#include <codecvt>

struct KinectGesture;

class KinectGestureDatabase
{
private:
	std::map<std::wstring, KinectGesture> gesturesCollection;

	static KinectGestureDatabase* instance;
	
	IVisualGestureBuilderDatabase* database;
	int gestureCount;

	KinectGestureDatabase();
	
	/// <summary>
	/// Loads the specified gestures into the Gesture Database
	///
	///REQUIRES TESTING - SPECIFICALLY MEMORY DE-ALLOCATION
	/// </summary>
	/// <returns>True if successful, false if not</returns>
	bool GetGestures();

	/// <summary>
	/// Adds the specified gesture to the Source and Map
	///
	///REQUIRES TESTING - SPECIFICALLY MEMORY DE-ALLOCATION
	/// </summary>
	/// <param name="gesture">The gesture to add. Must be completely filled</param>
	/// <returns>True if inserted, false if not</returns>
	inline bool AddGestureToMap(KinectGesture gesture);

public:
	
	~KinectGestureDatabase();

	static KinectGestureDatabase* GetInstance();

	void DestroySingleton();

	///The maximum file name size in character count to expect
	static const int MaxFileNameSize;

	bool OpenDatabase(std::wstring& filePath);
	bool OpenDatabase(std::string& filePath);

	//Do we need to call release on each gesture here?
	void CloseDatabase();

	bool IsOpen()const;

	/// <summary>
	/// Converts the specifed string to its Wide String equivalent
	/// </summary>
	/// <param name="stringtoConvert">The string to convert</param>
	/// <returns>A wide string representing the same text</returns>
	static std::wstring StringToWideString(const std::string& stringtoConvert);

	/// <summary>
	/// Converts the specifed wide string to its string equivalent
	/// </summary>
	/// <param name="stringtoConvert">The wide string to convert</param>
	/// <returns>A string representing the same text</returns>
	static std::string WideStringToString(const std::wstring& stringToConvert);

	/// <summary>
	/// Fills the specified source with all currently loaded gestures. Will not work
	///if no gestures have been loaded
	///REQUIRES TESTING - SPECIFICALLY MEMORY DE-ALLOCATION
	/// </summary>
	/// <param name="reader">The Gesture Reader to fill with gestures</param>
	/// <returns>The number of gestures added</returns>
	int FillSourceWithAllGestures(KinectGestureReader* reader);

	/// <summary>
	/// Fills the specified source with all gestures that match the names as those passed in the vector parameter
	///REQUIRES TESTING - SPECIFICALLY MEMORY DE-ALLOCATION
	/// </summary>
	/// <param name="reader">The kinect gesture reader to fill with gesture data</param>
	/// <param name="gestureNames">A vector of gesture names representing the ones to add</param>
	/// <returns>The number of gestures added</returns>
	int FillSourceWithGestures(KinectGestureReader* reader, std::vector<std::wstring>& gestureNames)const;

	/// <summary>
	/// Fills the specified source with all gestures that match the names as those passed in the vector parameter
	///REQUIRES TESTING - SPECIFICALLY MEMORY DE-ALLOCATION
	/// </summary>
	/// <param name="reader">The kinect gesture reader to fill with gesture data</param>
	/// <param name="gestureNames">A vector of gesture names representing the ones to add</param>
	/// <returns>The number of gestures added</returns>
	int FillSourceWithGestures(KinectGestureReader* reader, std::vector<std::string>& gestureNames)const;

	/// <summary>
	/// Places the specified gesture into the source
	///REQUIRES TESTING - SPECIFICALLY MEMORY DE-ALLOCATION
	/// </summary>
	/// <param name="reader">The kinect gesture reader to fill with gesture data</param>
	/// <param name="gestureName">The name of the gesture to add</param>
	/// <returns>True if successful, false if not</returns>
	bool FillSourceWithGesture(KinectGestureReader* reader, std::wstring& gestureName)const;

	/// <summary>
	/// Places the specified gesture into the source
	///REQUIRES TESTING - SPECIFICALLY MEMORY DE-ALLOCATION
	/// </summary>
	/// <param name="reader">The kinect gesture reader to fill with gesture data</param>
	/// <param name="gestureName">The name of the gesture to add</param>
	/// <returns>True if successful, false if not</returns>
	bool FillSourceWithGesture(KinectGestureReader* reader, std::string& gestureName)const;

	/// <summary>
	/// Places the gestures of the matching type into the source
	///REQUIRES TESTING - SPECIFICALLY MEMORY DE-ALLOCATION
	/// </summary>
	/// <param name="reader">The kinect gesture reader to fill with gesture data</param>
	/// <param name="gestureType">The type of the gesture to add</param>
	/// <returns>Number of gestures added</returns>
	int FillSourceWithGestureOfType(KinectGestureReader* reader, GestureType gestureType)const;
};

