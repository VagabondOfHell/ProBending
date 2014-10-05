#include "KinectGestureDatabase.h"
#include <codecvt>

KinectGestureDatabase* KinectGestureDatabase::instance;
const int KinectGestureDatabase::MaxFileNameSize = 200;

struct KinectGesture
{
	GestureType Type;
	IGesture* GestureInterface;

	KinectGesture()
	{
		GestureInterface = NULL;
	}

	KinectGesture(IGesture* gesture)
	{
		//Get the type out of the gesture
		if(gesture)
		{
			GestureType type;

			HRESULT hr = gesture->get_GestureType(&type);

			if(SUCCEEDED(hr))
			{
				GestureInterface = gesture;
				Type = type;
			}
			else
			{
				Type = GestureType_None;
				GestureInterface = NULL;
			}
		}
	}

	KinectGesture(GestureType type, IGesture* gesture)
	{
		Type = type;
		GestureInterface = gesture;
	}

	///Get the name of the gesture as a Wide String
	std::wstring GetWideName()
	{
		std::wstring name;
		wchar_t fill[KinectGestureDatabase::MaxFileNameSize];

		if(GestureInterface)
		{
			HRESULT hr = GestureInterface->get_Name(sizeof(fill), fill);
			//Fill the wstring with the array results
			name.assign(fill);

			if(SUCCEEDED(hr))
			{
				return name;
			}
		}

		return nullptr;
	}

	static std::wstring GetWideName(IGesture* gestureToName)
	{
		std::wstring name;
		name.reserve(KinectGestureDatabase::MaxFileNameSize);

		if(gestureToName)
		{
			HRESULT hr = gestureToName->get_Name(KinectGestureDatabase::MaxFileNameSize, &name[0]);
			
			if(SUCCEEDED(hr))
			{
				name.resize(name.length());
				return name;
			}
		}

		return nullptr;
	}

	//Gets the name as a string
	std::string GetName()
	{
		return KinectGestureDatabase::WideStringToString(GetWideName());
	}

	
};

KinectGestureDatabase::KinectGestureDatabase()
{
	gesturesCollection = std::map<std::wstring, KinectGesture>();
	database = NULL;
	gestureCount = -1;
}
	
KinectGestureDatabase::~KinectGestureDatabase()
{
	CloseDatabase();
}

std::wstring KinectGestureDatabase::StringToWideString(const std::string& stringToConvert)
{
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(stringToConvert);
}

std::string KinectGestureDatabase::WideStringToString(const std::wstring& stringToConvert)
{
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(stringToConvert);
}

bool KinectGestureDatabase::GetGestures()
{
	UINT gestureCountInSource;

	//Try to get the number of gestures in the database
	HRESULT hr = database->get_AvailableGesturesCount(&gestureCountInSource);
	
	if(FAILED(hr))
	{
		gestureCount = -1;
		return false;
	}
	//Set gesture count
	gestureCount = gestureCountInSource;

	IGesture** gestures = new IGesture*[gestureCount];

	//Try to get the gestures in the database
	hr = database->get_AvailableGestures(gestureCount, gestures);	

	if(SUCCEEDED(hr))
	{
		//For each discovered gesture, add it to the Database's Map
		for (int i = 0; i < gestureCount; i++)
		{
			KinectGesture gesture = KinectGesture(gestures[i]);
			
			AddGestureToMap(gesture);
		}	
	}

	delete gestures;

	return true;
}

bool KinectGestureDatabase::AddGestureToMap(KinectGesture gesture)
{
	//Insert into map
	std::pair<std::map<std::wstring, KinectGesture>::iterator, bool> result = 
		gesturesCollection.insert(std::pair<std::wstring, KinectGesture>(gesture.GetWideName(), gesture));

	return result.second;						
}

bool KinectGestureDatabase::OpenDatabase(std::wstring& filePath)
{
	if(!IsOpen())
	{
		//Try to get the database
		HRESULT hr = CreateVisualGestureBuilderDatabaseInstanceFromFile(filePath.c_str(), &database);

		if(FAILED(hr))
		{
			database = NULL;
			return false;
		}

		//If gathering the gestures failed
		if(!GetGestures())
		{
			//Close the database
			CloseDatabase();
			return false;
		}
	}
	else
		return false;

	return true;
}

bool KinectGestureDatabase::OpenDatabase(std::string& filePath)
{
	//convert to wide characters
	std::wstring widePath = std::wstring(filePath.begin(), filePath.end());

	return OpenDatabase(widePath);
}

