#pragma once
#include <kinect.h>
#include "KinectBody.h"
#include <vector>

class KinectBodyReader
{
	friend class KinectReader;

private:
	 // Body reader
    IBodyFrameReader*			mBodyFrameReader;
	KinectReader*				mKinectReader;

public:
	
	KinectBodyReader(void);
	~KinectBodyReader(void);

	/// <summary>
	///Tells the Kinect which body to check hand states for
	/// </summary>
	/// <param name="bodyTrackingID">The ID to set</param>
	/// <returns>True if successful, false if not</returns>
	bool OverrideHandTracking(UINT64 bodyTrackingID);

	/// <summary>
	///Tells the Kinect which body to check hand states for
	/// </summary>
	/// <param name="oldTrackingID">The ID to replace</param>
	/// <param name="newTrackingID">The ID to set</param>
	/// <returns>True if successful, false if not</returns>
	bool ReplaceAndOverrideHandTracking(UINT64 oldTrackingID, UINT64 newTrackingID);

	HRESULT Capture();
	
	//Gets the Kinect Reader that owns the body reader
	KinectReader* GetKinectReader()const;

	/// <summary>
	///Gets the body index from the provided tracking ID
	/// </summary>
	/// <param name="trackingID">The ID to look for</param>
	/// <returns>The index, or -1 if it wasn't found</returns>	
	int FindBodyIndexFromTrackID(UINT64 trackingID)const;

	/// <summary>
	///Gets the Tracking ID from the specified body index
	/// </summary>
	/// <param name="bodyIndex">The index to look for</param>
	/// <returns>The tracking ID, or 0 if the body isn't tracked</returns>	
	UINT64 FindTrackIDFromBodyIndex(UINT8 bodyIndex)const;

	/// <summary>
	///Checks if the given Body has a Listener
	/// </summary>
	/// <param name="bodyID">The ID to check</param>
	/// <returns>True for having at least one listener, false for none or error</returns>	
	bool CheckBodyIndexHasListener(const UINT8 bodyIndex)const;

	/// <summary>
	///Gets the body from the given index
	/// </summary>
	/// <param name="bodyIndex">The Index to check</param>
	/// <returns>The body pointer, or NULL if failed or not initialized</returns>	
	KinectBody* const GetBodyAtIndex(UINT8 bodyIndex)const;

	/// <summary>
	///Gets the first valid BodyID found
	/// </summary>
	/// <returns>The body ID of the valid body, or -1 for errors</returns>	
	int GetFirstValidBodyIndex()const;

	/// <summary>
	///Gets all valid body ids
	/// </summary>
	/// <returns>A vector containing all valid body ids</returns>	
	std::vector<UINT8> GetAllValidBodyIndices()const;

	/// <summary>
	///Gets the first body ID that doesn't currently have a listener
	/// </summary>
	/// <returns>Body ID of the first found, or -1 for errors or not found</returns>	
	int GetFirstUnlistenedBodyIndex()const;

	/// <summary>
	///Gets all body IDs that do not have listeners attached
	/// </summary>
	/// <returns>Vector representing all unlistened body ids</returns>	
	std::vector<UINT8> GetAllUnlistenedBodyIndices()const;

	/// <summary>
	///Gets the first body ID that is valid and doesn't have a listener
	/// </summary>
	/// <returns>Number of ID for valid, -1 for invalid or error</returns>	
	int GetFirstValidUnlistenedBodyIndex()const;

	/// <summary>
	///Gets all body IDs that are valid and don't have listeners
	/// </summary>
	/// <returns>A vector representing all valid and unlistened bodies</returns>	
	std::vector<UINT8> GetValidUnlistenedBodyIndices()const;
};

