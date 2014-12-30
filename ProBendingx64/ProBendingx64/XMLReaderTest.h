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
<No_Stack>15.03</No_Stack>
<No_Stack>SNNA_Val</No_Stack>
<EmptyNameChild>SNNA_Val</EmptyNameChild>
</StringNodeAlloc>
</CharNodeNoAlloc>
<Root_Child StringAttName="50.0" CharAtt="CharVal">
</Root_Child>
</StringNodeNoAlloc>

*/