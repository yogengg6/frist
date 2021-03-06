#include <math.h>
#include "Includes.h"
#include "NISTField.h"
#include "NISTRecord.h"
#include "OpenEBTS.h"


static char g_szGS[2] = { (char)0x1D, '\0' };
static char g_szFS[2] = { (char)0x1C, '\0' };
static char g_szUS[2] = { (char)0x1F, '\0' };
static char g_szRS[2] = { (char)0x1E, '\0' };


/************************************************************/
/*                                                          */
/*               CSubFieldItem Implementation               */
/*                                                          */
/************************************************************/

void CSubFieldItem::CopyObj(const CSubFieldItem& obj)
{
	this->m_nSubField = obj.m_nSubField;
	this->m_nSubFieldItem = obj.m_nSubFieldItem;
	this->m_sData = obj.m_sData;
}

int CSubFieldItem::SetItemData(int nSubField, int nSubFieldItem, CStdString sData)
{
	int nRet = IW_ERR_OUT_OF_MEMORY;

	IWS_BEGIN_EXCEPTION_METHOD("CSubFieldItem::SetItemData")
	IWS_BEGIN_CATCHEXCEPTION_BLOCK()

	m_nSubField = nSubField;
	m_nSubFieldItem = nSubFieldItem;
	m_sData = sData;

	nRet = IW_SUCCESS;

	IWS_END_CATCHEXCEPTION_BLOCK()

	return nRet;
}

int CSubFieldItem::GetLength()
// Get *byte* length of field, used to populate LEN record fields
{
#ifdef UNICODE
	// Since the field will be stored in UTF-8, we have to temporarily convert it
	// to get to know its size. TODO: this can be made more efficient by limiting
	// the number of conversions performed. As it stands we do it once to get the
	// total byte count to allow setting the LEN field, and again when it comes to
	// to write out the data.
	int nLen = 0;
	char *szOut;
	if (UCStoUTF8(m_sData, &szOut, &nLen))
	{
		delete szOut;
		// Returned length is allocated byte-length, so we don't include the null-terminator
		return nLen - 1;
	}
	else
	{
		// In the rare possibility of a conversion error, we don;t do anything
		// here but return 0. The error will eventually get caught when writing
		// the file contents.
		return 0;
	}
#else
	// Just the number of characters
	return m_sData.GetLength();
#endif
}

/************************************************************/
/*                                                          */
/*               CNISTField Implementation                  */
/*                                                          */
/************************************************************/

CNISTField::CNISTField()
{ 
	InitVars();
}

CNISTField::CNISTField(long lRecordType, long lFieldID)
{
	InitVars();

	m_nField = lFieldID;
	m_nRecordType = lRecordType;
}

void CNISTField::CopyObj(const CNISTField& obj)
{
	this->m_ImageFormat = obj.m_ImageFormat;
	this->m_nField = obj.m_nField;
	this->m_nImageLen = obj.m_nImageLen;
	this->m_nRecordType = obj.m_nRecordType;
	this->m_SubFieldAry = obj.m_SubFieldAry;

	if (obj.m_pImageData == NULL)
	{
		this->m_pImageData = NULL;
	}
	else
	{
		this->m_pImageData = new unsigned char[obj.m_nImageLen];
		memcpy(this->m_pImageData, obj.m_pImageData, obj.m_nImageLen);
	}
}

void CNISTField::InitVars()
{
	m_nField = 0;
	m_pImageData = 0;
	m_nImageLen = 0;
	m_ImageFormat = fmtUNK;
	m_nRecordType = 0;
}

CNISTField::~CNISTField() 
{ 
	size_t nCount = m_SubFieldAry.size();
	CSubFieldItem *pTemp;

	for (unsigned int i = 0; i < nCount; i++)
	{
		pTemp = m_SubFieldAry.at(i);
		if (pTemp) delete pTemp;
	}
	m_SubFieldAry.clear();

	if (m_pImageData)
	{
		m_nImageLen = 0;
		delete m_pImageData;
		m_pImageData = 0;
	}
}

int CNISTField::SetSubField(int nSubField, int nSubFieldItem, CStdString sData)
{
	size_t nCount = m_SubFieldAry.size();
	int nRet = IW_SUCCESS;
	bool bFound = false;
	CSubFieldItem* pItem;

	for (unsigned int i = 0; i < nCount && !bFound; i++)
	{
		pItem = m_SubFieldAry.at(i);
		
		if (pItem->m_nSubField == nSubField &&  pItem->m_nSubFieldItem == nSubFieldItem)
		{
			nRet = pItem->SetItemData(nSubField, nSubFieldItem, sData);
			bFound = true;
		}
	}

	if (!bFound)
	{
		pItem = new CSubFieldItem;
		nRet = pItem->SetItemData(nSubField, nSubFieldItem, sData);
		m_SubFieldAry.push_back(pItem);
	}

	return nRet;
}

