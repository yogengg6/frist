#include "stdafx.h"

//
// M1Standard
//
// Defines a Biometric Data Interchange Format (BDIF) standard,
// usually read-in via XML file.
//
//

M1Standard::M1Standard() : M1SectionDef(NULL)
{
	// The standard is a section too, and it has itself as the parent standard
	SetStandard(this);

	// The standard is always an M1SectionDef
	SetIsItemDef(false);
}

M1Standard::~M1Standard()
{
}

bool M1Standard::LoadFromFile(const CStdString &sFile, CStdString &sError)
{
	try
	{
		XMLPlatformUtils::Initialize();

	}
	catch (const XMLException& e)
	{
		sError = _T("Error during Xerces-c Initialization.\n");
		sError += e.getMessage();
		goto error;
	}

    //
    //  Create our parser, then attach an error handler to the parser.
    //  The parser will call back to methods of the ErrorHandler if it
    //  discovers errors during the course of parsing the XML document.
    //
    XercesDOMParser *parser = new XercesDOMParser;
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(true);
    parser->setDoSchema(true);
    parser->setHandleMultipleImports(true);
    parser->setValidationSchemaFullChecking(true);
    parser->setCreateEntityReferenceNodes(false);

    DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
    parser->setErrorHandler(errReporter);

    //
    //  Parse the XML file, catching any XML exceptions that might propogate
    //  out of it.
    //
	try
	{
		parser->parse(sFile);
	}
	catch (const OutOfMemoryException&)
	{
		sError = _T("Out of memory exception.");
		goto error;
	}
	catch (const XMLException& e)
	{
		sError = _T("An error occurred during parsing");
		sError += e.getMessage();
		goto error;
	}
	catch (const DOMException& e)
	{
		const unsigned int maxChars = 2047;
		XMLCh errText[maxChars + 1];

		sError.Format(_T("DOM exception %d occurred during parsing"), e.code);
		if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
		{
			CStdString sErrorExtra;
			sErrorExtra.Format(_T(":\n%s"), StrX(errText));
			sError += sErrorExtra;
		}
		goto error;
	}
	catch (...)
	{
		sError = _T("An error occurred during parsing\n");
		goto error;
	}

	if (errReporter->getSawErrors())
	{
		sError = errReporter->getErrorMessage();
		goto error;
	}

    // If the parse was successful, traverse the document data from the DOM tree
	try
	{
		// get the DOM representation
		xercesc::DOMDocument *doc = parser->getDocument();
		xercesc::DOMNode *node = doc->getFirstChild();

		// First node must be <standard>. Just do a sanity check and continue
		if (node == NULL || wcscmp(node->getNodeName(), L"standard") != 0)
		{
			sError = _T("Unexpected XML content found.\n");
			goto error;
		}

		TraverseSectionDef(this, node);
	}
	catch (const OutOfMemoryException&)
	{
		sError = _T("Out of memory exception.\n");
	}
	catch (XMLException& e)
	{
		sError = _T("An error occurred during parsing:\n");
		sError += e.getMessage();
	}

	XMLPlatformUtils::Terminate();

	// Finally perform some simple validation on the XML content.
	// Ensure that all section and item names are original, since we depend on
	// this when using parametrixed functions, such as length() and occurrences().
	//
	// TODO: create linear list of all item and section identifiers.
	// TODO: nxn name comparison
	// TODO: makse sure all reserved items have default values
	// TODO: automaticlaly set all subcontainers of a reserved section to reserved

	return true;

error:
	XMLPlatformUtils::Terminate();
	return false;
}

void M1Standard::TraverseSectionDef(M1SectionDef *pSectionDef, DOMNode *node)
{
	DOMNodeList			*nodeList;
	DOMNode::NodeType	type;
	const XMLCh*		sName;
	const XMLCh*		sContent;

	nodeList = node->getChildNodes();
	if (nodeList == NULL) return;	// shouldn't happen if XML passed validation

	for (unsigned int i = 0; i < nodeList->getLength(); i++)
	{
		node = nodeList->item(i);
		type = node->getNodeType();

		if (type == DOMNode::ELEMENT_NODE)
		{
			sName = node->getNodeName();

			if (wcscmp(sName, L"name") == 0)
			{
				sContent = node->getTextContent();
				pSectionDef->SetName(sContent);
			}
			else if (wcscmp(sName, L"description") == 0)
			{
				sContent = node->getTextContent();
				pSectionDef->SetDescription(sContent);
			}
			else if (wcscmp(sName, L"minoccurences") == 0)
			{
				sContent = node->getTextContent();
				pSectionDef->SetMinOccurrences(sContent);
			}
			else if (wcscmp(sName, L"maxoccurences") == 0)
			{
				sContent = node->getTextContent();
				pSectionDef->SetMaxOccurrences(sContent);
			}
			else if (wcscmp(sName, L"reserved") == 0)
			{
				sContent = node->getTextContent();
				pSectionDef->SetReserved(sContent);
			}
			else if (wcscmp(sName, L"item") == 0)
			{
				// Call separate function to scan item nodes
				M1ItemDef *pItemDefNew = new M1ItemDef(this);
				TraverseItemDef(pItemDefNew, node);
				pSectionDef->AddContainerDef(pItemDefNew, true);
			}
			else if (wcscmp(sName, L"section") == 0)
			{
				// Recurse into this function
				M1SectionDef *pSectionDefNew = new M1SectionDef(this);
				TraverseSectionDef(pSectionDefNew, node);
				pSectionDef->AddContainerDef(pSectionDefNew, false);
			}
		}
	}
}

