#include "stdafx.h"
#include <sstream>

//
// M1ItemValue
//
// Container for variable datatype info
//

M1ItemValue::M1ItemValue()
{
	m_pBin = NULL;
	ResetData();
}

void M1ItemValue::CopyFrom(const M1ItemValue& m)
{
	ResetData();

	if (IsStringType(m.GetFormat()))
	{
		SetString(m.GetString());
	}
	else if (IsNumericType(m.GetFormat()))
	{
		SetNumber(m.GetNumber(), m.GetFormat());
	}
	else if (IsBinaryType(m.GetFormat()))
	{
		BYTE *pBin;
		BYTE *pBinCopy;
		int	 iLen;

		pBin = m.GetBinary(&iLen);

		if (pBin && iLen)
		{
			pBinCopy = new BYTE[iLen];
			memcpy(pBinCopy, pBin, iLen);

			SetBinary(pBinCopy, iLen);
		}
	}
}

M1ItemValue::M1ItemValue(const M1ItemValue& m)
{
	CopyFrom(m);
}

M1ItemValue& M1ItemValue::operator= (const M1ItemValue& other)
{
	CopyFrom(other);
	return *this;
}

M1ItemValue::~M1ItemValue()
{
	ResetData();
}

void M1ItemValue::ResetData()
{
	m_format = IF_undefined;
	m_sValue.empty();
	if (m_pBin != NULL) delete m_pBin;
	m_pBin = NULL;
	m_iLen = 0;
	m_iValue = 0;
}

bool M1ItemValue::IsNumericType(const M1ItemFormat fmt)
{
	if (fmt == IF_bit) return true;
	if (fmt == IF_byte) return true;
	if (fmt == IF_word) return true;
	if (fmt == IF_dword) return true;
	if (fmt == IF_tword) return true;
	return false;
}

bool M1ItemValue::IsStringType(const M1ItemFormat fmt)
{
	if (fmt == IF_string) return true;
	return false;
}

bool M1ItemValue::IsBinaryType(const M1ItemFormat fmt)
{
	if (fmt == IF_binary) return true;
	if (fmt == IF_jpegorjpeg2000) return true;
	if (fmt == IF_image) return true;
	return false;
}

bool M1ItemValue::IsMacro()
// We check if the value is a supported macro
{
	if (m_format == IF_string)
	{
		if (m_sValue.Find(_T("length(")) == 0 ||
			m_sValue.Find(_T("occurrences(")) == 0 ||
			m_sValue.Find(_T("format(")) == 0 ||
			m_sValue.Find(_T("width(")) == 0 ||
			m_sValue.Find(_T("height(")) == 0)
		{
			return true;
		}
	}

	return false;
}

int M1ItemValue::CalculateBitLength()
{
	int iRet = 0;

	switch (m_format)
	{
		case IF_undefined:		iRet = 0; break;
		case IF_string:			iRet = (m_sValue.GetLength() + 1) * 8; break; // (null terminator)
		case IF_tword:			iRet = 48; break;
		case IF_dword:			iRet = 32; break;
		case IF_word:			iRet = 16; break;
		case IF_byte:			iRet = 8; break;
		case IF_bit:			iRet = 1; break;
		case IF_jpegorjpeg2000:
		case IF_image:
		case IF_binary:			iRet = m_iLen * 8; break;
		default:				iRet = 0;
	}

	return iRet;	
}

CStdString M1ItemValue::DebugClassDump()
{
	CStdString s;
	CStdString sLen;

	switch (m_format)
	{
		case IF_undefined:		s = _T("empty"); break;
		case IF_string:			s.Format(_T("string(%s)"), m_sValue); break;
		case IF_tword:			s.Format(_T("tword(%d)"), m_iValue); break;
		case IF_dword:			s.Format(_T("dword(%d)"), m_iValue); break;
		case IF_word:			s.Format(_T("word(%d)"), m_iValue); break;
		case IF_byte:			s.Format(_T("byte(%d)"), m_iValue); break;
		case IF_bit:			s.Format(_T("bit(%d)"), m_iValue); break;
		case IF_jpegorjpeg2000: s.Format(_T("jpeg(%d)"), m_iLen); break;
		case IF_image:			s.Format(_T("image(%d)"), m_iLen); break;
		case IF_binary:			s.Format(_T("binary(%d)"), m_iLen); break;
		default:				s = _T("undefined");
	}

	sLen.Format(_T(" {%d bits}"), CalculateBitLength());
	s += sLen;

	return s;
}

void M1ItemValue::SetString(const CStdString& s)
{
	ResetData();
	SetFormat(IF_string);
	m_sValue = s;
}

void M1ItemValue::SetNumber(int n, M1ItemFormat fmt /*= 0*/)
// Set value to the m_iValue member. If a numerical fmt is provided we also
// set the format to this value, otherwise we use IF_dword since it's a good
// numerical data type to fit any number in.
{
	ResetData();

	m_iValue = n;

	if (fmt == IF_tword || fmt == IF_word || fmt == IF_byte || fmt == IF_bit)
	{
		SetFormat(fmt);
	}
	else
	{
		SetFormat(IF_dword);
	}
}

void M1ItemValue::SetBinary(const BYTE* pBin, int iBufLen)
{
	ResetData();
	SetFormat(IF_binary);

	// We make our own copy of the binary data
	m_iLen = iBufLen;
	m_pBin = new BYTE[m_iLen];
	memcpy(m_pBin, pBin, m_iLen);
}

//
// M1Container
//
// Defines the abstract class that both M1Item and M1Section derive from. The tree structure
// is a list of M1Containers, which represent the items and sections of the data tree.
//

M1Container::M1Container(const CStdString& sName, int index, M1Section* pParent)
{
	SetName(sName);
	SetIndex(index);
	SetParent(pParent);
}


//
// M1Item
//
// Defines the contents of an item from the package
//

M1Item::M1Item(const CStdString& sName, int index, M1ItemFormat fmtFinal, M1Section* pParent)
	   :M1Container(sName, index, pParent)
{
	SetIsItem(true);
	SetFormatFinal(fmtFinal);
}

M1Item::~M1Item()
{
}

CStdString M1Item::DebugClassDump()
{
	CStdString s;
	s.Format(_T("Item (%s)[%d]=(%s)"), GetName(), GetIndex(), m_value.DebugClassDump());
	return s;
}

void M1Item::SetValue(const CStdString& s)
{
	M1ItemValue val;

	val.SetString(s);
	SetValue(val);
}

int M1Item::CalculateBitLength()
{
	if (m_value.IsMacro())
	{
		// macros are strings with the macro, so to calculate the real length
		// we base it on the final format, which must be numerical
		switch (m_formatFinal)
		{
			case IF_tword:			return 48;
			case IF_dword:			return 32;
			case IF_word:			return 16;
			case IF_byte:			return 8;
			case IF_bit:			return 1;
			default:				return 0;
		}
	}
	else
	{
		return m_value.CalculateBitLength();
	}
}


//
// M1Section
//
// Defines the contents of a section of the package
//

M1Section::M1Section(const CStdString& sName, int index, M1Section* pParent)
		  :M1Container(sName, index, pParent)
{
	SetIsItem(false);
	m_pSectionValidated = NULL;
	m_bValidated = false;
}

M1Section::~M1Section()
{
	if (m_pSectionValidated != NULL) delete m_pSectionValidated;
}

CStdString M1Section::DebugSectionDump(M1Section& mainSection, int depth)
// Return a string containing the full tree structure of this section,
// including the name of the section and of all the items and sections
// within it.
{
	M1Container						*pContainer = NULL;
	list<M1Container*>::iterator	it;
	CStdString						sSpacer;
	CStdString						sTotal;

	// Every line will be preceeded by [depth] spacer margins, to clearly
	// see the tree structure. Each 'margin' is made up of 4 periods.
	for (int i = 0; i < 4*depth; i++) sSpacer += _T('.');

	sTotal.Format(_T("%sSection (%s)[%d] {%d bits}\n"), sSpacer, mainSection.GetName(), mainSection.GetIndex(), CalculateBitLength());

	// Traverse the list of containers, determining whether they're items or sections
	for (it = mainSection.GetContainerList().begin(); it != mainSection.GetContainerList().end(); it++)
	{
		pContainer = *it;

		if (pContainer->GetIsItem())
		{
			// Output basic details about the item
			sTotal += sSpacer + _T("....") + pContainer->DebugClassDump() + _T('\n');
		}
		else
		{
			// It's a section, recurse into it
			sTotal += DebugSectionDump((M1Section&)*pContainer, depth+1);
		}
	}

	return sTotal;
}

