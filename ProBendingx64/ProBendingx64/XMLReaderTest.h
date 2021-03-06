#pragma once
#include "XMLReader.h"

class XMLReaderTest
{
public:
	XMLReaderTest(void);
	~XMLReaderTest(void);

	void PrintNodeName(XMLReader::XMLNode node);

	void RunTest();

};

/*--------------XML FILE USED FOR TESTING------------------
<StringNodeNoAlloc>
<CharNodeNoAlloc>
<StringNodeAlloc AddToNodeString="AddToNodeStringVal" AddToNodeAtt="AddToNodeAttVal" AttAfterPop="VALUE">
<No_Stack>SNNA_Val</No_Stack>
<No_Stack_Char>SNNA_Val</No_Stack_Char>
<EmptyNameChild>SNNA_Val</EmptyNameChild>
</StringNodeAlloc>
</CharNodeNoAlloc>
<Root_Child StringAttName="StringAttVal" CharAtt="CharVal">
<NodeValuesTesting AttributeValueTesting="25">
<NodeBoolTest AttBoolTest="F" AttFloatTest="17.350000">T</NodeBoolTest>
</NodeValuesTesting>
</Root_Child>
</StringNodeNoAlloc>



*/