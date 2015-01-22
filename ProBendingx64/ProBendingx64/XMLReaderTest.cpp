#include "XMLReaderTest.h"
#include "rapidxml.hpp"

XMLReaderTest::XMLReaderTest(void)
{
}


XMLReaderTest::~XMLReaderTest(void)
{
}

/*///////////////THINGS TO TEST////////////////
///Open File ---------------------------------- PASS - Correct file successfully opened and parsed
///Open Incorrect File ------------------------ PASS - Exception caught and handled when File not found
///Open non-XML File -------------------------- PASS - Exception caught and handled when inappropriate format
///Get Root Node ------------------------------ PASS - Returns the Root Node of the document after file loading
///Get Current Node --------------------------- PASS - Returns the current node being worked on
///Get Current Node Next Sibling -------------- PASS - Moves to the next sibling. Does not move past the end
///Get Current Node Previous Sibling ---------- PASS - Moves to the previous sibling. Does not move past the beginning
///Get Node Name ------------------------------ PASS - Returns the name of the node currently being worked on
///Get Attribute Name ------------------------- PASS - Returns the name of the attribute currently being viewed
///Get Node Has Attributes -------------------- PASS - Returns true if node has attributes, false if not
///Get Current Attribute ---------------------- PASS - If there is an attribute, its returned successfully otherwise NULL
///Get Child Node ----------------------------- PASS - Stack moves to the child successfully
///Reset Tree Position ------------------------ PASS - Top node becomes the node below the root
///Pop Current Node --------------------------- PASS - Pops current node if the current isn't one below the root
///Get Node String Value ---------------------- PASS - Gets the value as a string. Empty string if no value
///Get Node Float Value ----------------------- PASS - Gets value as float, rounding if decimal portion is too long. Returns false if invalid
///Get Node BOOL Value ------------------------ PASS - Gets value as boolean, provided it's correct format
///Get Node Int Value ------------------------- PASS - Gets value as an integer if its a numerical value
///Get Attribute String Value ----------------- PASS - Gets value of attribute as string
///Get Attribute Float Value ------------------ PASS - Gets value of attribute as float
///Get Attribute BOOL Value ------------------- PASS - Gets value of attribute as bool
///Get Attribute Int Value -------------------- PASS - Gets value of attribute as int
*/
void XMLReaderTest::RunTest()
{
	XMLReader reader;

	reader.OpenFile("ProbendingArena.xml"); //Open File

	XMLReader::XMLNode rootNode = reader.GetRootNode(); //Get Root Node Test
	PrintNodeName(rootNode);//Print results of above

	PrintNodeName(reader.GetCurrentNode());//Check to confirm that the current node, after doc creation, is one below the root
	
	if(reader.MoveToChildNode()) //Try to move to the child node
	{
		printf("Child Found: "); //Indicate movement
		printf(reader.GetCurrentNodeName().c_str());printf("\n"); //Print results of above, getting current nodes name
	}
	else
		printf("Child not Found\n");

	if(reader.NodeHasAttributes())//Check if node has attribute
	{
		printf("Node has Attributes: ");
		printf(reader.GetCurrentAttributeName().c_str());printf("\n");

		printf("Get Attribute Name from Attribute Reference: ");
		printf(reader.GetCurrentAttribute()->name()); printf("\n");
	}
	
	if(reader.MoveToChildNode()) //Try to move to the child node
	{
		printf("Child Found: "); //Indicate movement
		printf(reader.GetCurrentNodeName().c_str());printf("\n"); //Print results of above, getting current nodes name
	}
	else
		printf("Child not Found\n");

	if(reader.MoveToChildNode()) //Try to move to the child node
	{
		printf("Child Found: "); //Indicate movement
		printf(reader.GetCurrentNodeName().c_str());printf("\n"); //Print results of above, getting current nodes name
	}
	else
		printf("Child not Found\n");
#pragma region Test Node Values
	printf("Value As String: ");
	printf(reader.GetStringValue().c_str());printf("\n");

	float val = 0.0f;

	if(reader.GetFloatValue(val, false))
		printf("Value As Float: %f\n", val);

	bool boolVal = false;

	if(reader.GetBoolValue(boolVal, false))
		boolVal ? printf("BoolVal is True\n") : printf("BoolVal is False\n");

	long long intVal = 0;

	if(reader.GetLongValue(intVal, false))
		printf("Value as Int: %i\n", intVal);

#pragma endregion 

	if(reader.MoveToNextSiblingNode()) //Move to next sibling from the child, to make sure traversal across child siblings works
	{
		printf("Next Sibling of Child Found: ");
		printf(reader.GetCurrentNodeName().c_str());printf("\n");
	}
	else
		printf("Sibling not found\n");

#pragma region Test Reset and Traversal
	reader.ResetToBeginning(); //Reset to the beginning of the file

	if(reader.NodeHasAttributes()) //Check if node has attributes (in our case it should be FALSE at this point)
		printf("Node After Reset has attributes\n");
	else
		printf("Node After Reset does not have attributes\n"); //This is what we want in this case

	reader.PopNode(); //Pop a node (should not do anything here, because we are at beginning of file)
	printf(reader.GetCurrentNodeName().c_str());printf("\n");//Confirm we haven't moved

	if(reader.MoveToNextSiblingNode()) //Move to next sibling
	{
		printf("Next Sibling Found: ");
		printf(reader.GetCurrentNodeName().c_str());printf("\n");
	}
	else
		printf("Sibling not found\n");

	if(reader.MoveToPrevSiblingNode()) //Move to prev sibling
	{
		printf("Previous Sibling Found: ");
		printf(reader.GetCurrentNodeName().c_str());printf("\n");
	}
	else
		printf("Sibling not found\n");

#pragma endregion

	if(reader.MoveToNextSiblingNode()) //Move to next sibling so we can check attribute values
	{
		printf("Next Sibling Found: ");
		printf(reader.GetCurrentNodeName().c_str());printf("\n");
	}
	else
		printf("Sibling not found\n");

	reader.MoveToChildNode();
	reader.MoveToChildNode();

	printf("CHILD VAL: ");
	printf(reader.GetStringValue().c_str());

#pragma region Attribute Value Testing

	do
	{
		printf("Value As String: ");
		printf(reader.GetStringValue(true).c_str());printf("\n");

		val = 0.0f;

		if(reader.GetFloatValue(val, true))
			printf("Value As Float: %f\n", val);

		boolVal = false;

		if(reader.GetBoolValue(boolVal, true))
			boolVal ? printf("BoolVal is True\n") : printf("BoolVal is False\n");

		intVal = 0;

		if(reader.GetLongValue(intVal, true))
			printf("Value as Int: %i\n", intVal);
	}while(reader.MoveToNextAttribute());
	

#pragma endregion

	printf("\nDone");
}

void XMLReaderTest::PrintNodeName(XMLReader::XMLNode node)
{
	printf(node->name());printf("\n");
}
