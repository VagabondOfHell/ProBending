#include "InputNotifier.h"

InputNotifier* InputNotifier::instance = NULL;

InputNotifier::InputNotifier(bool _deleteObserversOnClose )
{
	observers = std::vector<InputObserver*>();
	observers.reserve(5);

	deleteObserversOnClose = _deleteObserversOnClose;
}

InputNotifier::~InputNotifier(void)
{
	//If we are to delete all observers on close
	if(deleteObserversOnClose)
	{
		DeleteObservers();
	}
}

InputNotifier* InputNotifier::GetInstance()
{
	if(!instance)
		instance = new InputNotifier(false);

	return instance;
}

void InputNotifier::DestroySingleton()
{
	if(instance)
	{
		delete instance;
		instance = NULL;
	}
}

void InputNotifier::AddObserver(InputObserver* observer)
{
	for (unsigned int i = 0; i < observers.size(); i++)
	{
		if(observer == observers[i])
			return;
	}

	observers.push_back(observer);
}

///////////////////////NEEDS TESTING//////////////////////////////////
void InputNotifier::RemoveObserver(InputObserver* observer)
{
	for (int i = 0; i < observers.size(); i++)
	{
		if(observers[i] == observer)
		{
			observers.erase(observers.begin() + i);
			return;
		}
	}
}

void InputNotifier::ClearObservers()
{
	observers.clear();
}

void InputNotifier::DeleteObservers()
{
	//Loop through and delete all observers. Don't worry
	//about removing them from the vector, as the vector will be deleted too
	for (int i = 0; i < observers.size(); i++)
	{
		if(observers[i])
		{
			delete observers[i];
			observers[i] = nullptr;
		}
	}

	observers.clear();
}

bool InputNotifier::keyDown(const OIS::KeyEvent &arg)
{
	for (unsigned int i = 0; i < observers.size(); i++)
	{
		observers[i]->keyDown(arg);
	}

	return true;
}

bool InputNotifier::keyPressed( const OIS::KeyEvent &arg )
{
	for (unsigned int i = 0; i < observers.size(); i++)
	{
		observers[i]->keyPressed(arg);
	}

	return true;
}

bool InputNotifier::keyReleased( const OIS::KeyEvent &arg )
{
	for (int i = 0; i < observers.size(); i++)
	{
		observers[i]->keyReleased(arg);
	}

	return true;
}

bool InputNotifier::mouseMoved( const OIS::MouseEvent &arg )
{
	for (unsigned int i = 0; i < observers.size(); i++)
	{
		observers[i]->mouseMoved(arg);
	}

	return true;
}

bool InputNotifier::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	for (unsigned int i = 0; i < observers.size(); i++)
	{
		observers[i]->mousePressed(arg, id);
	}

	return true;
}

bool InputNotifier::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	for (int i = 0; i < observers.size(); i++)
	{
		observers[i]->mouseReleased(arg, id);
	}

	return true;
}