int CNISTField::RemoveSubField(int nSubField)
{
	size_t nCount = m_SubFieldAry.size();
	int nRet = IW_SUCCESS;
	bool bFound = false;
	CSubFieldItem *pItem;
	std::vector<WORD> wSubFieldIndex;
	WORD wLastIndexDeleted;
	WORD index;

	// Form a list of all Item indexes in the SubField array to remove
	for (size_t i = 0; i < nCount; i++)
	{
		pItem = m_SubFieldAry.at(i);

		if (pItem->m_nSubField == nSubField)
		{
			wSubFieldIndex.push_back((WORD)i);
			bFound = true;
		}
	}

	if (bFound)
	{
		// First delete the items to delete, but backwards (!) to keep the indexes
		// in wSubFieldIndex pertinent.
		for (int i = (int)wSubFieldIndex.size()-1; i >= 0; i--)
		{
			index = wSubFieldIndex.at(i);
			pItem = m_SubFieldAry.at(index);
			m_SubFieldAry.erase(m_SubFieldAry.begin() + index);
			if (pItem != NULL) delete pItem;
		}
		// Next decrementing all Subfield indexes above the last one deleted.
		// This is to maintain the necessary sequential numbering scheme.
		wLastIndexDeleted = wSubFieldIndex.at(0);
		nCount = m_SubFieldAry.size();	// re-adjust count
		for (size_t i = wLastIndexDeleted; i < nCount; i++)
		{
			pItem = m_SubFieldAry.at(i);
			pItem->m_nSubField--;
		}
	}

	return nRet;
}

int CNISTField::GetNumSubfields()
{
	int nRet = 0;
	size_t nCount = m_SubFieldAry.size();
	unsigned int i;

	if (nCount == 1) // usual case
		nRet = 1;
	else
	{
		CSubFieldItem *pSubField;
		int nSparseAry[MAX_SUBFIELDS_PER_FIELD] = { 0, };

		for (i = 0; i < nCount; i++)
		{
			pSubField = m_SubFieldAry.at(i);

			nSparseAry[pSubField->m_nSubField]++;
		}
		for (i = 1; i < MAX_SUBFIELDS_PER_FIELD; i++)
		{
			if (nSparseAry[i])
				nRet++;
			else
				break;
		}
	}

	return nRet;
}

int CNISTField::AddItem(int RecordType, int Field, int Subfield, int Item, CStdString sData)
{
	int nRet = IW_SUCCESS;

	m_nRecordType = RecordType;
	m_nField = Field;

	nRet = SetSubField(Subfield, Item, sData);

	return nRet;
}

int CNISTField::SetImageData(unsigned char *pImage, int nImageLen)
{
	if (m_pImageData)
	{
		m_nImageLen = 0;
		delete m_pImageData;
		m_pImageData = 0;
	}

	if (nImageLen <= 0)
		return IW_SUCCESS;

	int nRet = IW_ERR_OUT_OF_MEMORY;

	IWS_BEGIN_EXCEPTION_METHOD("CNISTField::SetImageData")

	IWS_BEGIN_CATCHEXCEPTION_BLOCK()

	int nLen = nImageLen;

	m_pImageData = new unsigned char[nLen];

	memcpy(m_pImageData, pImage, nImageLen);
	m_nImageLen = nImageLen;

	if (CNISTRecord::IsBinaryFingerType(m_nRecordType))
	{
		m_nImageLen -= sizeof(FINGERPRINT_HEADER);
	}
	else if (m_nRecordType == RECORD_TYPE8)
	{
		m_nImageLen -= sizeof(SIGNATURE_HEADER);
	}

	m_ImageFormat = CNISTField::GetImageFormatFromHeader(m_nRecordType, m_pImageData);

	nRet = IW_SUCCESS;

	IWS_END_CATCHEXCEPTION_BLOCK()

	return nRet;
}

