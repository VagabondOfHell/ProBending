#include "XMLWriter.h"
#include "rapidxml_print.hpp"
#include <fstream>

#ifdef _DEBUG
#include "OgreLogManager.h"
#endif // _DEBUG


XMLWriter::XMLWriter(void)
{
	xmlDocument = new XMLDocument();
	rootNode = NULL;
}

XMLWriter::~XMLWriter(void)
{
	if(xmlDocument)
		delete xmlDocument;
}

XMLWriter::XMLNode XMLWriter::CreateXMLNode(const char* nodeName, const char* nodeValue)
{
	XMLNode newNode;

	try
	{
		//Create the node
		newNode = xmlDocument->allocate_node(rapidxml::node_element, nodeName, nodeValue);
	}
	catch (std::bad_alloc e)
	{
#ifdef _DEBUG
		Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage(e.what(), Ogre::LML_CRITICAL);
		printf(e.what());
#endif // _DEBUG

		newNode = NULL;
	}
	catch(std::exception e)
	{
#ifdef _DEBUG
		Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage(e.what(), Ogre::LML_CRITICAL);
		printf(e.what());
#endif // _DEBUG

		newNode = NULL;
	}

	return newNode;
}

XMLWriter::XMLAttribute XMLWriter::CreateXMLAttribute(const char* attName, const char* attValue)
{
	XMLAttribute newAtt;

	try
	{
		newAtt = xmlDocument->allocate_attribute(attName, attValue);
	}
	catch(std::bad_alloc e)
	{
#ifdef _DEBUG
		Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage(e.what(), Ogre::LML_CRITICAL);
		printf(e.what());
#endif // _DEBUG

		newAtt = NULL;
	}
	catch(std::exception e)
	{
#ifdef _DEBUG
		Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage(e.what(), Ogre::LML_CRITICAL);
		printf(e.what());
#endif // _DEBUG

		newAtt = NULL;
	}

	return newAtt;
}

const char* XMLWriter::AllocateString(const char* stringToAlloc)
{
	const char* retVal;

	try
	{
		if(stringToAlloc)//Make sure there is data to allocate
			retVal = xmlDocument->allocate_string(stringToAlloc);
		else
			retVal = NULL;
	}
	catch (std::bad_alloc e)
	{
#ifdef _DEBUG
		Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage(e.what(), Ogre::LML_CRITICAL);
		printf(e.what());
#endif // _DEBUG

		retVal = NULL;
	}
	catch(std::exception e)
	{
#ifdef _DEBUG
		Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage(e.what(), Ogre::LML_CRITICAL);
		printf(e.what());
#endif // _DEBUG

		retVal = NULL;
	}

	return retVal;
}

bool XMLWriter::CreateNode(const std::string& nodeName, const std::string& nodeValue, bool push /*= true*/,
						XMLNode parentNode /*= NULL*/, bool allocateName /*= false*/, bool allocateValue /*= false*/)
{
	using namespace rapidxml;

	//Allocate the strings if necessary
	const char* allocName = allocateName ? AllocateString(nodeName.c_str()) : nodeName.c_str();
	const char* allocValue = allocateValue ? AllocateString(nodeValue.c_str()) : nodeValue.c_str();

	XMLNode newNode = CreateXMLNode(allocName, allocValue);

	//if creation is successful
	if(newNode)
		AppendNode(newNode, parentNode, push);

	return newNode != NULL;
}

bool XMLWriter::CreateNode(const std::string& nodeName, const bool nodeValue, 
		bool push /*= true*/, XMLNode parentNode /*= NULL*/, bool allocateName /*= false*/)
{
	return CreateNode(nodeName, nodeValue ? std::string("T") : std::string("F"),
		push, parentNode, allocateName, true);
}

bool XMLWriter::CreateNode(const char* nodeName, const char* nodeValue, bool push /*= true*/,XMLNode parentNode /*= NULL*/)
{
	const char* allocName = nodeName != NULL ? xmlDocument->allocate_string(nodeName) : NULL;
	const char* allocValue = nodeValue != NULL ? xmlDocument->allocate_string(nodeValue) : NULL;

	XMLNode newNode = NULL;

	//A NULL Value is allowed, a NULL name is not
	if(allocName)
		newNode = CreateXMLNode(allocName, allocValue);

	if(newNode)
		AppendNode(newNode, parentNode, push);

	return newNode != NULL;
}

bool XMLWriter::CreateNode(const char* nodeName, const bool nodeValue, bool push /*= true*/, XMLNode parentNode /*= NULL*/)
{
	return CreateNode(nodeName, nodeValue ? "T" : "F", push, parentNode);
}

