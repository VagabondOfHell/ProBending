#pragma once
#include "FileReader.h"
#include <stack>

namespace rapidxml
{
	template<class Ch>class file;
	// Forward declarations
	template<class Ch> class xml_node;
	template<class Ch> class xml_attribute;
	template<class Ch> class xml_document;
};

class XMLReader :
	public FileReader
{
public:
	typedef rapidxml::file<char> XMLFile;
	typedef rapidxml::xml_node<char>* XMLNode;
	typedef rapidxml::xml_attribute<char>* XMLAttribute;
	typedef rapidxml::xml_document<char> XMLDocument;

protected:
	XMLFile* xmlFile;
	XMLDocument* xmlDocument;

	XMLNode rootNode;//The root node
	XMLNode highLevelNode;//The node at the highest level under the root node
	XMLNode topNode;//The node currently at the top of the stack

	//Stack to hold the nodes in. Nodes are added to the stack when searching for children
	//When looking for siblings the top node will be replaced with the value
	std::stack<XMLNode> nodeStack;

	XMLAttribute attribute;//The current attribute viewed. Changes every time the top node is modified
	bool hasAttributes;//Bool to hold whether or not the current node has attributes

	///<summary>Used to switch the top node</summary>
	///<param name="nodeToSet">The node to set the top node to, or NULL to set to the top of the stack</param>
	void SetTopNode(XMLNode nodeToSet = NULL);

public:
	XMLReader(void);
	virtual ~XMLReader(void);

	///<summary>Opens an XML File</summary>
	///<param name="fileName">The name of the file to be loaded. Must be an XML file</param>
	///<returns>True if successful, false if not</returns>
	virtual bool OpenFile(std::string fileName);

	///<summary>Resets the stack to the first node under the Root</summary>
	void ResetToBeginning();

	///<summary>Gets the root node of the XML File</summary>
	///<returns>The root node. Any modifications could affect the reader</returns>
	inline XMLNode GetRootNode()const {return rootNode;}

	///<summary>Provide access to the node in case it's needed</summary>
	///<returns>The current node. Changing the value could result in the reader not working properly</returns>
	inline XMLNode GetCurrentNode()const{return topNode;}

	///<summary>Provide access to the attribute in case it's needed</summary>
	///<returns>The current attribute, or NULL if none. Changing the value could result in the reader not working properly</returns>
	inline XMLAttribute GetCurrentAttribute()const{return attribute;}

	///<summary>Checks if the current node has at least one attribute</summary>
	///<returns>True if there is an attribute, false if not</returns>
	inline bool NodeHasAttributes()const{return hasAttributes;}

	///<summary>Resets the attribute iterator to the first attribute of the current node</summary>
	void ResetToFirstAttribute();

	///<summary>Pops a child off the stack, but never passed the first child of the root</summary>
	inline void PopNode()
	{
		if(nodeStack.size() > 2)//if there are more nodes then the root and its child, allow the pop
		{
			nodeStack.pop();
			SetTopNode(nodeStack.top());
		}
	}

	///<summary>Get the string representing the name of the currently viewed node</summary>
	///<returns>String of the nodes' name</returns>
	std::string GetCurrentNodeName()const;
	
	///<summary>Gets the name of the current attribute</summary>
	///<returns>The name of the attribute, or "" if no attribute available</returns>
	std::string GetCurrentAttributeName()const;

	///<summary>Gets the value as a string</summary>
	///<param name="attributeVal">True to get current attribute value, false to get current node value</param>
	///<returns>The value as a string. If the string is empty, there was no value. NULL reference exception if
	///attributeVal is true but there is no valid attribute to retrieve info from</returns>
	std::string GetStringValue(bool attributeVal = false);
	
	///<summary>Gets the value as a float</summary>
	///<param name="outVal">The float to place the result in</param>
	///<param name="attributeVal">True to get current attribute value, false to get current node value</param>
	///<returns>True if successful, false if conversion issues. If false, it is not recommended to use the value</returns>
	bool GetFloatValue(float& outVal, bool attributeVal = false);

	///<summary>Gets the value as a boolean. Note that the value needs to be "T", "TRUE", or "true" for true
	///or "F", "FALSE", or "false" for false, otherwise it will not be registered successfully</summary>
	///<param name="outVal">The bool to place the result in</param>
	///<param name="attributeVal">True to get current attribute value, false to get current node value</param>
	///<returns>True if successful, false if conversion issues. If false, it is not recommended to use the value</returns>
	bool GetBoolValue(bool& outVal, bool attributeVal = false);

	///<summary>Gets the value as an integer</summary>
	///<param name="outVal">The integer to place the result in</param>
	///<param name="attributeVal">True to get current attribute value, false to get current node value</param>
	///<returns>True if successful, false if conversion issues. If false, it is not recommended to use the value</returns>
	bool GetLongValue(long long& outVal, bool attributeVal = false);
	
	///<summary>Places the child on the stack if valid</summary>
	///<returns>True if there was a child, false if not</returns>
	bool MoveToChildNode();
	
	///<summary>Sets the currently viewed node to its next sibling, if available</summary>
	///<returns>True if sibling existed, false if not</returns>
	bool MoveToNextSiblingNode();

	///<summary>Sets the currently viewed node to its previous sibling, if available</summary>
	///<returns>True if sibling existed, false if not</returns>
	bool MoveToPrevSiblingNode();

	///<summary>Moves to the next attribute, if possible</summary>
	///<returns>True if successful and attribute is a new name/value. False if
	///unsuccessful and attribute was or now is NULL</returns>
	bool MoveToNextAttribute();
};

