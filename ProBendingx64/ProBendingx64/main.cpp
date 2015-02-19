#pragma once
#define MEMORY_LEAK_DETECT 0

#if(MEMORY_LEAK_DETECT)
#include "vld.h"
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "Game.h"



#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
		Game game = Game();

        try {
			game.Run();

        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
			game.CloseGame();
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
		catch(std::exception e)
		{
			MessageBox( NULL, e.what(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
			game.CloseGame();
		}

        return 0;
    }

#ifdef __cplusplus
}
#endif