void M1Standard::TraverseItemDef(M1ItemDef *pItemDef, DOMNode *node)
{
	DOMNodeList			*nodeList;
	DOMNode::NodeType	type;
	const XMLCh*		sName;
	const XMLCh*		sContent;

	nodeList = node->getChildNodes();
	if (nodeList == NULL) return;	// shouldn't happen if XML passed validation

	for (unsigned int i = 0; i < nodeList->getLength(); i++)
	{
		node = nodeList->item(i);
		type = node->getNodeType();

		if (type == DOMNode::ELEMENT_NODE)
		{
			sName = node->getNodeName();
			sContent = node->getTextContent();

			if (wcscmp(sName, L"name") == 0)
			{
				pItemDef->SetName(sContent);
			}
			else if (wcscmp(sName, L"format") == 0)
			{
				pItemDef->SetFormat(sContent);
			}
			else if (wcscmp(sName, L"description") == 0)
			{
				pItemDef->SetDescription(sContent);
			}
			else if (wcscmp(sName, L"minoccurences") == 0)
			{
				pItemDef->SetMinOccurrences(sContent);
			}
			else if (wcscmp(sName, L"maxoccurences") == 0)
			{
				pItemDef->SetMaxOccurrences(sContent);
			}
			else if (wcscmp(sName, L"min") == 0)
			{
				pItemDef->SetMinValue(sContent);
			}
			else if (wcscmp(sName, L"max") == 0)
			{
				pItemDef->SetMaxValue(sContent);
			}
			else if (wcscmp(sName, L"default") == 0)
			{
				pItemDef->SetDefaultValue(sContent);
			}
			else if (wcscmp(sName, L"reserved") == 0)
			{
				pItemDef->SetReserved(sContent);
			}
			else if (wcscmp(sName, L"listitem") == 0)
			{
				CNameValuePair nameValuePair;
				TraverseListitem(nameValuePair, node);
				pItemDef->AddListItem(nameValuePair);
			}
		}
	}
}

void M1Standard::TraverseListitem(CNameValuePair &nameValuePair, DOMNode *node)
// Visit <listitem> entry and extract and return <name> and <value> strings in nameValuePair.
{
	DOMNodeList			*nodeList;
	DOMNode::NodeType	type;
	const XMLCh*		sName;
	const XMLCh*		sContent;

	nodeList = node->getChildNodes();
	if (nodeList == NULL) return;	// shouldn't happen if XML passed validation

	for (unsigned int i = 0; i < nodeList->getLength(); i++)
	{
		node = nodeList->item(i);
		type = node->getNodeType();

		if (type == DOMNode::ELEMENT_NODE)
		{
			sName = node->getNodeName();
			sContent = node->getTextContent();

			if (wcscmp(sName, L"name") == 0)
			{
				nameValuePair.SetName(sContent);
			}
			else if (wcscmp(sName, L"value") == 0)
			{
				nameValuePair.SetValue(sContent);
			}
		}
	}
}


// M1ContainerDef

M1ContainerDef::M1ContainerDef(M1Standard* pStandard)
{
	m_pStandard = pStandard;

	// Min and Max occurrences always default to 1
	SetMinOccurrences(_T("1"));
	SetMaxOccurrences(_T("1"));
	m_bReserved = false;
}

void M1ContainerDef::DebugOutput()
{
}


// M1SectionDef

M1SectionDef::M1SectionDef(M1Standard* pStandard)
			: M1ContainerDef(pStandard)
{
}

M1SectionDef::~M1SectionDef()
{
	list<M1ContainerDef*>::iterator it;
	M1ContainerDef*				    p;

	for (it = m_containerDefs.begin(); it != m_containerDefs.end(); it++)
	{
		p = *it;
		if (p != NULL) delete p;
	}
}

