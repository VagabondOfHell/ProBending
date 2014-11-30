#pragma once
#include <string>
#include <exception>

class NotImplementedException
	:public std::exception
{
private:
     std::string errorMessage;
	
public:
    // Construct with given error message:
    NotImplementedException(const std::string& error = "Functionality not yet implemented!")
    {
        errorMessage = error;
    }

    // Provided for compatibility with std::exception.
    const char * what() const 
    {
        return errorMessage.c_str();
    }


};