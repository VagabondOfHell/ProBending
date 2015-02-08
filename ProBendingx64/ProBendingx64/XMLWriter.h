#pragma once
#include "FileWriter.h"
#include <stack>

namespace rapidxml
{
	// Forward declarations
	template<class Ch> class xml_node;
	template<class Ch> class xml_attribute;
	template<class Ch> class xml_document;
};

class XMLWriter :
	public FileWriter
{
public:
	typedef rapidxml::xml_node<char>* XMLNode;
	typedef rapidxml::xml_attribute<char>* XMLAttribute;
	typedef rapidxml::xml_document<char> XMLDocument;

protected:
	XMLDocument* xmlDocument;
	XMLNode rootNode;

	std::stack<XMLNode> nodeStack;

	///<summary>Creates a new XML Node in Rapid XML</summary>
	///<param name="nodeName">The name the node should have</param>
	///<param name="nodeValue">The value the node should have</param>
	///<returns>The created XML Node, or NULL if an error occured</returns>
	XMLNode CreateXMLNode(const char* nodeName, const char* nodeValue);

	///<summary>Creates an XML attribute in Rapid XML</summary>
	///<param name="attName">The name of the attribute</param>
	///<param name="attValue">The value of the attribute</param>
	///<returns>The created XML Attribute or NULL if an error occured</returns>
	XMLAttribute CreateXMLAttribute(const char* attName, const char* attValue);

	///<summary>Allocates a string in Rapid XML</summary>
	///<param name="stringToAlloc">The original string to allocate space for</param>
	///<returns>Pointer to the newly created string</returns>
	const char* AllocateString(const char* stringToAlloc);

	///<summary>Appends the node to the parent node or the top node and adds to the stack</summary>
	///<param name="nodeToAppend">The node to append</param>
	///<param name="parentNode">The node to attach the new node to. If NULL, attaches to top of stack</param>
	///<param name="push">True to place at top of the stack, false to skip</push>
	void AppendNode(XMLNode nodeToAppend, XMLNode parentNode = NULL, bool push = true);

	///<summary>Appends an attribute to the node</summary>
	///<param name="attToAppend">The attribute to append</param>
	///<param name="nodeToAppendTo">The node to append to</param>
	///<returns>True if successful, false if errors</returns>
	bool AppendAttribute(XMLAttribute attToAppend, XMLNode nodeToAppendTo);

public:
	XMLWriter(void);
	virtual ~XMLWriter(void);
	
	///<summary>Creates a new XML Node and sets it as the node to be modified</summary>
	///<param name="nodeName">The name to assign to the node. Does not check for empty string</param>
	///<param name="nodeValue">The value to be assigned to the node</param>
	///<param name="push">True to place at top of the stack for modifications (such as adding attributes or children)</param>
	///<param name="parentNode">The node that this node should be attached to, if any</param>
	///<param name="allocateName">True to have the writer allocate the name string (good for function-scoped variables)</param>
	///<param name="allocateValue">True to have the writer allocate the value string (good for function-scoped variables)</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool CreateNode(const std::string& nodeName, const std::string& nodeValue = std::string(""), 
		bool push = true, XMLNode parentNode = NULL, bool allocateName = false, bool allocateValue = false);

	///<summary>Creates a new XML Node and sets it as the node to be modified</summary>
	///<param name="nodeName">The name to assign to the node. Does not check for empty string</param>
	///<param name="nodeValue">The value to be assigned to the node</param>
	///<param name="push">True to place at top of the stack for modifications (such as adding attributes or children)</param>
	///<param name="parentNode">The node that this node should be attached to, if any</param>
	///<param name="allocateName">True to have the writer allocate the name string (good for function-scoped variables)</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	bool CreateNode(const std::string& nodeName, const bool nodeValue,
		bool push = true, XMLNode parentNode = NULL, bool allocateName = false);

	///<summary>Creates a new XML Node and sets it as the node to be modified. This automatically allocates
	///the string in the writer</summary>
	///<param name="nodeName">The name to assign to the node. Does not check for empty string</param>
	///<param name="nodeValue">The value to assign to the node</param>
	///<param name="push">True to place at top of the stack for modifications (such as adding attributes or children)</param>
	///<param name="parentNode">The parent node to add this node to, or NULL to add to root</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool CreateNode(const char* nodeName, const char* nodeValue, bool push = true, XMLNode parentNode = NULL);

	///<summary>Creates a new XML Node and sets it as the node to be modified. This automatically allocates
	///the string in the writer</summary>
	///<param name="nodeName">The name to assign to the node. Does not check for empty string</param>
	///<param name="nodeValue">The value to assign to the node</param>
	///<param name="push">True to place at top of the stack for modifications (such as adding attributes or children)</param>
	///<param name="parentNode">The parent node to add this node to, or NULL to add to root</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool CreateNode(const char* nodeName, const bool nodeValue, bool push = true, XMLNode parentNode = NULL);

	///<summary>Creates a new XML Node and sets it as the node to be modified. This automatically allocates
	///the string in the writer</summary>
	///<param name="nodeName">The name to assign to the node. Does not check for empty string</param>
	///<param name="nodeValue">The value to assign to the node</param>
	///<param name="push">True to place at top of the stack for modifications (such as adding attributes or children)</param>
	///<param name="parentNode">The parent node to add this node to, or NULL to add to root</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool CreateNode(const char* nodeName, const float nodeValue, bool push = true, XMLNode parentNode = NULL);

	///<summary>Creates a new XML Node and sets it as the node to be modified. This automatically allocates
	///the string in the writer</summary>
	///<param name="nodeName">The name to assign to the node. Does not check for empty string</param>
	///<param name="nodeValue">The value to assign to the node</param>
	///<param name="push">True to place at top of the stack for modifications (such as adding attributes or children)</param>
	///<param name="parentNode">The parent node to add this node to, or NULL to add to root</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool CreateNode(const char* nodeName, const __int64 nodeValue, bool push = true, XMLNode parentNode = NULL);

	///<summary>Gets the currently modified Node</summary>
	///<returns>Pointer to the currently modified node, or NULL if none</returns>
	XMLNode GetTop(){return nodeStack.top();}

	///<summary>Adds an attribute to the node at the top of the stack or to the specified node. This method should
	///not be called if nodeToAddTo is NULL and there are no nodes in the stack</summary>
	///<param name="attName">The name of the attribute</param>
	///<param name="attValue">The value of the attribute</param>
	///<param name="allocateName">True to have the writer allocate the name string (good for function-scoped variables)</param>
	///<param name="allocateValue">True to have the writer allocate the value string (good for function-scoped variables)</param>
	///<param name="nodeToAddTo">The node to add the attribute to, or NULL to add to the Top Node</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool AddAttribute(const std::string& attName, const std::string& attValue, 
		bool allocateName = false, bool allocateValue = false, XMLNode nodeToAddTo = NULL);

	///<summary>Adds an attribute to the node at the top of the stack or to the specified node. This method should
	///not be called if nodeToAddTo is NULL and there are no nodes in the stack</summary>
	///<param name="attName">The name of the attribute</param>
	///<param name="attValue">The value of the attribute</param>
	///<param name="allocateName">True to have the writer allocate the name string (good for function-scoped variables)</param>
	///<param name="nodeToAddTo">The node to add the attribute to, or NULL to add to the Top Node</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool AddAttribute(const std::string& attName, const __int64 attVal, 
		bool allocateName = false, XMLNode nodeToAddTo = NULL);

	///<summary>Adds an attribute to the node at the top of the stack or to the specified node. This method should
	///not be called if nodeToAddTo is NULL and there are no nodes in the stack</summary>
	///<param name="attName">The name of the attribute</param>
	///<param name="attValue">The value of the attribute</param>
	///<param name="allocateName">True to have the writer allocate the name string (good for function-scoped variables)</param>
	///<param name="nodeToAddTo">The node to add the attribute to, or NULL to add to the Top Node</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool AddAttribute(const std::string& attName, const float attVal,
		bool allocateName = false, XMLNode nodeToAddTo = NULL);

	///<summary>Adds an attribute to the node at the top of the stack or to the specified node. This method should
	///not be called if nodeToAddTo is NULL and there are no nodes in the stack</summary>
	///<param name="attName">The name of the attribute</param>
	///<param name="attValue">The value of the attribute</param>
	///<param name="allocateName">True to have the writer allocate the name string (good for function-scoped variables)</param>
	///<param name="nodeToAddTo">The node to add the attribute to, or NULL to add to the Top Node</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	bool AddAttribute(const std::string& attName, const bool attVal,
		bool allocateName = false, XMLNode nodeToAddTo = NULL);

	///<summary>Adds an attribute to the top node, or the specified node, after allocating the strings with the writer</summary>
	///<param name="attName">The attribute name</param>
	///<param name="attVal">The attribute value</param>
	///<param name="nodeToAddTo">The node to add the attribute to. NULL to add to the top node</param>
	///<returns>True if successful, false if error. If error, check last error</returns>
	virtual bool AddAttribute(const char* attName, const char* attVal, XMLNode nodeToAddTo = NULL);

	///<summary>Creates a new XML Node and sets it as the node to be modified. This automatically allocates
	///the string in the writer</summary>
	///<param name="nodeName">The name to assign to the node. Does not check for empty string</param>
	///<param name="nodeValue">The value to assign to the node</param>
	///<param name="push">True to place at top of the stack for modifications (such as adding attributes or children)</param>
	///<param name="parentNode">The parent node to add this node to, or NULL to add to root</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool AddAttribute(const char* attName, bool attVal, XMLNode nodeToAddTo = NULL);

	///<summary>Creates a new XML Node and sets it as the node to be modified. This automatically allocates
	///the string in the writer</summary>
	///<param name="nodeName">The name to assign to the node. Does not check for empty string</param>
	///<param name="nodeValue">The value to assign to the node</param>
	///<param name="push">True to place at top of the stack for modifications (such as adding attributes or children)</param>
	///<param name="parentNode">The parent node to add this node to, or NULL to add to root</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool AddAttribute(const char* attName, float attVal, XMLNode nodeToAddTo = NULL);

	///<summary>Creates a new XML Node and sets it as the node to be modified. This automatically allocates
	///the string in the writer</summary>
	///<param name="nodeName">The name to assign to the node. Does not check for empty string</param>
	///<param name="nodeValue">The value to assign to the node</param>
	///<param name="push">True to place at top of the stack for modifications (such as adding attributes or children)</param>
	///<param name="parentNode">The parent node to add this node to, or NULL to add to root</param>
	///<returns>True if successful, false if an error occured. Check last error if false</returns>
	virtual bool AddAttribute(const char* attName, __int64 attVal, XMLNode nodeToAddTo = NULL);

	///<summary>Remove a node off the stack. Does not allow the removal of the root node after it's been created</summary>
	virtual inline void PopNode()
	{
		if(nodeStack.size() > 1)
			nodeStack.pop();
	}

	///<summary>Gets the root XML node of the document</summary>
	///<returns>XML Node of the document</returns>
	inline XMLNode GetRootNode(){return rootNode;}

	///<summary>Writes the contents of the writer to the file</summary>
	///<param name="fileName">The name of the file, with extension</param>
	///<returns>True if successful, false for errors</returns>
	virtual bool WriteFile(std::string fileName);

};