int CNISTField::FindItem(int nSubfield, int nItem, CStdString& sData)
{
	int nRet = IW_SUCCESS;
	bool bFound = false;

	sData.Empty();

	size_t nCount = m_SubFieldAry.size();
	CSubFieldItem *pSubfield;

	for (unsigned int i = 0; i < nCount; i++)
	{
		pSubfield = m_SubFieldAry.at(i);

		if (pSubfield->m_nSubField == nSubfield && pSubfield->m_nSubFieldItem == nItem)
		{
			sData = pSubfield->m_sData;
			bFound = true;
			break;
		}
	}

	if (!bFound) nRet = IW_ERR_RECORD_NOT_FOUND;

	return nRet;
}

CStdString CNISTField::ImageExtFromImageFormat(OpenEBTSImageFormat fmt)
{
	switch (fmt)
	{
		case fmtRAW: return _T("raw"); break;
		case fmtBMP: return _T("bmp"); break;
		case fmtJPG: return _T("jpg"); break;
		case fmtJP2: return _T("jp2"); break;
		case fmtPNG: return _T("png"); break;
		case fmtFX4: return _T("fx4"); break;
		case fmtWSQ: return _T("wsq"); break;
		case fmtCBEFF: return _T("cbeff"); break;
		default: return _T("raw");
	}
}

OpenEBTSImageFormat CNISTField::ImageFormatFromImageExt(CStdString sFormat)
{
	sFormat.MakeLower();
	if (sFormat == _T("raw")) return fmtRAW;
	if (sFormat == _T("bmp")) return fmtBMP;
	if (sFormat == _T("jpg")) return fmtJPG;
	if (sFormat == _T("jp2")) return fmtJP2;
	if (sFormat == _T("png")) return fmtPNG;
	if (sFormat == _T("fx4")) return fmtFX4;
	if (sFormat == _T("wsq")) return fmtWSQ;
	if (sFormat == _T("cbeff")) return fmtCBEFF;
	return fmtUNK; // Format unknown
}