CStdString M1Section::DebugClassDump()
{
	return DebugSectionDump(*this, 0);
}

M1Container* M1Section::FindContainer(const CStdString& sName, int iItem)
// Loop through list of containers and return the one with the given name and index
// Name comparison is case-sensitive. Return NULL if no such container found.
{
	M1Container						*pContainer = NULL;
	list<M1Container*>::iterator	it;

	for (it = GetContainerList().begin(); it != GetContainerList().end(); it++)
	{
		pContainer = *it;

		if ((pContainer->GetName().Compare(sName) == 0) &&
			(pContainer->GetIndex() == iItem))
		{
			return pContainer;
		}
	}

	return NULL;
}

M1Item* M1Section::FindItem(const CStdString& sItemName, int iItem)
// Find container with same name and index, and before returning it just
// make sure it's an item.
{
	M1Container* pContainer = NULL;

	pContainer = FindContainer(sItemName, iItem);

	if (pContainer != NULL)
	{
		if (pContainer->GetIsItem())
		{
			return (M1Item*)pContainer;
		}
	}

	return NULL;
}

M1Item* M1Section::Item(const CStdString& sItemName, int iItem, bool *pbNewItemAdded/*= NULL*/)
// Return pointer to an item with the same name and item index. If one does not exist,
// it gets created and added to the list of items, then returned.
{
	M1Item *pItem = NULL;

	pItem = FindItem(sItemName, iItem);
	if (pItem == NULL)
	{
		// Add a new item witn undefined final format. This final format will be
		// properly set during validation.
		pItem = new M1Item(sItemName, iItem, IF_undefined, this);
		GetContainerList().push_back(pItem);

		if (pbNewItemAdded != NULL) *pbNewItemAdded = true;
	}
	else
	{
		if (pbNewItemAdded != NULL) *pbNewItemAdded = false;

	}

	return pItem;
}

M1Section* M1Section::FindSection(const CStdString& sSectionName, int iSection)
// Find container with same name and index, and before returning it just
// make sure it's a section.
{
	M1Container* pContainer = NULL;

	pContainer = FindContainer(sSectionName, iSection);

	if (pContainer != NULL)
	{
		if (!pContainer->GetIsItem())
		{
			return (M1Section*)pContainer;
		}
	}

	return NULL;
}

M1Section* M1Section::Section(const CStdString& sSectionName, int iSection, bool *pbNewSectionAdded/*= NULL*/)
// Return pointer to a section with the same name and index. If one does not exist,
// it gets created and added to the list of sections, then returned.
{
	M1Section *pSection = NULL;

	pSection = FindSection(sSectionName, iSection);
	if (pSection == NULL)
	{
		pSection = new M1Section(sSectionName, iSection, this);
		GetContainerList().push_back(pSection);
		// We added a section, "the document is dirty"
		m_bValidated = false;

		if (pbNewSectionAdded != NULL) *pbNewSectionAdded = true;
	}
	else
	{
		if (pbNewSectionAdded != NULL) *pbNewSectionAdded = false;
	}

	return pSection;
}

void M1Section::SetItemString(const CStdString& sItemName, int iItem, const CStdString& sValue)
{
	Item(sItemName, iItem)->GetValue()->SetString(sValue);
	m_bValidated = false;
}

void M1Section::SetItemNumber(const CStdString& sItemName, int iItem, int iValue)
{
	Item(sItemName, iItem)->GetValue()->SetNumber(iValue);
	m_bValidated = false;
}

void M1Section::SetItemBit(const CStdString& sItemName, int iItem, bool bValue)
{
	Item(sItemName, iItem)->GetValue()->SetNumber(bValue ? 1 : 0);
	m_bValidated = false;
}

void M1Section::SetItemImage(const CStdString& sItemName, int iItem, BYTE* pImage, int iImageLen)
{
	Item(sItemName, iItem)->GetValue()->SetBinary(pImage, iImageLen);
	m_bValidated = false;
}

bool M1Section::GetItemString(const CStdString& sItemName, int iItem, CStdString& sValue)
// Return item's string, if its datatype is indeed a string.
// If the item does not exist or is not of string type, returns false.
{
	M1Item		*pItem = NULL;
	M1ItemValue	*pValue;

	pItem = FindItem(sItemName, iItem);

	if (pItem != NULL)
	{
		pValue = pItem->GetValue();

		if (M1ItemValue::IsStringType(pValue->GetFormat()))
		{
			sValue = pValue->GetString();
			return true;
		}
	}

	return false;
}

bool M1Section::GetItemNumber(const CStdString& sItemName, int iItem, int& iValue)
{
	M1Item		*pItem = NULL;
	M1ItemValue	*pValue;

	pItem = FindItem(sItemName, iItem);

	if (pItem != NULL)
	{
		pValue = pItem->GetValue();

		if (M1ItemValue::IsNumericType(pValue->GetFormat()))
		{
			iValue = pValue->GetNumber();
			return true;
		}
	}

	return false;
}

bool M1Section::GetItemImage(const CStdString& sItemName, int iItem, BYTE** ppImage, int* piImageLen)
{
	M1Item		*pItem = NULL;
	M1ItemValue	*pValue;

	pItem = FindItem(sItemName, iItem);

	if (pItem != NULL)
	{
		pValue = pItem->GetValue();

		if (M1ItemValue::IsBinaryType(pValue->GetFormat()))
		{
			int			iLen;
			const BYTE* pBin;

			pBin = pValue->GetBinary(&iLen);

			if (pBin != NULL && iLen != 0)
			{
				// Make a copy for the caller, who will eventually need to call M1Free.
				*piImageLen = iLen;
				*ppImage = new BYTE [*piImageLen];

				memcpy(*ppImage, pBin, *piImageLen);
				return true;
			}
		}
	}

	return false;
}

