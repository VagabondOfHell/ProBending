#include "XMLWriterTest.h"
#include "XMLWriter.h"

XMLWriterTest::XMLWriterTest(void)
{
}


XMLWriterTest::~XMLWriterTest(void)
{
}

/*///////////////THINGS TO TEST////////////////
///Node Creation with strings ------------------------------------ PASS - node successfully created
///Node Creation with strings with Rapid XML allocation ---------- PASS - node successfully created
///Node Creation with char* with Rapid XML allocation ------------ PASS - node successfully created
///Node Creation with NULL or Empty ------------------------------ PASS - NULL isn't added. Empty is. Client should check for empty
///Node Creation without adding to stack ------------------------- PASS - node is added to the file but not the stack
///Node Creation specifying Node To Add To ----------------------- PASS - node is added to the specified node
///Node Creation with string thats changed ----------------------- PASS - if allocated to Rapid XML, works perfectly. If not, will write whatever is left in the string from previous text
///Get Top Node -------------------------------------------------- PASS - gets the node at top of stack
///Get Top Node when empty --------------------------------------- PASS - returns NULL
///Get Root Node ------------------------------------------------- PASS - gets the first created node
///Get Root Node when empty -------------------------------------- PASS - returns NULL
///Attribute appending with Strings ------------------------------ PASS - attribute successfully created
///Attribute appending with char* -------------------------------- PASS - attribute successfully created
///Attribute appending before Root Node creation ----------------- PASS - NULL error
///Attribute appending with string to specified node ------------- PASS - attribute successfully created on target node
///Attribute appending with char* to specified node -------------- PASS - attribute successfully created on target node
///Pop Node ------------------------------------------------------ PASS - pops node
///Too many Pop Nodes -------------------------------------------- PASS - won't pop past root
///Write File ---------------------------------------------------- PASS - File written
///
*/
void XMLWriterTest::RunTest()
{
	XMLWriter writer;

	writer.GetRootNode();
	std::string nodeName = "StringNodeNoAlloc";
	std::string nodeValue = "SNNA_Val";

	std::string node2Name = "StringNodeAlloc";
	std::string node2Val = "SNA_VAL";

	char* charNodeName = "CharNodeNoAlloc";
	char* charValName = "CNNA_VAL";

	char* charNode2Name = "";
	char* charNode2Val = NULL;

	std::string emptyName = "";
	std::string emptyVal = "";

	std::string noStackName = "No_Stack";
	char* noStackNameChar = "No_Stack_Char";

	writer.CreateNode(nodeName, nodeValue, true, NULL, false, false);//Node String with RapidXML Alloc
	writer.CreateNode(charNodeName, charValName, true, NULL);//Node Char with Rapid XML Alloc
	writer.CreateNode(node2Name, node2Val, true, NULL, false, false);//Node String without Alloc

	writer.CreateNode(noStackName, nodeValue, false, NULL); //Node create not added to stack
	writer.CreateNode(noStackNameChar, nodeValue.c_str(), false);//Node create char not added to stack

	XMLWriter::XMLNode topNode = writer.GetTop();//Get top node test
	
	std::string childNode = "EmptyNameChild";
	writer.CreateNode(childNode, nodeValue, false, topNode);//Add child node to top node

	XMLWriter::XMLNode rootNode = writer.GetRootNode();//Get root node test
	writer.CreateNode("Root_Child", "RootChildVal", true, rootNode);//Add new child to root node and add to stack

	std::string attName = "StringAttName";
	std::string attVal = "StringAttVal";

	writer.AddAttribute(attName, attVal);//Add string attribute to most recent node
	writer.AddAttribute("CharAtt", "CharVal");//Add char attribute to most recent node

	std::string attNameSpecNode = "AddToNodeString";
	std::string attValSpecNode = "AddToNodeStringVal";

	writer.AddAttribute(attNameSpecNode, attValSpecNode, false, false, topNode);//Add string attribute to previously grabbed top node
	writer.AddAttribute("AddToNodeAtt", "AddToNodeAttVal", topNode);//Add char attribute to previously grabbed top node
	
	writer.CreateNode(std::string("NodeValuesTesting").c_str(), (long long)75);
	writer.AddAttribute(std::string("AttributeValueTesting"), (long long)25);

	writer.CreateNode("NodeBoolTest", true);
	writer.AddAttribute("AttBoolTest", false);

	//writer.CreateNode("NodeFloatTest", 34.782f);
	writer.AddAttribute("AttFloatTest", 17.35f);

	writer.PopNode();//Pop the two blanks off
	writer.PopNode();//Pop the two blanks off

	writer.PopNode();//pop the root child off
	writer.AddAttribute("AttAfterPop", "VALUE");//Add an attribute after popping the previous two


	writer.WriteFile("ProbendingArena.xml");//Write to file
}