void KinectGestureDatabase::CloseDatabase()
{
	if(IsOpen())
	{
		database->Release();
		database = NULL;

		gesturesCollection.clear();
		gestureCount = -1;
	}
}

int KinectGestureDatabase::FillSourceWithAllGestures(KinectGestureReader* reader)
{
	if(reader != NULL)
	{
		std::map<std::wstring, KinectGesture>::const_iterator start = gesturesCollection.begin();
		std::map<std::wstring, KinectGesture>::const_iterator end = gesturesCollection.end();

		int numberAdded = 0;

		for (start; start != end; ++start)
		{
			//Add the gesture to the source
			HRESULT hr = reader->gestureSource->AddGesture(start->second.GestureInterface);
		
			if(SUCCEEDED(hr))
			{
				//Add the gesture to the high-level readers map
				reader->gesturesInSource.insert(std::pair<std::wstring, IGesture*>
					(start->first, start->second.GestureInterface));

				++numberAdded;
			}
		}
		return numberAdded;
	}

	return 0;
}

int KinectGestureDatabase::FillSourceWithGestures(KinectGestureReader* reader, std::vector<std::wstring>& gestureNames)const
{
	if(reader != NULL)
	{
		std::map<std::wstring, KinectGesture>::const_iterator findResult;

		int numberAdded = 0;

		for (int i = 0; i < gestureNames.size(); i++)
		{
			//Find the name in the map
			findResult = gesturesCollection.find(gestureNames[i]);

			if(findResult != gesturesCollection.end())
			{
				//Add the gesture to the source
				HRESULT hr = reader->gestureSource->AddGesture(findResult->second.GestureInterface);
		
				if(SUCCEEDED(hr))
				{
					//Add the gesture to the high-level readers map
					reader->gesturesInSource.insert(std::pair<std::wstring, IGesture*>
						(findResult->first, findResult->second.GestureInterface));

					++numberAdded;
				}
			}
		}

		return numberAdded;
	}

	return 0;
}

int KinectGestureDatabase::FillSourceWithGestures(KinectGestureReader* reader, std::vector<std::string>& gestureNames)const
{
	if(reader != NULL)
	{
		std::wstring wideGestureName;

		int numberAdded = 0;

		//Loop through and convert each name to a wide string and add it to the source
		for (int i = 0; i < gestureNames.size(); i++)
		{
			wideGestureName = std::wstring(gestureNames[i].begin(), gestureNames[i].end());

			if(FillSourceWithGesture(reader, wideGestureName))
				++numberAdded;
		}
		return numberAdded;
	}

	return 0;
}

bool KinectGestureDatabase::FillSourceWithGesture(KinectGestureReader* reader, std::wstring& gestureName)const
{
	if(reader != NULL)
	{
		std::map<std::wstring, KinectGesture>::const_iterator findResult = gesturesCollection.find(gestureName);

		if(findResult != gesturesCollection.end())
		{
			//Add the gesture to the source
			HRESULT hr = reader->gestureSource->AddGesture(findResult->second.GestureInterface);
		
			if(SUCCEEDED(hr))
			{
				//Add the gesture to the high-level readers map
				reader->gesturesInSource.insert(std::pair<std::wstring, IGesture*>
					(findResult->first, findResult->second.GestureInterface));

				return true;
			}
		}
	}
	return false;
}

bool KinectGestureDatabase::FillSourceWithGesture(KinectGestureReader* reader, std::string& gestureName)const
{
	if(reader != NULL)
	{
		std::wstring wideGestureName = std::wstring(gestureName.begin(), gestureName.end());

		return FillSourceWithGesture(reader, wideGestureName);
	}

	return false;
}

int KinectGestureDatabase::FillSourceWithGestureOfType(KinectGestureReader* reader, GestureType gestureType)const
{
	if(reader != NULL)
	{
		std::map<std::wstring, KinectGesture>::const_iterator start = gesturesCollection.begin();
		std::map<std::wstring, KinectGesture>::const_iterator end = gesturesCollection.end();

		int numberAdded = 0;

		for (start; start != end; ++start)
		{
			if(start->second.Type == gestureType)
			{
				//Add the gesture to the source
				HRESULT hr = reader->gestureSource->AddGesture(start->second.GestureInterface);
		
				if(SUCCEEDED(hr))
				{
					//Add the gesture to the high-level readers map
					reader->gesturesInSource.insert(std::pair<std::wstring, IGesture*>
						(start->first, start->second.GestureInterface));

					++numberAdded;
				}
			}
		}

		return numberAdded;
	}
	return 0;
}