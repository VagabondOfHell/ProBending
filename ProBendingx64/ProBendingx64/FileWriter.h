#pragma once
#include <string>

class FileWriter
{
public:

	FileWriter(void)
	{
	}

	virtual ~FileWriter(void)
	{
	}

	virtual bool WriteFile(std::string fileName) = 0;
};

