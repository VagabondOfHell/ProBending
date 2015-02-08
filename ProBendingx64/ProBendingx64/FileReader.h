#pragma once
#include <string>

class FileReader
{
public:
	FileReader(void)
	{
	}

	virtual ~FileReader(void)
	{
	}

	virtual bool OpenFile(std::string fileName) = 0;
};