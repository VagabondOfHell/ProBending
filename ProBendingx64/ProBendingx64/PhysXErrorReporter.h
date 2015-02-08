#ifndef ErrorReporter
#define ErrorReporter
#include <foundation\PxErrorCallback.h>
#include <string>
#include <iostream>
#endif

using std::cout;
using std::endl;

class PhysXErrorReporter: public physx::PxErrorCallback
{
public:
	
	///Display the PhysX Error to the console
	virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
    {
		//// error processing implementation
		std::string errorHeading = "";

		switch (code)
		{
			case physx::PxErrorCode::eNO_ERROR:
				errorHeading = "No Error";
				break;
			case physx::PxErrorCode::eDEBUG_INFO:
				errorHeading = "Debug Info";
				break;
			case physx::PxErrorCode::eDEBUG_WARNING:
				errorHeading = "Debug warning";
				break;
			case physx::PxErrorCode::eINVALID_PARAMETER:
				errorHeading = "Invalid Parameter: ";
				break;
			case physx::PxErrorCode::eINVALID_OPERATION:
				errorHeading = "Invalid Operation: ";
				break;
			case physx::PxErrorCode::eOUT_OF_MEMORY:
				errorHeading = "Out Of Memory: ";
				break;
			case physx::PxErrorCode::eINTERNAL_ERROR:
				errorHeading = "Internal Error: ";
				break;
			case physx::PxErrorCode::eABORT:
				errorHeading = "Abort: ";
				break;
			case physx::PxErrorCode::ePERF_WARNING:
				errorHeading = "Performance Warning: ";
				break;
			case physx::PxErrorCode::eMASK_ALL:
				errorHeading = "All Errors: ";
				break;
			default:
				errorHeading = "Nothing to report";
				break;
		}

		//Display the error message and location of error
		cout << errorHeading << message << " \nFile: " << file << " \nLine: " << line << endl;
    }
};