bool XMLWriter::CreateNode(const char* nodeName, const float nodeValue, bool push /*= true*/, XMLNode parentNode /*= NULL*/)
{
	return CreateNode(nodeName, std::to_string(nodeValue).c_str(), push, parentNode);
}

bool XMLWriter::CreateNode(const char* nodeName, const __int64 nodeValue, bool push /*= true*/, XMLNode parentNode /*= NULL*/)
{
	return CreateNode(nodeName, std::to_string(nodeValue).c_str(), push, parentNode);
}

void XMLWriter::AppendNode(XMLNode nodeToAppend, XMLNode parentNode /*= NULL*/, bool push /*= true*/)
{
	//If root exists, add to stack
	if(rootNode)
	{
		if(parentNode)
			parentNode->append_node(nodeToAppend);//add to specified node
		else
			GetTop()->append_node(nodeToAppend);//or add to node at top of the work stack
	}
	else
	{
		//otherwise assign root
		rootNode = nodeToAppend;
		xmlDocument->append_node(rootNode);
	}

	if(push)//check to see if it should be placed in the stack
		nodeStack.push(nodeToAppend);
}

bool XMLWriter::AppendAttribute(XMLAttribute attToAppend, XMLNode nodeToAppendTo)
{
	//Append the attribute to the appropriate node
	if(nodeToAppendTo)
	{
		nodeToAppendTo->append_attribute(attToAppend);
		return true;
	}
	else
	{
		GetTop()->append_attribute(attToAppend);
		return true;
	}

	return false;
}

bool XMLWriter::AddAttribute(const std::string& attName, const std::string& attValue, 
		bool allocateName /*= false*/, bool allocateValue /*= false*/, XMLNode nodeToAddTo /*= NULL*/)
{
	using namespace rapidxml;

	//Allocate the strings if necessary
	const char* allocName = allocateName ? AllocateString(attName.c_str()) : attName.c_str();
	const char* allocValue = allocateValue ? AllocateString(attValue.c_str()) : attValue.c_str();

	return AddAttribute(allocName, allocValue, nodeToAddTo);
}

bool XMLWriter::AddAttribute(const std::string& attName, const __int64 attVal, 
			bool allocateName /*= false*/, XMLNode nodeToAddTo /*= NULL*/)
{
	using namespace rapidxml;

	//Allocate the strings if necessary
	const char* allocName = allocateName ? AllocateString(attName.c_str()) : attName.c_str();

	return AddAttribute(allocName, std::to_string(attVal).c_str(), nodeToAddTo);
}

bool XMLWriter::AddAttribute(const std::string& attName, const float attVal, bool allocateName /*= false*/, XMLNode nodeToAddTo /*= NULL*/)
{
	using namespace rapidxml;

	//Allocate the strings if necessary
	const char* allocName = allocateName ? AllocateString(attName.c_str()) : attName.c_str();

	return AddAttribute(allocName, std::to_string(attVal).c_str(), nodeToAddTo);
}

bool XMLWriter::AddAttribute(const std::string& attName, const bool attVal, bool allocateName /*= false*/, XMLNode nodeToAddTo /*= NULL*/)
{
	using namespace rapidxml;

	//Allocate the strings if necessary
	const char* allocName = allocateName ? AllocateString(attName.c_str()) : attName.c_str();

	return AddAttribute(allocName,  attVal ? std::string("T") : std::string("F"), nodeToAddTo);
}

bool XMLWriter::AddAttribute(const char* attName, const char* attVal, XMLNode nodeToAddTo /*= NULL*/)
{
	const char* allocName = AllocateString(attName);
	const char* allocVal = AllocateString(attVal);

	XMLAttribute newAtt = CreateXMLAttribute(allocName, allocVal);

	if(newAtt)
		return AppendAttribute(newAtt, nodeToAddTo);

	return false;
}

bool XMLWriter::AddAttribute(const char* attName, bool attVal, XMLNode nodeToAddTo /*= NULL*/)
{
	return AddAttribute(attName, attVal ? "T" : "F", nodeToAddTo);
}

bool XMLWriter::AddAttribute(const char* attName, float attVal, XMLNode nodeToAddTo /*= NULL*/)
{
	return AddAttribute(attName, std::to_string(attVal).c_str(), nodeToAddTo);
}

bool XMLWriter::AddAttribute(const char* attName, __int64 attVal, XMLNode nodeToAddTo /*= NULL*/)
{
	return AddAttribute(attName, std::to_string(attVal).c_str(), nodeToAddTo);
}

bool XMLWriter::WriteFile(std::string fileName)
{
	std::ofstream fout(fileName);
	fout << *xmlDocument;
	fout.close();
	return true;
}