bool M1Section::Validate(M1Standard* pStandard, CStdString& sLog, CStdString& sError)
// Perform complete validation of this package section based on the standard defined
// in pStandard. On error returns false and sets sError. On success this function
// sets the m_pSectionValidated fully validated package ready for serialization
// and sets the m_bValidated flag which will permit calls to the serialization functions.
// The string sLog is always populated and contains detailed information on the validation
// process.
{
	bool				bRet;

	sLog.Empty();
	sError.Empty();

	m_bValidated = false;
	if (m_pSectionValidated != NULL) delete m_pSectionValidated;
	m_pSectionValidated = new M1Section(GetName(), 1, NULL);

	OutputDebugString(DebugClassDump());

	sLog = _T("Pass 1: Verifying and adjusting general tree structure...\n");
	bRet = AdjustSection(*this, *pStandard, *m_pSectionValidated, sLog, sError);
	if (!bRet) goto done;

	OutputDebugString(_T(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"));
	OutputDebugString(m_pSectionValidated->DebugClassDump());
	OutputDebugString(_T(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"));

	// If the adjustment succeeded, sectionAdjusted contains the new realigned
	// tree structure with all the reserved and missing default containers
	// added. Now we can validate the data within the individual items.
	sLog += _T("Pass 2: Validating individual items...\n");
	bRet = ValidateSection(*m_pSectionValidated, *pStandard, sLog, sError);
	if (!bRet) goto done;

	// If the validation of all the individual items succeeded, we move
	// on to pass 3 where we evaluate all macros.
	sLog += _T("Pass 3: Resolving macros...\n");
	bRet = true;
	ResolveSectionMacros(*m_pSectionValidated, *m_pSectionValidated, bRet, sLog, sError);
	if (!bRet) goto done;

done:
	if (!bRet)
	{
		if (m_pSectionValidated != NULL)
		{
			delete m_pSectionValidated;
			m_pSectionValidated = NULL;
		}

		OutputDebugString(_T("--------------------------------------------------\n"));
		OutputDebugString(sError);
		OutputDebugString(_T("\n--------------------------------------------------\n"));
	}
	else
	{
		// Success!
		m_bValidated = true;

		OutputDebugString(_T("--------------------------------------------------\n"));
		OutputDebugString(sLog);
		OutputDebugString(_T("--------------------------------------------------\n"));
		OutputDebugString(m_pSectionValidated->DebugClassDump());
		OutputDebugString(_T("--------------------------------------------------\n"));
	}

	return bRet;
}

void M1Section::ResolveSectionMacros(M1Section& section, M1Section& headSection, bool& bRet, CStdString& sLog, CStdString& sError)
// We look for string items that look like macros, and evaluate them. Sections recurse back into this function.
// Return value indicating success is set in bRet, which must be initialized to true before calling this function.
{
	list<M1Container*>::iterator	it;
	M1Container*					pContainerNext;
	M1Item*							pItem;

	for (it = section.GetContainerList().begin(); it != section.GetContainerList().end(); it++)
	{
		pContainerNext = *it;

		if (pContainerNext->GetIsItem())
		{
			pItem = (M1Item*)pContainerNext;
			if (pItem->GetValue()->IsMacro())
			{
				bRet &= ResolveItemMacro(*pItem, section, headSection, sLog, sError);
				if (!bRet) return;
			}
		}
		else
		{
			ResolveSectionMacros(*(M1Section*)pContainerNext, headSection, bRet, sLog, sError); // Recurse
		}
	}
}

bool M1Section::ResolveItemMacro(M1Item& item, M1Section& section, M1Section& headSection, CStdString& sLog, CStdString& sError)
// For the given item that is contained in section and resides in the full package defined by headSection,
// we evaluate the macro by parsing it and operating on the identifier found within, based on the macro.
// Once evaluated, the item's value is changed to the resulting value.
{
	CStdString	sValue;
	CStdString	sMacro;
	CStdString	sIdentifier;
	CStdString	s;
	M1Container *pContainer = NULL;
	int			iNum = 0;

	sValue = item.GetValue()->GetString();

	// Add simple log entry
	s.Format(_T("Resolving macro \"%s\" = "), sValue);
	sLog += s;

	if (!ParseMacro(sValue, sMacro, sIdentifier))
	{
		sError.Format(_T("Failed to parse macro \"%s\""), sValue);
		return false;
	}

	// Add simple log ent
	s.Format(_T("\"%s\" of \"%s\"\n"), sMacro, sIdentifier);
	sLog += s;

	if (sMacro == _T("length"))
	{
		// The length macro always operates on containers that precede it
		FindContainerPreceedingItem(item, sIdentifier, headSection, &pContainer);

		if (pContainer == NULL)
		{
			sError.Format(_T("Failed to find container \"%s\" preceeding macro \"%s\""), sIdentifier, sValue);
			return false;
		}

		// Add simple log entry
		s.Format(_T("Found container \"%s\" of byte length %d\n"), pContainer->GetName(), pContainer->CalculateBitLength()/8);
		sLog += s;

		// TODO: set datatype. TODO: ensure multiple of 8.
		item.GetValue()->SetNumber(pContainer->CalculateBitLength()/8, item.GetFormatFinal());
	}
	else if (sMacro == _T("occurrences"))
	{
		// The occurrences macro always operates on containers that are within the tree level one above
		// the section of the current item.
		CountContainerOccurrences(sIdentifier, section.GetParent(), &iNum);

		// Add simple log entry
		s.Format(_T("Found %d occurrences of container \"%s\"\n"), iNum, sIdentifier);
		sLog += s;

		item.GetValue()->SetNumber(iNum, item.GetFormatFinal());
	}
	//
	// TODO: someday support advanced functionality to automatically set the image format
	// and width and height.
	//
	// M1Item		*pItem;
	// const BYTE	*pBin;
	// int			iLen;
	//
	//else if (sMacro == _T("format") || sMacro == _T("width") || sMacro == _T("height"))
	//{
	//	// The image property macro always operates on containers that succeed it
	//	FindContainerSucceedingItem(item, sIdentifier, headSection, &pContainer);

	//	// Make sure we found it
	//	if (pContainer == NULL)
	//	{
	//		sError.Format(_T("Failed to find container \"%s\" succeeding macro \"%s\""), sIdentifier, sValue);
	//		return false;
	//	}

	//	// Make sure it's an item
	//	if (!pContainer->GetIsItem())
	//	{
	//		sError.Format(_T("Container \"%s\" used in macro \"%s\" is a section"), sIdentifier, sValue);
	//		return false;
	//	}

	//	pItem = (M1Item*)pContainer;

	//	// Make sure it's binary
	//	if (!M1ItemValue::IsBinaryType(pItem->GetValue()->GetFormat()))
	//	{
	//		sError.Format(_T("Container \"%s\" used in macro \"%s\" is not binary"), sIdentifier, sValue);
	//		return false;
	//	}

	//	// Add simple log entry
	//	//s.Format(_T("Found container \"%s\" of byte length %d\n"), pContainer->GetName(), pContainer->CalculateBitLength()/8);
	//	//sLog += s;
	//	pBin = pItem->GetValue()->GetBinary(&iLen);
	//	M1Image img(pBin, iLen);

	//	//M1Image.GetFormat
	//	//M1Image.GetWidth
	//	//M1Image.GetHeight

	//	//item.GetValue()->SetNumber(0, item.GetFormatFinal());		
	//}
	else
	{
		sError.Format(_T("Unsupported macro \"%s\""), sMacro);
		return false;
	}

	return true;
}

void M1Section::CountContainerOccurrences(const CStdString &sContainerName, const M1Section &section, int* piNum)
// Count the number of occurrences of all containers named sContainerName in the given section.
{
	list<M1Container*>::iterator	it;
	M1Container						*pContainer;
	M1Section						*pSection;

	pSection = (M1Section*)&section;

	// Continue will all child containers
	for (it = pSection->GetContainerList().begin(); it != pSection->GetContainerList().end(); it++)
	{
		pContainer = *it;

		// Item or section, we increment our container count
		if (pContainer->GetName() == sContainerName)
		{
			(*piNum)++;
		}

		if (!pContainer->GetIsItem())
		{
			// It's a section, recurse into it
			CountContainerOccurrences(sContainerName, *(M1Section*)pContainer, piNum); // Recurse
		}
	}
}

void M1Section::FindContainerPreceedingItem(const M1Item& item, const CStdString &sContainerName, const M1Section& startSection, M1Container** ppCurrentContainer)
// Look for the first container with name equal to the identifier, travelling backwards
// up the tree from item. Since it's easier to travel the tree forwards, we look for the
// last container named sIdentifier just before the item.
{
	list<M1Container*>::iterator	it;
	M1Container						*pContainer;
	M1Section						*pSection;

	// Check the current section first
	pSection = (M1Section*)&startSection;
	if (pSection->GetName() == sContainerName)
	{
		// we have one, save it in 'currentContainer'
		*ppCurrentContainer = pSection;
	}

	// Continue will all child containers
	for (it = pSection->GetContainerList().begin(); it != pSection->GetContainerList().end(); it++)
	{
		pContainer = *it;

		if (pContainer->GetName() == sContainerName)
		{
			// we have one, save it in 'currentContainer'
			*ppCurrentContainer = pContainer;
		}
		else if (pContainer == &item)
		{
			// we finally reached item, so the value of *ppCurrentContainer will be the
			// one we're looking for (or NULL if none was found before)
			return;
		}
		else if (!pContainer->GetIsItem())
		{
			// it's another section, recurse into it
			FindContainerPreceedingItem(item, sContainerName, *(M1Section*)pContainer, ppCurrentContainer); // Recurse
		}
	}
}

void M1Section::FindContainerSucceedingItem(const M1Item& item, const CStdString &sContainerName, const M1Section& startSection, M1Container** ppCurrentContainer)
// Look for the first container with name equal to the identifier right after item.
{
	list<M1Container*>::iterator	it;
	M1Container						*pContainer;
	M1Section						*pSection;
	bool							bAfterItem;

	bAfterItem = false;

	// Check the current section first
	pSection = (M1Section*)&startSection;
	if (pSection->GetName() == sContainerName)
	{
		if (bAfterItem)
		{
			// we have it, save it in 'currentContainer'
			*ppCurrentContainer = pSection;
			return;
		}
	}

	// Continue will all child containers
	for (it = pSection->GetContainerList().begin(); it != pSection->GetContainerList().end(); it++)
	{
		pContainer = *it;

		if (bAfterItem && pContainer->GetName() == sContainerName)
		{
			// we have it, save it in 'currentContainer'
			*ppCurrentContainer = pContainer;
			return;
		}

		if (pContainer == &item)
		{
			// we reached item, so we can start looking for the first occurence of
			// sContainerName
			bAfterItem = true;
		}
		else if (!pContainer->GetIsItem())
		{
			// it's another section, recurse into it
			FindContainerPreceedingItem(item, sContainerName, *(M1Section*)pContainer, ppCurrentContainer); // Recurse
		}
	}
}

bool M1Section::ParseMacro(const CStdString& sIn, CStdString& sMacro, CStdString& sIdentifier)
// Returns true and sets 'f' and 'x' for 'f(x)' using the brackets as parsing boundaries.
// Returns false if these boundaries are missing or not in the correct order.
{
	bool		bRet = false;
	int			iStart;
	int			iEnd;

	iStart = sIn.Find(_T("("));
	iEnd = sIn.Find(_T(")"));

	if (iStart != -1 && iEnd != -1 && iEnd > iStart)
	{
		sMacro = sIn.Left(iStart);
		sIdentifier = sIn.Mid(iStart + 1, iEnd - iStart - 1);

		bRet = true;
	}

	return bRet;
}

int M1Section::CalculateBitLength()
{
	return CalculateBitLength(*this);
}

int M1Section::CalculateBitLength(M1Section& section)
// Traverse all containers recursively and add up total bit length. Item lengths are calculated
// by the items themselves, section lengths are just the sum of the lengths of all their containers.
{
	int iTotal = 0;
	list<M1Container*>::iterator	itCon;
	M1Container*					pContainerNext;

	for (itCon = section.GetContainerList().begin(); itCon != section.GetContainerList().end(); itCon++)
	{
		pContainerNext = *itCon;

		if (pContainerNext->GetIsItem())
		{
			iTotal += pContainerNext->CalculateBitLength();
		}
		else
		{
			iTotal += CalculateBitLength(*(M1Section*)pContainerNext);
		}
	}

	return iTotal;
}

bool M1Section::ValidateSection(M1Section& section, M1SectionDef& sectionDef,
							    CStdString& sLog, CStdString& sError)
{
	list<M1ContainerDef*>::iterator	itDef;
	list<M1Container*>::iterator	itCon;
	CStdString						sName;
	list<M1Container*>				containersNamed;
	M1ContainerDef*					pContainerDefNext;
	M1Container*					pContainerNext;
	M1SectionDef*					pSectionDefNext;
	M1Section*						pSectionNext;
	M1ItemDef*						pItemDefNext;
	M1Item*							pItemNext;

	// Loop through all sectiondefs and itemdefs at this tree level
	for (itDef = sectionDef.GetContainerDefList().begin();
		 itDef != sectionDef.GetContainerDefList().end();
		 itDef++)
	{
		pContainerDefNext = *itDef;

		sName = pContainerDefNext->GetName();

		// Look for all containers in pSection that go by the same name, and form
		// a list of them in containersNamed.
		containersNamed.clear();
		for (itCon = section.GetContainerList().begin(); itCon != section.GetContainerList().end(); itCon++)
		{
			pContainerNext = *itCon;

			if (pContainerNext->GetName().Compare(sName) == 0)
			{
				containersNamed.push_back(pContainerNext);
			}
		}

		// Validate the item(s) or recurse into the section(s)
		for (itCon = containersNamed.begin(); itCon != containersNamed.end(); itCon++)
		{
			pContainerNext = *itCon;

			if (pContainerDefNext->GetIsItemDef())
			{
				// It's an item, validate it
				pItemNext = (M1Item*)pContainerNext;
				pItemDefNext = (M1ItemDef*)pContainerDefNext;
				if (!ValidateItem(*pItemNext, *pItemDefNext, sLog, sError)) return false;
			}
			else
			{
				// It's a section, just recurse into this function
				pSectionNext = (M1Section*)pContainerNext;
				pSectionDefNext = (M1SectionDef*)pContainerDefNext;
				if (!ValidateSection(*pSectionNext, *pSectionDefNext, sLog, sError)) return false;
			}
		}
	}

	return true;
}

bool M1Section::ValidateItem(M1Item& item, M1ItemDef& itemDef,
							 CStdString& sLog, CStdString& sError)
// This is a 3 step process whereby first we consolidate the data and datatype within item with
// the datatype of the standard within itemDef. Coming into this function, the datatypes used
// may not be correct. For example, the caller may have used a string where a dword is needed.
// Or for simplicity a dword may have been used to store a wide variety of numerical formats.
// Upon successful exit, the datatype will be updated to it's final datatype as defined in itemDef.
// Once the datatype has been consolidated, we validate the unit item against min and max. Note
// that for strings and binary types the min and max values refer to the length.
// Finally, if the item has an associated list of values (listitems) we make sure the present value
// is among those.
{
	CStdString						sName;
	M1ItemValue						*pValue;
	M1ItemFormat					fmt;
	M1ItemFormat					fmtDef;
	int								iMin;
	int								iMax;
	int								iLen;
	list<CNameValuePair>::iterator	it;
	CNameValuePair					nameValue;
	CStdString						sValue;
	bool							bLegalValueFound;

	sName = item.GetName();

	// Add simple log entry
	CStdString s;
	s.Format(_T("Validating item \"%s\"\n"), sName);
	sLog += s;

	pValue = item.GetValue();
	fmt = pValue->GetFormat();
	fmtDef = itemDef.GetFormat();

	// Skip macros, we deal with them later
	if (pValue->IsMacro()) return true;

	if (fmtDef == IF_bit)
	{
		// The standard needs a a binary bit value. So any numerical value type will do,
		// as long as the value is either 0 or 1. Likewise we can accept a string, but
		// only the values "true" or "false" or "1" or "0".
		if (fmt == IF_string)
		{
			if (pValue->GetString() == _T("0") || pValue->GetString() == _T("false"))
			{
				pValue->SetNumber(0, IF_bit);
				fmt = pValue->GetFormat();	// refresh
			}
			else if (pValue->GetString() == _T("1") || pValue->GetString() == _T("true"))
			{
				pValue->SetNumber(1, IF_bit);
				fmt = pValue->GetFormat();	// refresh
			}
			else
			{
				sError.Format(_T("Standard item \"%s\": invalid value \"%s\" for bit datatype"), sName, pValue->GetString());
				return false;
			}
		}
		if (M1ItemValue::IsNumericType(fmt))
		{
			if (pValue->GetNumber() != 0 && pValue->GetNumber() != 1)
			{
				sError.Format(_T("Standard item \"%s\": invalid value \"%d\" for bit datatype"), sName, pValue->GetNumber());
				return false;
			}
			else
			{
				// Success, ensure it's set as a bit
				pValue->SetNumber(pValue->GetNumber(), IF_bit);
			}
		}
		else
		{
			sError.Format(_T("Standard item \"%s\": invalid datatype specified"), sName);
			return false;
		}
	}
	else if (M1ItemValue::IsNumericType(fmtDef))
	{
		// The standard needs a a byte, word, dword or tword value. So any positive numerical value type
		// will do, as long as the value fits in the numerical datatype. Likewise we can accept
		// a string, but only if its content is numerical and its value also fits in the datatype.
		//
		// TODO: what if the value specified by a string is larger than a 32-bit value?
		// TODO: do we really need to support twords fully?
		//
		if (fmt == IF_string)
		{
			if (IsNumerical(pValue->GetString()))
			{
				pValue->SetNumber(_ttol(pValue->GetString()));
				fmt = pValue->GetFormat();	// refresh
			}
			else
			{
				sError.Format(_T("Standard item \"%s\": invalid value \"%s\" for numerical datatype"), sName, pValue->GetString());
				return false;
			}
		}

		if (M1ItemValue::IsNumericType(fmt))
		{
			if (pValue->GetNumber() < 0)
			{
				sError.Format(_T("Standard item \"%s\": negative values not allowed"), sName);
				return false;
			}
			// Check that number fits in datatype
			if (fmtDef == IF_byte)
			{
				if (pValue->GetNumber() > 255)
				{
					sError.Format(_T("Standard item \"%s\": value %d too large for byte datatype"), sName, pValue->GetNumber());
					return false;
				}
			}
			else if (fmtDef == IF_word)
			{
				if (pValue->GetNumber() > 65535)
				{
					sError.Format(_T("Standard item \"%s\": value %d too large for word datatype"), sName, pValue->GetNumber());
					return false;
				}
			}
			else if (fmtDef == IF_tword)
			{
				// twords are 48-bit, so there's no point comparing it to a 32-bit limit
			}

			// Number is positive, it fits in the required container, so set it's exact numerical datatype
			pValue->SetNumber(pValue->GetNumber(), fmtDef);
		}
		else
		{
			sError.Format(_T("Standard item \"%s\": invalid datatype specified"), sName);
			return false;
		}
	}
	else if (fmtDef == IF_string)
	{
		// The standard wants a string, we only allow a string as input
		if (fmt != IF_string)
		{
			sError.Format(_T("Standard item \"%s\": invalid datatype specified"), sName);
			return false;
		}
	}
	else if (fmtDef == IF_binary || fmtDef == IF_jpegorjpeg2000 || fmtDef == IF_image)
	{
		// If the standard wants a binary or a jpeg/jpeg2000 or an image format, we accept
		// any of those 3 for now, since we do not yet differentiate between these types.
		// In the future, they may be treated differently.
		if (fmt != IF_binary && fmt != IF_jpegorjpeg2000 && fmt != IF_image)
		{
			sError.Format(_T("Standard item \"%s\": invalid datatype specified"), sName);
			return false;
		}

		// Set to correct format according to fmdDef
		pValue->SetFormat(fmtDef);
	}

	// Now that we have pValue set to the correct format we can check its bounds.
	// Note the min and max may be unspecified in the standard, in which case
	// GetMinValue and GetMaxValue are empty strings and there is nothing to enforce.
	if (!itemDef.GetMinValue().IsEmpty())
	{
		iMin = InterpretStringValue(itemDef.GetMinValue());

		if (fmt == IF_string)
		{
			if (pValue->GetString().GetLength() < iMin)
			{
				sError.Format(_T("Standard item string \"%s\" too short, min length is %d"), sName, iMin);
				return false;
			}
		}
		else if (M1ItemValue::IsBinaryType(fmt))
		{
			pValue->GetBinary(&iLen);
			if (iLen < iMin)
			{
				sError.Format(_T("Standard item \"%s\" too short, min length is %d"), sName, iMin);
				return false;
			}
		}
		else // fmt is one of the 4 numerical formats
		{
			if (pValue->GetNumber() < iMin)
			{
				sError.Format(_T("Standard numerical item \"%s\" too small, min is %d"), sName, iMin);
				return false;
			}			
		}
	} 

	if (!itemDef.GetMaxValue().IsEmpty())
	{
		iMax = InterpretStringValue(itemDef.GetMaxValue());

		if (iMax != -1) // "unbounded"
		{
			if (fmt == IF_string)
			{
				if (pValue->GetString().GetLength() > iMax)
				{
					sError.Format(_T("Standard item string \"%s\" too long, max length is %d"), sName, iMax);
					return false;
				}
			}
			else if (M1ItemValue::IsBinaryType(fmt))
			{
				pValue->GetBinary(&iLen);
				if (iLen > iMax)
				{
					sError.Format(_T("Standard item \"%s\" too long, max length is %d"), sName, iMax);
					return false;
				}
			}
			else // fmt is one of the 4 numerical formats
			{
				if (pValue->GetNumber() > iMax)
				{
					sError.Format(_T("Standard numerical item \"%s\" too large, max is %d"), sName, iMax);
					return false;
				}			
			}
		}
	}

	// Traverse listitems that define the list of possible values the item can have. Note that in most
	// cases the item list is empty since the item does not have an associated list of legal values.
	if (itemDef.GetListItems().size() != 0)
	{
		// A list is provided by the standard definition, so we must ensure pValue is one of
		// the legal values.
		if (M1ItemValue::IsNumericType(pValue->GetFormat()))
		{
			sValue.Format(_T("%d"), pValue->GetNumber());
		}
		else if (M1ItemValue::IsStringType(pValue->GetFormat()))
		{
			sValue = pValue->GetString();
		}
		else
		{
			// This shouldn't happen, because it would mean that the standard provided a list of
			// possible values for a binary datatype.
			sError.Format(_T("Standard item \"%s\" has listitem, error in standard's definition"), sName);
			return false;
		}

		bLegalValueFound = false;

		for (it = itemDef.GetListItems().begin(); it != itemDef.GetListItems().end(); it++)
		{
			nameValue = *it;

			if (nameValue.GetValue() == sValue)
			{
				// Add simple log entry
				s.Format(_T("Verified legal name/value \"%s\"/\"%s\" for item %s\n"), nameValue.GetName(), nameValue.GetValue(), sName);
				sLog += s;
				bLegalValueFound = true;
				break;
			}
		}

		if (!bLegalValueFound)
		{
			sError.Format(_T("Standard item \"%s\" has illegal value \"%s\""), sName, sValue);
			return false;
		}
	}

	return true;
}

bool M1Section::AdjustSection(M1Section& section, M1SectionDef& sectionDef, M1Section& sectionAdjusted,
							  CStdString& sLog, CStdString& sError)
// Since section represents an unordered tree of data, the first step is to traverse the
// tree defined by sectionDef, and for every itemdef and sectiondef we locate the associated
// items and sections in section. We proceed as follows for each container defined in sectionDef:
//
// 1 - If the container is reserved, we add it ourselves explicitly to sectionAdjusted.
// 2 - Otherwise we collect all containers in section that have the same name and place
//     them in a temporary list, containersNamed.
// 3 - If containersNamed is empty the user has not provided this container, and we check
//     to see if there's a default value. If so, we add the container ourselves to 
//     sectionAdjusted, otherwise we flag an error and exit.
// 4 - We check containersNamed against the occurrence limitations defined it the associated
//     containerDef, and flag an error and exit if any condition has been breached.
// 5 - We sort containersNamed, and ascertain that the indices provided are indeed sequential
//     and start at 1. Otherwise we flag an error and exit.
// 6 - If the container is an item, we add it to sectionAdjusted.
// 7 - If the container is a section, we recurse back into this function, using this section
//     and it's associated sectionDef as parameters.
//
// In the end, sectionAdjusted contains the data provided by section, with additional missing and
// reserved containers added, and respecting the tree order of sectionDef.
//
{
	list<M1Container*>				containersNamed;
	M1ContainerDef*					pContainerDefNext;
	M1Container*					pContainerNext;
	M1SectionDef*					pSectionDefNext;
	M1ItemDef*						pItemDef;
	M1Section*						pSectionNext;
	M1Section*						pSectionNew;
	CStdString						sName;
	list<M1ContainerDef*>::iterator	itDef;
	list<M1Container*>::iterator	itCon;
	CStdString						s;

	// Loop through all sectiondefs and itemdefs at this tree level
	for (itDef = sectionDef.GetContainerDefList().begin();
		 itDef != sectionDef.GetContainerDefList().end();
		 itDef++)
	{
		pContainerDefNext = *itDef;

		sName = pContainerDefNext->GetName();

		// If the item or section is reserved, we need to insert it ourselves. If the item
		// is not reserved, the caller must provide it, unless we have a default value.
		if (pContainerDefNext->GetReserved())
		{
			AddReservedContainer(sectionAdjusted, *pContainerDefNext, sLog);
		}
		else
		{
			// Look for all containers in pSection that go by the same name, and form
			// a list of them in containersNamed.
			containersNamed.clear();
			for (itCon = section.GetContainerList().begin(); itCon != section.GetContainerList().end(); itCon++)
			{
				pContainerNext = *itCon;

				if (pContainerNext->GetName().Compare(sName) == 0)
				{
					containersNamed.push_back(pContainerNext);
				}
			}

			// If it's an item and we have 0 occurrences, we first check to see if there's a default.
			// Like that we can add the item and set its value to this default and proceed.
			if (containersNamed.size() == 0 && pContainerDefNext->GetIsItemDef())
			{
				pItemDef = (M1ItemDef*)pContainerDefNext;
				if (pItemDef->GetDefaultValue() != _T(""))
				{
					// Add simple log entry
					s.Format(_T("Adding missing item \"%s\" with default value \"%s\"\n"), sName, pItemDef->GetDefaultValue());
					sLog += s;

					M1Item *pItem = new M1Item(sName, 1, pItemDef->GetFormat(), NULL);
					pItem->SetValue(pItemDef->GetDefaultValue());

					containersNamed.push_back(pItem);
				}
			}

			// Check number of occurrences against minimum and maximum
			if (!CheckOccurrences(*pContainerDefNext, containersNamed.size(), sLog, sError)) return false;

			// Check indices of multiples items, should be 1, 2, 3, etc..
			if (!CheckIndices(containersNamed, sLog, sError)) return false;

			// Add the item(s) or section(s) to pContainersAdjusted. Recurse if it's a section
			for (itCon = containersNamed.begin(); itCon != containersNamed.end(); itCon++)
			{
				pContainerNext = *itCon;

				if (pContainerDefNext->GetIsItemDef())
				{
					// For an item, we just add the item pointer we already have
					pContainerNext->SetParent(&sectionAdjusted);
					sectionAdjusted.GetContainerList().push_back(pContainerNext);
				}
				else
				{
					// For a section, we don't want the whole section with all its sub-containers,
					// but just the empty section that we can continue adding to when we recurse
					// back into this function.
					pSectionNew = new M1Section(sName, pContainerNext->GetIndex(), &sectionAdjusted);
					sectionAdjusted.GetContainerList().push_back(pSectionNew);

					pSectionNext = (M1Section*)pContainerNext;
					pSectionDefNext = (M1SectionDef*)pContainerDefNext;
					if (!AdjustSection(*pSectionNext, *pSectionDefNext, *pSectionNew, sLog, sError)) return false;
				}
			}
		}
	}

	// Finally, we perform the opposite loop: we traverse section to see if there are any
	// undefined containers that have been erroneously added by the caller. We will consider
	// these errors as well.

	// Add simple log entry
	s.Format(_T("Checking for extraneous containers in section \"%s\"\n"), section.GetName());
	sLog += s;

	for (itCon = section.GetContainerList().begin(); itCon != section.GetContainerList().end(); itCon++)
	{
		pContainerNext = *itCon;

		if (sectionDef.FindContainerDef(pContainerNext->GetName(), pContainerNext->GetIsItem()) == NULL)
		{
			sError.Format(_T("Standard section \"%s\": encountered extraneous %s \"%s\""), section.GetName(),
						  pContainerNext->GetTypeString(), pContainerNext->GetName());
			return false;
		}
	}

	return true;
}

void M1Section::AddReservedContainer(M1Section& section, M1ContainerDef& containerDef, CStdString& sLog)
// Recursively add reserved containers to section's container list
{
	list<M1ContainerDef*>::iterator	it;
	M1Item							*pItem;
	M1ItemDef						*pItemDef;
	M1Section						*pSection;
	M1SectionDef					*pSectionDef;
	M1ContainerDef					*pContainerDefNext;

	// If it's an itemdef, we just add an item with the default value
	if (containerDef.GetIsItemDef())
	{
		// Create new item with index 1 (since reserved items and sections cannot have
		// multiple occurrences) and set the item's value to the default.
		pItemDef = (M1ItemDef*)&containerDef;
		pItem = new M1Item(pItemDef->GetName(), 1, pItemDef->GetFormat(), &section);
		pItem->SetValue(pItemDef->GetDefaultValue());

		// Add simple log entry
		CStdString s;
		s.Format(_T("Adding reserved item \"%s\" with default value \"%s\"\n"), pItem->GetName(), pItemDef->GetDefaultValue());
		sLog += s;

		// Add the item to the list of containers
		section.GetContainerList().push_back(pItem);
	}
	else
	{
		// It's a sectiondef, add a section and recurse into this function for all its subcontainers
		pSectionDef = (M1SectionDef*)&containerDef;
		pSection = new M1Section(pSectionDef->GetName(), 1, &section);

		// Add simple log entry
		CStdString s;
		s.Format(_T("Adding reserved section \"%s\"\n"), pSection->GetName());
		sLog += s;

		// Add the section to the list of containers
		section.GetContainerList().push_back(pSection);

		for (it = pSectionDef->GetContainerDefList().begin(); it != pSectionDef->GetContainerDefList().end(); it++)
		{
			pContainerDefNext = *it;
			AddReservedContainer(*pSection, *pContainerDefNext, sLog);
		}
	}
}

template<>
struct std::greater<M1Container*&>
{
public:
     bool operator()(const M1Container* const lhs, const M1Container* const rhs) const
     {
          return lhs->GetIndex() < rhs->GetIndex();
     }
};

bool M1Section::CheckIndices(list<M1Container*>& containerList, CStdString& sLog, CStdString& sError)
// Check indices of multiples items, should be 1, 2, 3, etc..
// This function returns the list sorted.
{
	list<M1Container*>::iterator	it;
	M1Container						*pContainer;
	int								index = 0;

	// First sort list by index
	containerList.sort(std::greater<M1Container*&>());

	// Traverse list and check for proper indexing
	for (it = containerList.begin(); it != containerList.end(); it++)
	{
		index++;
		pContainer = *it;

		// Add simple log entry
		CStdString s;
		s.Format(_T("Checking indices for container \"%s\", index %d of count %d\n"), pContainer->GetName(),
				 pContainer->GetIndex(), containerList.size());
		sLog += s;

		if (pContainer->GetIndex() != index)
		{
			sError.Format(_T("Standard %s \"%s\": encountered index out of order: %d"),
						  pContainer->GetTypeString(), pContainer->GetName(), pContainer->GetIndex());
			return false;
		}
	}

	return true;
}

bool M1Section::CheckOccurrences(M1ContainerDef& containerDef, int iNum, CStdString& sLog, CStdString& sError)
// Check the number of occurences defined by the containerdef against the
// provided number, and report any discrepancies.
{
	int			iMinOcc;
	int			iMaxOcc;
	bool		bRet = false;

	// Add simple log entry
	CStdString s;
	s.Format(_T("Checking occurrences for %d container(s) \"%s\": %s..%s\n"), iNum, containerDef.GetName(),
			 containerDef.GetMinOccurrences(), containerDef.GetMaxOccurrences());
	sLog += s;

	// Note that the min and max occurrences strings are never empty, because much like when
	// defining a schema, these both default to 1 if not specified.
	iMinOcc = InterpretStringValue(containerDef.GetMinOccurrences());
	iMaxOcc = InterpretStringValue(containerDef.GetMaxOccurrences());

	if (iNum == 0 && iMinOcc > 0)
	{
		// Mandatory container not present
		sError.Format(_T("Mandatory %s \"%s\" not present"), containerDef.GetTypeString(), containerDef.GetName());
	}
	else if (iNum < iMinOcc)
	{
		// Not enough presences
		sError.Format(_T("Standard %s \"%s\" is present only %d times, but must be present at least %d times"),
					  containerDef.GetTypeString(), containerDef.GetName(), iNum, iMinOcc);
	}
	else if (iMaxOcc != -1 && iNum > iMaxOcc) // -1 means "unbounded"
	{
		// Too many presences
		sError.Format(_T("Standard %s \"%s\" is present %d times, but must be present at most %d times"),
					  containerDef.GetTypeString(), containerDef.GetName(), iNum, iMaxOcc);
	}
	else
	{
		bRet = true;
	}

	return bRet;
}

int M1Section::InterpretStringValue(const CStdString& s)
// Interpret string as an integer, following certain rules:
// 1) Positive numerical values are automatically converted to integers
// 2) "unbounded" is converted to -1
// 3) return 0 otherwise
{
	int iRet;

	if (IsNumerical(s))
	{
		iRet = _ttoi(s);
	}
	else if (s.Compare(_T("unbounded")) == 0)
	{
		iRet = -1;
	}
	else
		iRet = 0;
	{
	}

	return iRet;
}

bool M1Section::IsNumerical(const CStdString& s)
// Note that in this function numerical means made up of only digits, so
// negative or decimal signs are not allowed.
{
	for (int i = 0; i < s.GetLength(); i++)
	{
		if (!_istdigit(s.at(i)))
		{
			return false;
		}
	}

	return true;
}

//
// Serialization
//

bool M1Section::WriteToMem(BYTE** ppMem, int* piLen)
{
	boost::dynamic_bitset<BYTE>	bitset;
	int iSectionNumBits;
	int iBitsetNumBits;

	if (!m_bValidated) return false;
	if (!ppMem) return false;
	if (!piLen) return false;
	*ppMem = NULL;

	iSectionNumBits = m_pSectionValidated->CalculateBitLength();
	// double-check that the bit length is a multiple of 8
	if (iSectionNumBits % 8 != 0) return false;

	WriteSectionToBitset(*m_pSectionValidated, bitset);

	iBitsetNumBits = bitset.num_blocks() * 8;

	// As a sanity check, we compare the bit length as calculated by the individual
	// items, based on their datatypes and/or contents, and the length of the
	// serialized bitset.
	if (iSectionNumBits == iBitsetNumBits)
	{
		*piLen = bitset.num_blocks();
		*ppMem = new BYTE [*piLen];

		if (BitsetPopBytes(bitset, *piLen, *ppMem)) return true;
	}

	if (*ppMem != NULL)
	{
		delete *ppMem;
		*ppMem = NULL;
	}

	return false;
}

void M1Section::WriteSectionToBitset(M1Section& section, boost::dynamic_bitset<BYTE>& bitset)
// Because some of the standard's fields can be of type 'bit' and the memory and file systems
// can only serialize with byte granularity, we use Boost's dynamic_bitset to serialize all
// of our data at the bit level. This makes for simpler code than to try and maintain partially
// packed bytes as we go along.
// Note that all these standards store numbers in big-endian whereas Windows is little-endian, so we
// perform the right byte-swaps before writing the numbers.
{
	list<M1Container*>::iterator	it;
	M1Container						*pContainer;
	M1Item							*pItem;
	M1ItemValue						*pItemValue;
	CStdString						sItemValue;

	for (it = section.GetContainerList().begin(); it != section.GetContainerList().end(); it++)
	{
		pContainer = *it;

		// We write items and recurse into sections

		if (pContainer->GetIsItem())
		{
			pItem = (M1Item*)pContainer;
			pItemValue = pItem->GetValue();

			if (pItemValue->GetFormat() == IF_string)
			{
				sItemValue = pItemValue->GetString();
				// The standards do not allow anything but regular ASCII, so if this is the
				// UNICODE version of the library we convert to MBCS.
#ifdef _UNICODE
				int  iValueLen = sItemValue.GetLength() + 1;	// include null-terminator
				char *szValue = new char [iValueLen];
				if (WideCharToMultiByte(CP_UTF8, 0, sItemValue, -1, szValue, iValueLen, 0, 0) != iValueLen)
				{
					// TODO:?
				}
				else
				{
					bitset.append(&szValue[0], &szValue[iValueLen]);
					delete szValue;
				}
#else
				bitset.append(pItemValue->GetString().begin(), pItemValue->GetString().end());
#endif				
			}
			else if (M1ItemValue::IsNumericType(pItemValue->GetFormat()))
			{
				if (pItemValue->GetFormat() == IF_bit)
				{
					bitset.push_back(pItemValue->GetNumber() == 1);
				}
				else if (pItemValue->GetFormat() == IF_byte)
				{
					BYTE b = (BYTE)pItemValue->GetNumber();
					bitset.append(b);
				}
				else if (pItemValue->GetFormat() == IF_word)
				{
					WORD w = (WORD)pItemValue->GetNumber();
					bitset.append(HIBYTE(w));
					bitset.append(LOBYTE(w));
				}
				else if (pItemValue->GetFormat() == IF_dword)
				{
					DWORD dw = (DWORD)pItemValue->GetNumber();
					bitset.append(HIBYTE(HIWORD(dw)));
					bitset.append(LOBYTE(HIWORD(dw)));
					bitset.append(HIBYTE(LOWORD(dw)));
					bitset.append(LOBYTE(LOWORD(dw)));
				}
				else if (pItemValue->GetFormat() == IF_tword)
				{
					// We need to write 6 byte. But we only support storing
					// the first 4, so we just add 2 blank bytes.
					DWORD dw = (DWORD)pItemValue->GetNumber();
					bitset.append(0);
					bitset.append(0);
					bitset.append(HIBYTE(HIWORD(dw)));
					bitset.append(LOBYTE(HIWORD(dw)));
					bitset.append(HIBYTE(LOWORD(dw)));
					bitset.append(LOBYTE(LOWORD(dw)));
				}
			}
			else if (M1ItemValue::IsBinaryType(pItemValue->GetFormat()))
			{
				int			iLen;
				const BYTE* pBin;

				pBin = pItemValue->GetBinary(&iLen);

				for (int i = 0; i < iLen; i++)
				{
					bitset.append(*(pBin + i));
				}
			}
		}
		else
		{
			WriteSectionToBitset(*(M1Section*)pContainer, bitset);
		}
	}
}

bool M1Section::WriteToFile(const CStdString& sPath)
// If the package is validated, write it to mem, write the mem to file,
// free the mem.
{
	bool	bRet = false;
	BYTE	*pMem = NULL;
	int		iLen;
	FILE	*f;

	bRet = WriteToMem(&pMem, &iLen);
	if (!bRet) goto done;		

	f = _tfopen(sPath, _T("wb"));

	if (f != NULL)
	{
		fwrite(pMem, 1, iLen, f);
		fclose(f);
		bRet = true;
	}

done:
	if (pMem != NULL) delete pMem;

	return bRet;
}

bool M1Section::BitsetPopBytes(boost::dynamic_bitset<BYTE>& bitset, int iBytes, void *pBytes)
// Helper function to pop the first N bytes from a dynamic_bitset.
// Caller is responsible for allocation.
{
	// Make sure the bitset has enough bytes to give
	if (bitset.num_blocks() < (unsigned)iBytes) return false;

	// Some STL abracadabra to get the bitset data into a stream...
	stringbuf				sb;
	ostream					os(&sb);
	ostream_iterator<char>	osit(os);
	boost::to_block_range(bitset, osit);

	// ...and then from the stream into a regular BYTE buffer
	sb.sgetn((char*)pBytes, iBytes);

	// Now shrink the bitset accordingly
	bitset >>= iBytes * 8;						// shift right
	bitset.resize(bitset.size() - iBytes * 8);	// adjust size

	return true;
}

void M1Section::ReadSectionFromBitset(M1Section& section, M1SectionDef& sectionDef, boost::dynamic_bitset<BYTE>& bitset,
									  M1Expectations& expectations, CStdString& sError)
// As described in the comments for WriteSectionToBitset, we deal with a 'bitset' and not a BYTE array
// for simplicity in dealing with bitfields. This function returns any accumulated errors in sError.
{
	list<M1ContainerDef*>::iterator	itDef;
	list<M1ContainerDef*>::iterator	itDefPeekNext;
	M1ContainerDef*					pContainerDef;
	CStdString						sName;
	M1ItemDef*						pItemDef;
	M1SectionDef*					pSectionDef;
	M1Item*							pItemNew;
	M1Section*						pSectionNew;
	M1ItemFormat					format;
	CStdString						sDefaultVal;
	int								iDefaultVal;
	M1ItemValue						value;
	boost::dynamic_bitset<BYTE>		bitsetVal;
	CStdString						sMacro;
	CStdString						sIdentifier;
	int								iOccurrences;
	int								iLength;

	// If any errors have been flagged don't bother continuing the recursion
	if (!sError.IsEmpty()) return;

	// Loop through all sectiondefs and itemdefs at this tree level
	for (itDef = sectionDef.GetContainerDefList().begin(); itDef != sectionDef.GetContainerDefList().end(); itDef++)
	{
		pContainerDef = *itDef;

		sName = pContainerDef->GetName();

		if (pContainerDef->GetIsItemDef())
		{
			OutputDebugString(_T("*** ItemDef ")); OutputDebugString(sName); OutputDebugString(_T(" ***\n"));

			pItemDef = (M1ItemDef*)pContainerDef; // cast containerdef to an itemdef

			// Create a new item and read from the bitset into it based on the datatype
			// provided by the itemdef.
			format = pItemDef->GetFormat();
			pItemNew = new M1Item(sName, 1, format, &section);

			// Populate value by reading data of the specified format from the bitset. We also pass in the
			// 'min value' which is used by strings to imply string length. Any errors that occurr will
			// set an appripriate string in sError. Also, if this is the last container in the current
			// section, we check to see if there's a length macro that could contain a clue as to how
			// much data is remaning in this last item.
			itDefPeekNext = itDef;
			itDefPeekNext++;
			iLength = 0;
			if (itDefPeekNext == sectionDef.GetContainerDefList().end())
			{
				// Check is there's an expected length for the item's parent section
				iLength = expectations.Find(ET_length, section.GetName());
				if (iLength != -1)
				{
					// Get the remaining bytes by subtracting the bytes of the current section from
					// the total section's expected length.
					iLength -= section.CalculateBitLength()/8;
				}
				else
				{
					iLength = 0;	// No expected length
				}
			}
			if (iLength == 0)
			{
				if (M1ItemValue::IsStringType(format) || M1ItemValue::IsBinaryType(format))
				{
					iLength = _ttol(pItemDef->GetMinValue());
				}
				else
				{
					iLength = 0;	// not needed
				}
			}
			ReadValueFromBitset(value, format, iLength, bitset, sError);

			sDefaultVal = pItemDef->GetDefaultValue();

			// An itendef can have one of three flavors. It can be a simple data placeholder where the
			// caller provides a value, it can be a restricted value which has a mandatory, fixed value,
			// or it can be a macro that requires decoding.

			if ((pItemDef->IsMacro()))
			{
				// If we have a macro, create an 'expectation' object that contains the macro function,
				// the identifier and the value, and add it to the ongoing expectation list.
				ParseMacro(sDefaultVal, sMacro, sIdentifier);

				M1Expectation expectation;
				if (sMacro == _T("occurrences"))
				{
					expectation.SetType(ET_occurrences);
				}
				else if (sMacro == _T("length"))
				{
					expectation.SetType(ET_length);
				}
				expectation.SetParams(sIdentifier, value.GetNumber());

				// Add it to the list
				expectations.Add(expectation);
			}
			else if (pItemDef->GetReserved())
			{
				// If the field is reserved check its value and flag an error if it doesn't match the mandatory value.
				if (M1ItemValue::IsNumericType(format))
				{
					iDefaultVal = _ttol(sDefaultVal);
					if (value.GetNumber() != iDefaultVal)
					{
						sError.Format(_T("Reserved item \"%s\" with mandatory value \"%d\" has incorrect value \"%d\"."), sName, iDefaultVal, value.GetNumber());
					}
				}
				else
				{
					if (value.GetString() != sDefaultVal)
					{
						sError.Format(_T("Reserved item \"%s\" with mandatory value \"%s\" has incorrect value \"%s\"."), sName, sDefaultVal, value.GetString());
					}
				}
			}

			// Add the item to the current section.
			pItemNew->SetValue(value);
			section.GetContainerList().push_back(pItemNew);
		}
		else
		{
			OutputDebugString(_T("*** SectionDef ")); OutputDebugString(sName); OutputDebugString(_T(" ***\n"));

			pSectionDef = (M1SectionDef*)pContainerDef; // cast containerdef to a sectiondef

			// Check to see if there are multiple occurrences of this section by checking the expectations list
			iOccurrences = expectations.Find(ET_occurrences, pSectionDef->GetName());
			// -1 means no element was found, hence the section occurs once by default
			if (iOccurrences == -1)
			{
				iOccurrences = 1;
			}
			else
			{
				// Remove the expecation from the expectation list, since we've dealt with it
				expectations.RemoveLastFound();
			}

			for (int i = 1; i <= iOccurrences; i++)
			{
				// Create a new blank section matching the sectiondef and recurse into it after adding it
				// to the present section.
				pSectionNew = new M1Section(sName, i, &section);
				section.GetContainerList().push_back(pSectionNew);

				ReadSectionFromBitset(*pSectionNew, *pSectionDef, bitset, expectations, sError);
			}
		}
	}
}

void M1Section::ReadValueFromBitset(M1ItemValue& value, M1ItemFormat format, int iValueLen, boost::dynamic_bitset<BYTE>& bitset, CStdString& sError)
// Set value based on bits read-in from bitset. The number of bits and how to set value depend on format
// and possibly iValueLen, in the cases of strings and binary types.
// Note that all these standards store numbers in big-endian whereas Windows is little-endian, so we
// perform the right byte-swaps before using the numbers.
{
	if (format == IF_string)
	{
		// Read in iStrLen bytes of string and set the item value by it
		BYTE* szVal = new BYTE [iValueLen+1];
		BitsetPopBytes(bitset, iValueLen+1, szVal);
		value.SetString((char*)szVal);
		delete szVal;
	}
	else if (M1ItemValue::IsNumericType(format))
	{
		if (format == IF_bit)
		{
			BYTE b;
			b = bitset[0];
			// Shift bits right and shrink by 1 bit to simulate "popping" bit
			bitset >>= 1;
			bitset.resize(bitset.size() - 1);
			value.SetNumber(b, IF_bit);
		}
		else if (format == IF_byte)
		{
			BYTE b;
			BitsetPopBytes(bitset, 1, &b);
			value.SetNumber(b, IF_byte);
		}
		else if (format == IF_word)
		{
			WORD w1, w2;
			BitsetPopBytes(bitset, 2, &w1);
			// Convert endianness
			w2 = LOBYTE(w1) << 8 | HIBYTE(w1);
			value.SetNumber(w2, IF_word);
		}
		else if (format == IF_dword)
		{
			DWORD dw1, dw2;
			BitsetPopBytes(bitset, 4, &dw1);
			// Convert endianness
			dw2 = LOBYTE(LOWORD(dw1)) << 24 | HIBYTE(LOWORD(dw1)) << 16 | LOBYTE(HIWORD(dw1)) << 8 | HIBYTE(HIWORD(dw1));
			value.SetNumber(dw2, IF_dword);
		}
		else if (format == IF_tword)
		{
			// We need to read 6 bytes into a number, but we only support storing
			// 4, so we just skip the 2 extra bytes.
			DWORD dw1, dw2;
			DWORD w;
	
			BitsetPopBytes(bitset, 2, &w);	// ignore these, they should be 0

			BitsetPopBytes(bitset, 4, &dw1);
			// Convert endianness
			dw2 = LOBYTE(LOWORD(dw1)) << 24 | HIBYTE(LOWORD(dw1)) << 16 | LOBYTE(HIWORD(dw1)) << 8 | HIBYTE(HIWORD(dw1));
			value.SetNumber(dw2, IF_tword);
		}
	}
	else if (M1ItemValue::IsBinaryType(format))
	{
		BYTE*	 pBin;

		// For binary types, iValueLen will most likley come in from a length() macro via the calculation
		// expected length of section - length of section up to now, since binary elements are always
		// placed at the end of a section. We check iValueLen for a possible error.

		if (iValueLen == 0)
		{
			sError.Format(_T("Failed to read binary type: expected length is 0."));
		}
		else
		{
			pBin = new BYTE[iValueLen];

			BitsetPopBytes(bitset, iValueLen, pBin);
			value.SetBinary(pBin, iValueLen);
			// (Note: value now owns pBin)
		}
	}
}

bool M1Section::ReadFromMem(BYTE* pMem, int iLen, M1Standard& standard, CStdString& sError)
{
	M1Expectations				expectations;
	boost::dynamic_bitset<BYTE>	bitset;

	// Create a dynamic bitset containing the package's memory
	bitset.append(&pMem[0], &pMem[iLen]);

	// Create an empty section that will eventually hold the data from the memory package block
	M1Section section(standard.GetName(), 1, NULL);

	// Now populate the section from the memory block
	ReadSectionFromBitset(section, standard, bitset, expectations, sError);

	OutputDebugString(expectations.DebugClassDump());
	OutputDebugString(DebugSectionDump(section, 0));

	// If any error occured during the recursions within ReadSectionFromBitset then sError will
	// have been set. We use this fact to set the appropriate return value.
	if (sError.IsEmpty())
	{
		SetContainerList(section.GetContainerList());
		return true;
	}
	else
	{
		return false;
	}
}

bool M1Section::ReadFromFile(const CStdString& sPath, M1Standard& standard, CStdString& sError)
// Read the file into mem, load from mem, free the mem.
{
	bool	bRet = false;
	BYTE	*pMem = NULL;
	int		iLen;
	FILE	*f;

	sError.empty();

	f = _tfopen(sPath, _T("rb"));

	if (f != NULL)
	{
		fseek(f, 0, SEEK_END);
		iLen = ftell(f);
		fseek(f, 0, SEEK_SET);

		pMem = new BYTE [iLen];

		fread(pMem, 1, iLen, f);
		fclose(f);
		bRet = true;
	}

	bRet = ReadFromMem(pMem, iLen, standard, sError);
	if (!bRet) goto done;

done:
	if (pMem != NULL) delete pMem;

	return bRet;
}

//
// M1Expectation class to define expected structures within a package when reading, such as, for example,
// "length(Facial Record Data) = 60891" or "occurrences(Feature Points) = 2".
//

M1Expectation::M1Expectation()
{
	m_type = ET_undefined;
}

void M1Expectation::SetParams(const CStdString& sIdentifier, int iValue)
{
	m_sIdentifier = sIdentifier;
	m_iValue = iValue;
}

void M1Expectations::Add(M1Expectation& ex)
{
	m_expectations.push_back(ex);
}

// M1Expectations

M1Expectations::M1Expectations()
{
	m_it = m_expectations.end();
}

int M1Expectations::Find(M1ExpectationType type, const CStdString& sIdentifier)
// Returns value for the given expectation type and identifier. If there
// isn't one, return -1.
{
	int			  iRet = -1;
	bool		  bFound;
	M1Expectation ex;

	bFound = false;

	for (m_it = m_expectations.begin(); m_it != m_expectations.end(); m_it++)
	{
		ex = *m_it;

		if (ex.GetType() == type && ex.GetIdentifier() == sIdentifier)
		{
			iRet = ex.GetValue();
			bFound = true;
			break;
		}
	}

	// If not found, reset m_it to "undefined" by setting it to the list end().
	// If it was found, then m_it points to the current item, so any successive
	// calls to RemoveLastFound() will be able to identify this item immediately.
	if (!bFound)
	{
		m_it = m_expectations.end();
	}

	return iRet;
}

void M1Expectations::RemoveLastFound()
{
	if (m_it == m_expectations.end()) return;	// shouldn't happen
	m_expectations.erase(m_it);
}

CStdString M1Expectations::DebugClassDump()
{
	M1Expectation					ex;
	list<M1Expectation>::iterator	it;
	CStdString						sRet;
	CStdString						sLine;

	for (it = m_expectations.begin(); it != m_expectations.end(); it++)
	{
		ex = *it;

		sLine.Format(_T("Expectation type %d, Identifier \"%s\", Value %d\n"), ex.GetType(), ex.GetIdentifier(), ex.GetValue());
		sRet += sLine;
	}

	return sRet;
}