void M1SectionDef::DebugOutput()
{
	M1ContainerDef::DebugOutput();
}

void M1SectionDef::AddContainerDef(M1ContainerDef *pContainerDef, bool bIsItemDef)
{
	pContainerDef->SetIsItemDef(bIsItemDef);
	m_containerDefs.push_back(pContainerDef);
}

void M1SectionDef::ResetContainerDefList()
{
	if (!m_containerDefs.empty())
	{
		m_it = m_containerDefs.begin();
	}
	else
	{
		m_it = m_containerDefs.end();
	}
}

void M1SectionDef::GetNextContainerDef(M1ContainerDef **ppContainer)
{
	if (m_it == m_containerDefs.end())
	{
		*ppContainer = NULL;
	}
	else
	{
		*ppContainer = *m_it;
		m_it++;
	}
}

M1ContainerDef* M1SectionDef::FindContainerDef(const CStdString& sName, bool bIsItemDef)
// Loop through list of containerdefs and return the one with the given name and
// container type Name comparison is case-sensitive. Return NULL if no such container
// found.
{
	M1ContainerDef					*pContainerDef = NULL;
	list<M1ContainerDef*>::iterator	it;

	for (it = GetContainerDefList().begin(); it != GetContainerDefList().end(); it++)
	{
		pContainerDef = *it;

		if ((pContainerDef->GetName().Compare(sName) == 0) &&
			(pContainerDef->GetIsItemDef() == bIsItemDef))
		{
			return pContainerDef;
		}
	}

	return NULL;
}


// M1ItemDef

M1ItemDef::M1ItemDef(M1Standard* pStandard)
		  :M1ContainerDef(pStandard)
{
	// Set defaults for all values
	m_format = IF_undefined;
	m_sMin = _T("");
	m_sMax =  _T("");
	m_sDefaultValue = _T("");
}

M1ItemDef::~M1ItemDef()
{
}

void M1ItemDef::DebugOutput()
{
	M1ContainerDef::DebugOutput();
}

void M1ItemDef::AddListItem(const CNameValuePair &nameValuePair)
{
	m_listItems.push_back(nameValuePair);
}

M1ItemFormat M1ItemDef::StringToFormat(const CStdString &sFormat)
{
	M1ItemFormat format;

	if (sFormat.Compare(_T("bit")) == 0)				 format = IF_bit;
	else if (sFormat.Compare(_T("byte")) == 0)			 format = IF_byte;
	else if (sFormat.Compare(_T("word")) == 0)			 format = IF_word;
	else if (sFormat.Compare(_T("dword")) == 0)			 format = IF_dword;
	else if (sFormat.Compare(_T("tword")) == 0)			 format = IF_tword;
	else if (sFormat.Compare(_T("string")) == 0)		 format = IF_string;
	else if (sFormat.Compare(_T("binary")) == 0)		 format = IF_binary;
	else if (sFormat.Compare(_T("jpegorjpeg2000")) == 0) format = IF_jpegorjpeg2000;
	else format = IF_undefined;	// If we passed schema validation, this shouldn't happen

	return format;
}

CStdString M1ItemDef::FormatToString(M1ItemFormat format)
{
	CStdString sFormat;

	switch (format)
	{
		case IF_bit:			sFormat = _T("bit"); break;
		case IF_byte:			sFormat = _T("byte"); break;
		case IF_word:			sFormat = _T("word"); break;
		case IF_dword:			sFormat = _T("dword"); break;
		case IF_tword:			sFormat = _T("tword"); break;
		case IF_string:			sFormat = _T("string"); break;
		case IF_binary:			sFormat = _T("binary"); break;
		case IF_jpegorjpeg2000:	sFormat = _T("jpegorjpeg2000"); break;
		default:				sFormat = _T("undefined");	// shouldn't happen
	}

	return sFormat;
}

bool M1ItemDef::IsMacro()
// We check if the value is a supported macro
{
	if (GetReserved())
	{
		if (m_sDefaultValue.Find(_T("length(")) == 0 ||
			m_sDefaultValue.Find(_T("occurrences(")) == 0 ||
			m_sDefaultValue.Find(_T("format(")) == 0 ||
			m_sDefaultValue.Find(_T("width(")) == 0 ||
			m_sDefaultValue.Find(_T("height(")) == 0)
		{
			return true;
		}
	}

	return false;
}

// CNameValuePair

CNameValuePair::CNameValuePair()
{
}

CNameValuePair::CNameValuePair(const CStdString &sName, const CStdString &sValue)
{
	m_sName = sName;
	m_sValue = sValue;
}
