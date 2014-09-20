#pragma once
#include "InputObserver.h"
#include <vector>

class InputNotifier: public OIS::MouseListener, public OIS::KeyListener
{
private:
	std::vector<InputObserver*> observers;
	bool deleteObserversOnClose;

public:
	//InputNotifier Constructor
	//<Param: _deleteObserversOnClose> When the Input Notifier is deleted
	//if this is true it will delete all observers as well. 
	InputNotifier(bool _deleteObserversOnClose = false);

	~InputNotifier(void);

	//Adds an observer
	//<Param: observer> The observer to add. Will not add if already in list
	void AddObserver(InputObserver* observer);

	//Removes an observer
	//<Param: observer> The observer to remove
	void RemoveObserver(InputObserver* observer);

	//Clears all observers (does not delete the observers)
	void ClearObservers();

	//Clears the list of observers, deleting them in the process.
	//If you don't want the observers deleted, only removed, call ClearObservers instead
	void DeleteObservers();

	virtual bool keyDown(const OIS::KeyEvent &arg);
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );
	
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

};

