#include "XMLReader.h"
#include "rapidxml_utils.hpp"
#include "OgreLogManager.h"
#include <string>

XMLReader::XMLReader(void)
{
}


XMLReader::~XMLReader(void)
{
	if(xmlDocument)
		delete xmlDocument;

	if(xmlFile)
		delete xmlFile;
}

void XMLReader::SetTopNode(XMLNode nodeToSet)
{
	if(nodeToSet)
		topNode = nodeToSet;
	else
		topNode = nodeStack.top();

	attribute = topNode->first_attribute();

	hasAttributes = attribute != NULL; //Indicate if this node has attributes
}

bool XMLReader::OpenFile(std::string fileName)
{
	try
	{
		xmlFile = new XMLFile(fileName.c_str());
	}
	//Only check for exception because Bad Alloc and Runtime Error inherit from exception and we aren't doing
	//any special per-case error handling
	catch(std::exception e)
	{
#ifdef _DEBUG
		Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage(e.what(), Ogre::LML_CRITICAL);
#endif // _DEBUG

		if(xmlFile)
		{
			delete xmlFile;
			xmlFile = NULL;
		}
		return false;
	}

	if(xmlFile)
		if(xmlFile->size() > 0 && xmlFile->data())
		{
			try
			{
				xmlDocument = new XMLDocument();
				//Parse no data nodes so that children will equal NULL if there are no children (otherwise blank nodes are created 
				//for nodes without children)
				xmlDocument->parse<rapidxml::parse_no_data_nodes>(xmlFile->data());
			}
			//We only check for exception because rapid xml Parse Error inherits std::exception and
			//we don't need special per-case error handling
			catch(std::exception e)
			{
#ifdef _DEBUG
				Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage(std::string("XML Reader Open File Error: ").append(e.what()), Ogre::LML_CRITICAL);
#endif // _DEBUG
				if(xmlDocument)
				{
					delete xmlDocument;
					xmlDocument = NULL;
				}

				if(xmlFile)
				{
					delete xmlFile;
					xmlFile = NULL;
				}

				return false;
			}
			
			rootNode = xmlDocument->first_node();//if everything loaded, set the root node
			highLevelNode = rootNode->first_node();//if everything loaded, set the first node under the root
			
			//Add both to the stack
			nodeStack.push(rootNode);
			nodeStack.push(highLevelNode);

			SetTopNode();
			
			return true;
		}

	return false;
}

void XMLReader::ResetToBeginning()
{
	while (nodeStack.top() != highLevelNode)
	{
		nodeStack.pop();
	}

	SetTopNode();//indicate current node (should always be high level node by this point)
}

bool XMLReader::MoveToChildNode()
{
	XMLNode child = topNode->first_node();//Grab the child

	if(child)//If valid
	{
		//Add to stack and save the top
		nodeStack.push(child);
		SetTopNode();
	}

	//Return that its not null
	return child != NULL;
}

bool XMLReader::MoveToNextSiblingNode()
{
	XMLNode sibling = topNode->next_sibling();

	if(sibling)
	{
		//Set the top of the stack to the sibling so if we go into children, 
		//when we pop we are at last used sibling
		nodeStack.top() = sibling; 
	
		SetTopNode(sibling);
	}
	return sibling != NULL;
}

bool XMLReader::MoveToPrevSiblingNode()
{
	XMLNode sibling = topNode->previous_sibling();

	if(sibling)
	{
		//Set the top of the stack to the sibling so if we go into children, 
		//when we pop we are at last used sibling
		nodeStack.top() = sibling; 

		SetTopNode(sibling);
	}
	return sibling != NULL;
}

std::string XMLReader::GetCurrentNodeName() const
{
	return topNode->name();
}

std::string XMLReader::GetStringValue(bool attributeVal)
{
	if(!attributeVal)
		return topNode->value();
	else
		//Dont check if attribute is NULL, up to client to check before calling
		return attribute->value();
}

bool XMLReader::GetFloatValue(float& outVal, bool attributeVal)
{
	float val;
	try
	{
		if(!attributeVal)
			val = std::stof(topNode->value());
		else
			//dont check for null because exceptions will catch it
			val = std::stof(attribute->value());
	}
	catch(std::exception e)
	{
		outVal = 0.0f;
		return false;
	}

	outVal = val;
	return true;	
}

bool XMLReader::GetBoolValue(bool& outVal, bool attributeVal)
{
	std::string val;
	
	if(!attributeVal)
		val = topNode->value();
	else
		val = attribute->value();

	if(val == "T" || val == "TRUE" || val == "true")
	{
		outVal = true;
		return true;
	}
	else if(val == "F" || val == "FALSE" || val == "false")
	{
		outVal = false;
		return true;
	}

	return false;
}

bool XMLReader::GetIntValue(int& outVal, bool attributeVal)
{
	int val;

	try
	{
		if(!attributeVal)
			val = std::stoi(topNode->value());
		else
			val = std::stoi(attribute->value());
	}
	catch(std::exception e)
	{
		outVal = 0;
		return false;
	}

	outVal = val;

	return true;
	
}

bool XMLReader::MoveToNextAttribute()
{
	if(attribute)
		attribute = attribute->next_attribute();

	return attribute != NULL;
}

std::string XMLReader::GetCurrentAttributeName() const
{
	if(attribute)
		return attribute->name();
	else
		return "";
}

void XMLReader::ResetToFirstAttribute()
{
	attribute = topNode->first_attribute();
}