OpenEBTSImageFormat CNISTField::GetImageFormatFromHeader(int nRecordType, const BYTE* pImage)
// Note that a dangerous assumption here is that a potential RAW image won't start
// with one of the other image file format headers.
// TODO: must differentiate between "cbeff" and "raw" somehow.
{
	BYTE wsqHdr[2]  = { 0xFF, 0xA0 };
	BYTE jpgHdr[4]  = { 0xFF, 0xD8, 0xFF, 0xE0 };
	BYTE exfHdr[4]  = { 0xFF, 0xD8, 0xFF, 0xE1 };
	BYTE bmpHdr[2]  = { 0x42, 0x4D };
	BYTE pngHdr[8] =  { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	BYTE jp2Hdr[23] = { 0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20, 0x0D, 0x0A, 0x87, 0x0A, 
					    0x00, 0x00, 0x00, 0x14, 0x66, 0x74, 0x79, 0x70, 0x6A, 0x70, 0x32 };
	BYTE tif1Hdr[2] = { 0x4D, 0x4D };
	BYTE tif2Hdr[2] = { 0x49, 0x49 };

	BYTE *pImageStart;

	pImageStart = (BYTE*)pImage;
	if (CNISTRecord::IsBinaryFingerType(nRecordType)) 
	{
		pImageStart += sizeof(FINGERPRINT_HEADER);
	} else if (nRecordType == RECORD_TYPE8)
	{
		pImageStart += sizeof(SIGNATURE_HEADER);
	}

	if (!memcmp(pImageStart, wsqHdr, 2))
	{
		return fmtWSQ;
	}
	else if ((!memcmp(pImageStart, jpgHdr, 4) && !memcmp(pImageStart+6, "JFIF", 4)) ||
			(!memcmp(pImageStart, exfHdr, 4) && !memcmp(pImageStart+6, "Exif", 4)))
	{
		return fmtJPG;
	}
	else if (!memcmp(pImageStart, jp2Hdr, 23))
	{
		return fmtJP2;
	}
	else if (!memcmp(pImageStart, bmpHdr, 2))
	{
		return fmtBMP;
	}
	else if (!memcmp(pImageStart, pngHdr, 8))
	{
		return fmtPNG;
	}
	else if (!memcmp(pImageStart, tif1Hdr, 2) || !memcmp(pImageStart, tif2Hdr, 2))
	{
		return fmtFX4;
	}
	else // assume RAW otherwise
	{
		return fmtRAW;
	}
}

int CNISTField::GetNumItems(int Subfield, int *pCount)
{
	int nRet = IW_SUCCESS;
	size_t nCount = m_SubFieldAry.size();
	CSubFieldItem *pTemp;

	*pCount = 0;

	for (unsigned int i = 0; i < nCount; i++)
	{
		pTemp = m_SubFieldAry.at(i);
		if (pTemp->m_nSubField == Subfield) (*pCount)++;
	}

	return nRet;
}

void CNISTField::GetFieldLabel(char* szLabel, int nMaxChars, int nRecordType, int nFieldIndex)
// Note: Field labels ALWAYS get written in ASCII, so we use regular char functions.
// For Type-1 records, use 2 digits by default, unless we happen to need more.
// For all other record types, use 3 digits by default, unless we happen to need more.
// Note that the orinal ANSI/NIST-ITL 1-2007 standard let's us use as many digits as we want,
// but some agencies prefer the specific method used here.
{
	int numDigitsNeeded;
	int numDigitsToUse;
	char szFormat[16];

	if (nFieldIndex < 1)
	{
		// Probably won't happen
		strncpy(szLabel, "", nMaxChars);
		return;
	}

	numDigitsNeeded = (int)log10((double)nFieldIndex) + 1;

	if (nRecordType == 1)
	{
		numDigitsToUse = max(2, numDigitsNeeded);
	}
	else
	{
		numDigitsToUse = max(3, numDigitsNeeded);
	}

	// Generate "%d.%0[N]d:" where [N] is numDigitsToUse
	snprintf(szFormat, 16, "%%d.%%0%dd:", numDigitsToUse);
	// Now apply "%d.%0[N]d:" to m_nRecordType and nFieldIndex
	snprintf(szLabel, nMaxChars, szFormat, nRecordType, nFieldIndex);
}

int CNISTField::GetWriteLen()
{
	size_t nLen = 0;
	size_t nCount = m_SubFieldAry.size();

	char szFieldLabel[20];
	size_t nLabelLen = 0;

	GetFieldLabel(szFieldLabel, sizeof(szFieldLabel), m_nRecordType, m_nField);
	nLabelLen = strlen(szFieldLabel);

	if (nCount)
	{
		CSubFieldItem *pSubField;

		for (unsigned int i = 0; i < nCount; i++)
		{
			pSubField = m_SubFieldAry.at(i);

			if (pSubField)
			{
				nLen += pSubField->GetLength();
				nLen++; // sub field separator
			}
		}
		if (nLen) nLen--; // remove last sub field separator
	}
	else if (m_pImageData)
	{
		nLen = m_nImageLen;
	}

	nLen += nLabelLen;
	nLen++; // for GS field separator

	if (IsLogging())
	{
		CStdString sLogMessage;
		sLogMessage.Format(IDS_LOGFIELDGETWRITELEN, m_nRecordType, m_nField, nLen);
		LogMessageVerbose(sLogMessage);
	}

	return (int)nLen;
}

struct SubfieldItemSort
{
	 bool operator()(CSubFieldItem*& rp1, CSubFieldItem*& rp2)
	 {
		if (rp1->m_nSubField == rp2->m_nSubField)
		// Subfield indices are equal, we sort by the item indices
		{
			return (rp1->m_nSubFieldItem < rp2->m_nSubFieldItem);
		}
		else
		{
			return (rp1->m_nSubField < rp2->m_nSubField);
		}
	 }
	 bool operator()(CSubFieldItem*& rp1, CSubFieldItem* const& rp2)
	 {
		if (rp1->m_nSubField == rp2->m_nSubField)
		// Subfield indices are equal, we sort by the item indices
		{
			return (rp1->m_nSubFieldItem < rp2->m_nSubFieldItem);
		}
		else
		{
			return (rp1->m_nSubField < rp2->m_nSubField);
		}
	 }
	 bool operator()(CSubFieldItem* const& rp1, CSubFieldItem*& rp2)
	 {
		if (rp1->m_nSubField == rp2->m_nSubField)
		// Subfield indices are equal, we sort by the item indices
		{
			return (rp1->m_nSubFieldItem < rp2->m_nSubFieldItem);
		}
		else
		{
			return (rp1->m_nSubField < rp2->m_nSubField);
		}
	 }
	 bool operator()(CSubFieldItem* const& rp1, CSubFieldItem* const& rp2)
	 {
		if (rp1->m_nSubField == rp2->m_nSubField)
		// Subfield indices are equal, we sort by the item indices
		{
			return (rp1->m_nSubFieldItem < rp2->m_nSubFieldItem);
		}
		else
		{
			return (rp1->m_nSubField < rp2->m_nSubField);
		}
	 }
};

int CNISTField::Write(BYTE *pBuffer, int *poffset, bool bRecordSeparator)
{
	int nRet = IW_ERR_WRITING_FILE;
	size_t nCount;

	if (IsLoggingVerbose())
	{
		CStdString sLogMessage;
		sLogMessage.Format(IDS_LOGFIELDWRITE, m_nRecordType, m_nField);
		LogMessageVerbose(sLogMessage);
	}

	IWS_BEGIN_EXCEPTION_METHOD("CNISTField::Write")
	IWS_BEGIN_CATCHEXCEPTION_BLOCK()

	// Before writing out all subfields and items we sort the list so we can just output them
	// sequentially afterwards. We sort the list first by subfield index then by item index.
	std::sort(m_SubFieldAry.begin(), m_SubFieldAry.end(), SubfieldItemSort());

	// Write the field and subfield data
	char szFieldLabel[40];

	if (m_nField == 1) // handle LEN field seperately
	{
		int len = _tcstol(m_SubFieldAry.at(0)->m_sData, NULL, 10);

		GetFieldLabel(szFieldLabel, sizeof(szFieldLabel), m_nRecordType, m_nField);
		// Append lenght]
		char szLength[16];
		snprintf(szLength, sizeof(szLength), "%d", len);
		strncat(szFieldLabel, szLength, sizeof(szFieldLabel));

		memcpy(pBuffer + *poffset, szFieldLabel, strlen(szFieldLabel));	// Field tag is ALWAYS in regular ASCII
		*poffset = *poffset + (int)strlen(szFieldLabel);

	}
	else
	{
		nCount = m_SubFieldAry.size();

		// Add the field descriptor
		GetFieldLabel(szFieldLabel, sizeof(szFieldLabel), m_nRecordType, m_nField);

		memcpy(pBuffer + *poffset, szFieldLabel, strlen(szFieldLabel));	// Field tag is ALWAYS in regular ASCII
		*poffset = *poffset + (int)strlen(szFieldLabel);

		if (nCount)
		{
			CSubFieldItem *pSubField, *pNextSubField;

			for (unsigned int i = 0; i < nCount; i++)
			{
				pSubField = m_SubFieldAry.at(i);

				pNextSubField = 0;
				if (i+1 < nCount)
					pNextSubField = m_SubFieldAry.at(i+1);

				if (pSubField)
				{
					// Write textual field data. If in UNICODE, write it in UTF-8, otherwise
					// we write it in regular ASCII.
					if (!pSubField->m_sData.IsEmpty())
					{
#ifdef UNICODE
						// In the UNICODE version of OpenEBTS, we write out UNICODE strings as UTF-8.
						char *p = NULL;
						int  nLength;

						if (UCStoUTF8(pSubField->m_sData, &p, &nLength))
						{
							if (p)
							{
								memcpy(pBuffer + *poffset, p, nLength-1);	
								*poffset = *poffset + nLength-1;

								delete p;
							}
							else
							{
								goto done;		// critical error
							}
						}
						else
						{
							goto done;	// failed encoding to UTF-8
						}
#else
						// Write it as regular ASCII
						memcpy(pBuffer + *poffset, pSubField->m_sData.c_str(), pSubField->m_sData.GetLength() );
						*poffset = *poffset + pSubField->m_sData.GetLength();
#endif
					}

					// Separators get written as regular ASCII
					if (pNextSubField)
					{
						if (pSubField->m_nSubField == pNextSubField->m_nSubField)
						{
							memcpy(pBuffer + *poffset, g_szUS, strlen(g_szUS));
							*poffset = *poffset + (int)strlen(g_szUS);
						}
						else
						{
							memcpy(pBuffer + *poffset, g_szRS, strlen(g_szRS));
							*poffset = *poffset + (int)strlen(g_szRS);
						}
					}
				}
			}
		}
		else if (m_pImageData)
		{
			// Image data also get written as regular ASCII
			memcpy(pBuffer + *poffset, m_pImageData, m_nImageLen);
			*poffset = *poffset + m_nImageLen;
		}
	}

	// Separators get written as regular ASCII
	if (bRecordSeparator)
	{
		memcpy(pBuffer + *poffset, g_szFS, strlen(g_szFS));
		*poffset = *poffset + (int)strlen(g_szFS);
	}
	else
	{
		memcpy(pBuffer + *poffset, g_szGS, strlen(g_szGS));
		*poffset = *poffset + (int)strlen(g_szGS);
	}

	nRet = IW_SUCCESS;

	IWS_END_CATCHEXCEPTION_BLOCK()

#ifdef UNICODE
done:
#endif
	return nRet;
}

