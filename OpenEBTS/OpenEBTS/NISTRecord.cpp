#include "Includes.h"
#include "NISTField.h"
#include "NISTRecord.h"
#include "OpenEBTS.h"
extern "C"
{
#include <wsq.h>
}


CNISTRecord::CNISTRecord()
{
	m_nRecordType = -1;
	m_nRecordLen = 0;
	m_dNativeResolutionPPMM = 0.0;
}

CNISTRecord::~CNISTRecord()
{
	size_t nSize = m_FieldList.size();
	CNISTField *pField;

	for (unsigned int i = 0; i < nSize; i++)
	{
		pField = m_FieldList.at(i);
		if (pField) delete pField;
	}
	m_FieldList.clear();
}

void CNISTRecord::InitializeNewRecord(int nRecordType)
{
	CNISTField *pField = new CNISTField;
	CStdString	sData;
	int nRet;

	if (IsLoggingVerbose())
	{
		CStdString sLogMessage;
		sLogMessage.Format(IDS_LOGRECINITNEW, nRecordType);
		LogMessageVerbose(sLogMessage);
	}

	m_nRecordType = nRecordType;

	// 2 fields must appear in every record, and they
	// must be the first 2 fields in the record
	// by default always add the .001 and .002 records

	sData = _T("0");	// Set length to 0
	if ((nRet = pField->AddItem(nRecordType, REC_TAG_LEN, 1/*Subfield*/, 1/*Item*/, sData)) == IW_SUCCESS)
	{
		AddField(pField);

		pField = new CNISTField;

		if (nRecordType == RECORD_TYPE1)
			sData = _T("0400");		// default version
		else
			sData = _T("00");		// default IDC value, should be correctly set by calling application?

		if ((nRet = pField->AddItem(nRecordType, (nRecordType == RECORD_TYPE1 ? REC_TAG_VER : REC_TAG_IDC), 1/*Subfield*/, 1/*Item*/, sData)) == IW_SUCCESS)
			AddField(pField);

		if (nRecordType == RECORD_TYPE1) // initialize an empty contents field
		{
			pField = new CNISTField;
			sData = _T("1");
			if ((nRet = pField->AddItem(nRecordType, TYPE1_CNT, 1/*Subfield*/, 1/*Item*/, sData)) == IW_SUCCESS)
			{
				sData = _T("0"); 
				if ((nRet = pField->AddItem(nRecordType, TYPE1_CNT, 1/*Subfield*/, 2/*Item*/, sData)) == IW_SUCCESS)
					AddField(pField);
			}
		}
	}
}

void CNISTRecord::SetNativeScanningResolution(double dNativeResolutionPPMM)
{
	m_dNativeResolutionPPMM = dNativeResolutionPPMM;
}

int CNISTRecord::ReadLogicalRecordLen(CIWStrTokParams& parserParms, BYTE* pTransactionData, int nRecordType, int nRecordIndex)
{
	char szTemp[120];
	char *pTemp;
	int nRet = 0;

	// grab the first few bytes, the len field is always first and should be in here
	memcpy(szTemp, pTransactionData, sizeof(szTemp));

	pTemp = IWStrTok(parserParms, szTemp, CHAR_PERIOD);

	if (pTemp)
	{
		if (atoi(pTemp) == nRecordType)
		{
			pTemp = IWStrTok(parserParms, NULL, CHAR_COLON);

			if (pTemp)
			{
				if (atoi(pTemp) == REC_TAG_LEN)
				{
					pTemp = IWStrTok(parserParms, NULL, CHAR_GS);

					if (pTemp)
						nRet = atoi(pTemp);
				}
			}
		}
	}

	if (IsLoggingVerbose())
	{
		CStdString sLogMessage;
		sLogMessage.Format(IDS_LOGRECREADRECLEN, nRecordType, nRecordIndex, nRet);
		LogMessageVerbose(sLogMessage);
	}

	return nRet;
}

int CNISTRecord::ReadRecord(BYTE* pTransactionData, int nRecordType)
{
	int nRet = IW_SUCCESS;

	if (IsLoggingVerbose())
	{
		CStdString sLogMessage;
		sLogMessage.Format(IDS_LOGRECREADREC, nRecordType);
		LogMessageVerbose(sLogMessage);
	}

	m_nRecordType = nRecordType;
	m_bGetImage = false;

	CIWStrTokParams parserParms;

	m_nRecordLen = ReadLogicalRecordLen(parserParms, pTransactionData, nRecordType); // get size of this record

	if (m_nRecordLen > 0)
	{
		BYTE* pRecord = new BYTE[m_nRecordLen];

		if (pRecord)
		{
			int			nField;
			BYTE		*pFieldData;
			CStdString	sErr;
			CNISTField	*pField;
			BYTE		*pTemp;
			bool		bEndofRecord = false;

			memcpy(pRecord, pTransactionData, m_nRecordLen * sizeof(BYTE));

			pTemp = (BYTE*)IWStrTok(parserParms, (char*)pRecord, CHAR_PERIOD);
	
			while (pTemp && !bEndofRecord)
			{
				nField = atoi((char*)pTemp);

				if (nField == nRecordType) // make sure we're in sync
				{
					pTemp = (BYTE*)IWStrTok(parserParms, NULL, CHAR_COLON, &bEndofRecord); // get the field

					if (pTemp)
					{
						nField = atoi((char*)pTemp);

						if (IsDATField(m_nRecordType, nField))
						{
							// take advantage of the fact that I know
							// the pointer is sitting at the beginning of the image bytes
							m_bGetImage = true;
							pFieldData = (BYTE*)IWStrTok(parserParms, NULL, CHAR_GS, &bEndofRecord); // get the field	data	
							m_bGetImage = false;
							// Explicitly set bEndofRecord because we know that DAT fields are
							// always the last fields
							bEndofRecord = true;
						}
						else
							pFieldData = (BYTE*)IWStrTok(parserParms, NULL, CHAR_GS, &bEndofRecord); // get the field	data	

						// This shouldn't happen, but can if the file is corrupted
						if (!pFieldData)
						{
							return IW_ERR_READING_FILE;
						}

						pField = new CNISTField;
						pField->m_nRecordType = nRecordType;
						pField->m_nField = nField;

						if (nField == REC_TAG_LEN || nField == REC_TAG_IDC)
						// these don't need to be handled special 
						// but it helps for debugging
						{
							// LEN and IDC are always in ASCII, no conversion necessary here
							CStdString sFieldData((char*)pFieldData);
							pField->SetSubField(1, 1, sFieldData);
						}
						else if (IsDATField(m_nRecordType, nField))
						{
							if (pFieldData)
							{
								int nOffset = (int)(pFieldData - pRecord);
								BYTE *pImage = (BYTE*)pTransactionData + nOffset;
								int nImageLen = m_nRecordLen - nOffset - 1;

								pField->SetImageData(pImage, nImageLen);
							}
						}
						else
						{
							if (strchr((char*)pFieldData, CHAR_US) || strchr((char*)pFieldData, CHAR_RS))
							{
								AddSubItems(pField, (char*)pFieldData);
							}
							else
							{
								// Field may be in UTF-8, convert if using the UNICODE version of OpenEBTS
								CStdString sFieldData;
#ifdef UNICODE
								// Field data may be in UTF-8, let's get it properly into wide char land
								wchar_t *wNew;

								if (!UTF8toUCS((char*)pFieldData, &wNew))
								{
									return IW_ERR_READING_FILE;	// Error decoding UTF-8
								}
								sFieldData = wNew;
								delete wNew;
#else
								sFieldData = (char*)pFieldData;
#endif
								pField->SetSubField(1, 1, sFieldData);
							}
						}
						AddField(pField);
					}
				}
				// get next field
				if (!bEndofRecord)
				{
					pTemp = (BYTE*)IWStrTok(parserParms, NULL, CHAR_PERIOD, &bEndofRecord);
				}
			}
			delete [] pRecord;
		}
	}
	else
		nRet = IW_ERR_READING_FILE;

	return nRet;
}

//
// IWStrTok was never thread safe - it should really be rewritten to not even use statics as
// this is just messy, but for now we'll use __declspec(thread) static to make it work
//

char* CNISTRecord::IWStrTok(CIWStrTokParams& position, char *pInStr, char cDelim, bool *pbEndofRecord)
{
	char *pTemp;
	char *pRet = 0;

	if (m_bGetImage)
		return position.pCurPos; 

	if (pInStr)
	{
		position.pCurPos = position.pString = pInStr;
		position.nCurPos = 0;
		size_t nLen = strlen(pInStr);
		position.pEndString = position.pString+nLen;
	}
	
	while (position.pCurPos <= position.pEndString)
	{
		if (*position.pCurPos == cDelim || *position.pCurPos == CHAR_FS)
		{
			if (*position.pCurPos == CHAR_FS && pbEndofRecord)
				*pbEndofRecord = true;

			if (position.pCurPos > position.pString)
			{
				pTemp = position.pCurPos-1;
				
				*position.pCurPos = '\0'; // break the string at the delimiter
				position.pCurPos++; // move off null
				
				// move to begin of previous field
				while (pTemp > position.pString && *pTemp != '\0')
					pTemp--;

				// move off null
				if (*pTemp == '\0')
					pTemp++;
					
				pRet = pTemp;
			}
			else
			{
				*position.pCurPos = '\0'; // return empty string
				pRet = position.pCurPos;
			}

			break;
		}
		else
			position.pCurPos++;
	}
	
	return pRet;
}

/*********************************************************/
/* Binary Record methods                                 */
/*********************************************************/

int CNISTRecord::ReadBinaryRecord(BYTE* pTransactionData, int nRecordType)
{
	int nRet = IW_SUCCESS;
	BYTE bIDC;

	if (IsLoggingVerbose())
	{
		CStdString sLogMessage;
		sLogMessage.Format(IDS_LOGRECREADRECBIN, nRecordType);
		LogMessageVerbose(sLogMessage);
	}

	m_nRecordType = nRecordType;
	m_bGetImage = false;

	if (nRecordType != RECORD_TYPE8)
	{
		FINGERPRINT_HEADER *pHdr = (FINGERPRINT_HEADER*)pTransactionData;
		bIDC = pHdr->bIDC;
	}
	else
	{
		SIGNATURE_HEADER *pHdr = (SIGNATURE_HEADER*)pTransactionData;
		bIDC = pHdr->bIDC;
	}

	CStdString sFieldData;

	IWS_BEGIN_EXCEPTION_METHOD("CNISTRecord::ReadBinaryRecord")
	IWS_BEGIN_CATCHEXCEPTION_BLOCK()

	CNISTField *pField = new CNISTField(nRecordType, REC_TAG_LEN);

	// the first 2 fields are the same for all binary records, LEN and IDC
	sFieldData.Format(_T("%d"), GetDecimalValue(pTransactionData, 4));
	pField->SetSubField(1, REC_TAG_LEN, sFieldData);
	AddField(pField);
	m_nRecordLen = (int)_tcstol(sFieldData, NULL, 10);

	pField = new CNISTField(nRecordType, REC_TAG_IDC);
	sFieldData.Format(_T("%d"), bIDC);
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	if (CNISTRecord::IsBinaryFingerType(nRecordType))
	{
		nRet = GetFingerprintInfo(nRecordType, pTransactionData);
		pField = new CNISTField(nRecordType, TYPE4_DAT); //9 in all cases
	}
	else if (nRecordType == RECORD_TYPE8)
	{
		nRet = GetSignatureInfo(pTransactionData);
		pField = new CNISTField(nRecordType, TYPE8_DAT); //8 in this case
	}
	pField->SetImageData((BYTE*)pTransactionData, m_nRecordLen);
	AddField(pField);

	IWS_END_CATCHEXCEPTION_BLOCK()

	// If we wanted to write out NIST records with fingerprint info we
	// would need to store the fingerprint data as well.
	// We would need to be careful to write fields correctly especially
	// the hex fields, 1, 4, 6 and 7. But at the moment we only care
	// about reading NIST files that contain fingerprint info.

	return nRet;
}

int CNISTRecord::GetFingerprintInfo(int nRecordType, BYTE* pTransactionData)
{
	int nRet = IW_SUCCESS;
	FINGERPRINT_HEADER *pFPrintHdr = (FINGERPRINT_HEADER *)pTransactionData;
	CNISTField *pField;
	CStdString sFieldData;

	pField = new CNISTField(nRecordType, 3);	// Impression Type
	sFieldData.Format(_T("%d"), pFPrintHdr->bImp);
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	pField = new CNISTField(nRecordType, 4);	// Finger position - 6 guesses allowed
	for (int i=0;i<6;i++)
	{
		sFieldData.Format(_T("%d"), GetDecimalValue(((BYTE*)pFPrintHdr->bFGP)+i, 1));
		pField->SetSubField((i+1), 1, sFieldData);
	}
	AddField(pField);

	pField = new CNISTField(nRecordType, 5);	// Scanning resolution
	sFieldData.Format(_T("%d"), pFPrintHdr->bISR);
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	pField = new CNISTField(nRecordType, 6);	// Horiz line length
	sFieldData.Format(_T("%d"), GetDecimalValue((BYTE*)pFPrintHdr->bHLL, 2));
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	pField = new CNISTField(nRecordType, 7);	// Vert line length
	sFieldData.Format(_T("%d"), GetDecimalValue((BYTE*)pFPrintHdr->bVLL, 2));
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	pField = new CNISTField(nRecordType, 8);	// Compression algorithm
	sFieldData.Format(_T("%d"), pFPrintHdr->bComp);
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	return nRet;
}

int CNISTRecord::GetSignatureInfo(BYTE* pTransactionData)
{
	int nRet = IW_SUCCESS;
	SIGNATURE_HEADER *pSig = (SIGNATURE_HEADER *)pTransactionData;
	CNISTField *pField;
	int nRecordType = RECORD_TYPE8;
	CStdString sFieldData;

	pField = new CNISTField(nRecordType, 3);	// Signature type
	sFieldData.Format(_T("%d"), pSig->bSIG);
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	pField = new CNISTField(nRecordType, 4);	// Signature representation type
	sFieldData.Format(_T("%d"), pSig->bSRT);
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	pField = new CNISTField(nRecordType, 5);	// Scanning resolution
	sFieldData.Format(_T("%d"), pSig->bISR);
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	pField = new CNISTField(nRecordType, 6);	// Horiz line length
	sFieldData.Format(_T("%d"), GetDecimalValue((BYTE*)pSig->bHLL, 2));
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	pField = new CNISTField(nRecordType, 7);	// Vert line length
	sFieldData.Format(_T("%d"), GetDecimalValue((BYTE*)pSig->bVLL, 2));
	pField->SetSubField(1, 1, sFieldData);
	AddField(pField);

	return nRet;
}

int CNISTRecord::GetDecimalValue(BYTE* pInStr, int nStrLen)
{
	char szHexStr[20] = { '\0', };
	char szTemp[5];

	for (int i = 0; i < nStrLen; i++)
	{
		memset(szTemp, 0, sizeof(szTemp));
		strcat(szHexStr, BytetoHexString(pInStr[i], szTemp));
	}

	return (int)strtol(szHexStr, NULL, 16);
}

char *CNISTRecord::BytetoHexString(BYTE lDecimalValue, char *pszHexString)
{
	char szHex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	
	pszHexString[0] = szHex[lDecimalValue/16];
	pszHexString[1] = szHex[lDecimalValue%16];

	return pszHexString;	
}

/*********************************************************/
/* End Binary Record methods                             */
/*********************************************************/

int CNISTRecord::GetNextField(int nField, int *pNextField)
{
	size_t nSize = m_FieldList.size();
	CNISTField *pField;
	int nRet = IW_ERR_RECORD_NOT_FOUND;

	*pNextField = 0;

	for (unsigned int i = 0; i < nSize; i++)
	{
		pField = m_FieldList.at(i);

		if (pField->m_nField > nField)
		{
			*pNextField = pField->m_nField;
			nRet = IW_SUCCESS;
			break;
		}
	}

	return nRet;
}

int CNISTRecord::GetNumFields(int *pCount)
{
	if (pCount)
		*pCount = (int)m_FieldList.size();
	return IW_SUCCESS;
}

int CNISTRecord::GetNumSubfields(int nField, int *pCount)
{
	int nRet = IW_ERR_INDEX_OUT_OF_RANGE;
	CNISTField *pField = GetNISTField(nField);

	*pCount = 0;

	if (pField)
	{
		*pCount = pField->GetNumSubfields();
		nRet = IW_SUCCESS;
	}

	return nRet;
}

CNISTField *CNISTRecord::GetNISTField(int nField)
{
	size_t nSize = m_FieldList.size();
	CNISTField *pRetField = 0;
	CNISTField *pField = 0;

	for (unsigned int i = 0; i < nSize; i++)
	{
		pField = m_FieldList.at(i);

		if (pField->m_nField == nField)
		{
			pRetField = pField;
			break;
		}
	}

	return pRetField;
}

int CNISTRecord::AddSubItems(CNISTField *pNISTField, char *szFieldData)
// Note that this function always operates in the regular ASCII character space
{
	int nRet = IW_SUCCESS;
	char szRS[2];

	szRS[0] = CHAR_RS;
	szRS[1] = '\0';

	if (szFieldData && *szFieldData)
	{
		int nLen = (int)strlen(szFieldData);

		if (nLen)
		{
			char *pTemp;
			char *pTempSubItem;
			char *pTempSub;
			int nSubField = 1;
			int nSubFieldLen;
			int nSubFieldItem = 1;
			int nSubFieldItemLen = 0;
			int i;
			CSubFieldItem *pSubItem;

			pTemp = strtok(szFieldData, szRS);
			while (pTemp)
			{
				pTempSubItem = pTempSub = pTemp;
				
				// get the list of subfield items
				if (strchr(pTempSub, CHAR_US))
				{
					nSubFieldItem = 1;
					nSubFieldLen = (int)strlen(pTempSub);
					for (i = 0; i <= nSubFieldLen; i++)
					{
						// add the subfield items
						if (*pTempSub == CHAR_US || *pTempSub == 0)
						{
							pSubItem = new CSubFieldItem;
							pSubItem->m_nSubField = nSubField;
							pSubItem->m_nSubFieldItem = nSubFieldItem++;
							nSubFieldItemLen = (int)(pTempSub - pTempSubItem);

							if (nSubFieldItemLen)
							{
								*pTempSub = '\0'; // chop off our string
#ifdef UNICODE
								// Field data may be in UTF-8, let's get it properly into wide char land
								wchar_t *wNew;

								if (!UTF8toUCS(pTempSubItem, &wNew))
								{
									return IW_ERR_READING_FILE;	// Error decoding UTF-8
								}

								pSubItem->m_sData = wNew;
								delete wNew;
#else
								pSubItem->m_sData = pTempSubItem;
#endif
							}
							pTempSubItem = pTempSub+1; // get past null

							pNISTField->m_SubFieldAry.push_back(pSubItem);
						}
						pTempSub++;
					}
				}
				else // only 1 subfield item
				{
					pSubItem = new CSubFieldItem;
					pSubItem->m_nSubField = nSubField;
					pSubItem->m_nSubFieldItem = 1;
					nSubFieldItemLen = (int)strlen(pTemp);
					if (nSubFieldItemLen)
					{
#ifdef UNICODE
						// Field data may be in UTF-8, let's get it properly into wide char land
						wchar_t *wNew;

						if (!UTF8toUCS(pTempSubItem, &wNew))
						{
							return IW_ERR_READING_FILE;	// Error decoding UTF-8
						}

						pSubItem->m_sData = wNew;
						delete wNew;
#else
						pSubItem->m_sData = pTempSubItem;
#endif
					}
					pNISTField->m_SubFieldAry.push_back(pSubItem);
				}
				nSubField++;

				if (pTempSub >= szFieldData + nLen)
					pTemp = NULL;
				else
					pTemp = strtok(NULL, szRS);
			}
		}
	}

	return nRet;
}

int CNISTRecord::GetNumItems(int Field, int Subfield, int* Count)
{
	int nRet = IW_ERR_INVALID_SUBFIELD_NUM;
	CNISTField *pField = GetNISTField(Field);

	if (pField)
		nRet = pField->GetNumItems(Subfield, Count);

	return nRet;
}

void CNISTRecord::AddField(CNISTField *pField)
{
	size_t nSize = m_FieldList.size();
	CNISTField *pTemp = 0;
	int nPos = 0;

	for (unsigned int i = 0; i < nSize; i++)
	{
		pTemp = m_FieldList.at(i);

		if (pTemp && pTemp->m_nField > pField->m_nField)
		{
			nPos = i;
			break;
		}
	}

	if (!nPos)
		m_FieldList.push_back(pField);
	else
		m_FieldList.insert(m_FieldList.begin() + nPos, pField);
}

int CNISTRecord::RemoveItem(int Field, int Subfield, int Item)
{
	int nRet = IW_SUCCESS;

	size_t nSize = m_FieldList.size();
	int nPos = -1;
	CNISTField *pTemp;

	// KAS 6/22/10
	// For now we simply remove the entire field and
	// ignore the subfield and item values
	for (unsigned int i = 0; i < nSize; i++)
	{
		pTemp = m_FieldList.at(i);

		if (pTemp && pTemp->m_nField == Field)
		{
			nPos = i;
			break;
		}
	}

	if (nPos > -1 && nPos < (int)nSize)
		m_FieldList.erase(m_FieldList.begin()+nPos);

	return nRet;
}

int CNISTRecord::FindItem(int Field, int Subfield, int Item, CStdString& sData)
{
	int nRet = IW_ERR_RECORD_NOT_FOUND;

	CNISTField *pField = GetNISTField(Field);

	if (pField)
	{
		if (IsDATField(m_nRecordType, Field))
		{
			// Don't return anything for images, but do return OK if it exists
			sData = "";
			nRet = IW_SUCCESS;
		}
		else
		{
			if (Item == 0)
			{
				// Field exists but this position refers to the header item defined by
				// a Location Form 4, and hence itself does not contain any data
				sData = "";
				nRet = IW_ERR_HEADER_ITEM;
			}
			else
			{
				nRet = pField->FindItem(Subfield, Item, sData);
			}
		}
	}

	return nRet;
}

int CNISTRecord::DeleteSubfield(int Field, int Subfield)
{
	int nRet = IW_ERR_INVALID_SUBFIELD_NUM;
	size_t nSize = m_FieldList.size();
	CNISTField *pField = 0;

	for (unsigned int i = 0; i < nSize; i++)
	{
		pField = m_FieldList.at(i);

		if (pField->m_nField == Field)
		{
			pField->RemoveSubField(Subfield);
			nRet = IW_SUCCESS;
			break;
		}
	}

	return nRet;
}

int CNISTRecord::SetItem(CStdString sData, int Field, int Subfield, int Item)
{
	int nRet = IW_SUCCESS;
	CNISTField *pField = GetNISTField(Field);

	if (IsLoggingVerbose())
	{
		CStdString sLogMessage;
		sLogMessage.Format(IDS_LOGRECSETITEM, Field, Subfield, Item, sData);
		LogMessageVerbose(sLogMessage);
	}

	if (pField)
	{
		nRet = pField->SetSubField(Subfield, Item, sData);
	}
	else
	{
		pField = new CNISTField;
		if ((nRet = pField->AddItem(m_nRecordType, Field, Subfield, Item, sData)) == IW_SUCCESS)
			AddField(pField);
		else
			delete pField;
	}

	if (CNISTRecord::IsBinaryFingerType(m_nRecordType))
	{
		// For fingerprints, also update the binary FINGERPRINT_HEADER
		pField = GetNISTField(GetDATField(m_nRecordType));
		if(pField)
		{
			int iVal = (int)_tcstol(sData, NULL, 10);
			FINGERPRINT_HEADER* pHdr = (FINGERPRINT_HEADER*)pField->m_pImageData;
			switch(Field)
			{
				case 1: pHdr->bFieldLen[0] = HIBYTE(HIWORD(iVal));
						pHdr->bFieldLen[1] = LOBYTE(HIWORD(iVal));
						pHdr->bFieldLen[2] = HIBYTE(LOWORD(iVal));
						pHdr->bFieldLen[3] = LOBYTE(LOWORD(iVal));
						break;
				case 2: pHdr->bIDC = iVal; break;		// IDC
				case 3: pHdr->bImp = iVal; break;		// finger impression type
				case 4: if (Subfield >= 1 && Subfield <= 6)
						{
							pHdr->bFGP[Subfield-1] = iVal;	// finger position
						}
						break;
				case 5: pHdr->bISR = iVal; break;		// scanning resolution
				case 6: pHdr->bHLL[1] = LOBYTE(iVal);	// horz line length
						pHdr->bHLL[0] = HIBYTE(iVal);	// horz line length
						break;
				case 7: pHdr->bVLL[1] = LOBYTE(iVal);	// vert line length
						pHdr->bVLL[0] = HIBYTE(iVal);	// vert line length	
						break;
				case 8: pHdr->bComp = iVal;				// compression algorithm used
			}
		}
	}
	else if (m_nRecordType == RECORD_TYPE8)
	{
		// For signatures, also update the binary SIGNATURE_HEADER
		pField = GetNISTField(GetDATField(m_nRecordType));
		if(pField)
		{
			int iVal = (int)_tcstol(sData, NULL, 10);
			SIGNATURE_HEADER* pHdr = (SIGNATURE_HEADER*)pField->m_pImageData;
			switch(Field)
			{
				case 1: pHdr->bFieldLen[0] = HIBYTE(HIWORD(iVal));
						pHdr->bFieldLen[1] = LOBYTE(HIWORD(iVal));
						pHdr->bFieldLen[2] = HIBYTE(LOWORD(iVal));
						pHdr->bFieldLen[3] = LOBYTE(LOWORD(iVal));
						break;
				case 2: pHdr->bIDC = iVal; break;		// IDC
				case 3: pHdr->bSIG = iVal; break;		// signature type
				case 4: pHdr->bSRT = iVal; break;		// signature representation type
				case 5: pHdr->bISR = iVal; break;		// scanning resolution
				case 6: pHdr->bHLL[1] = LOBYTE(iVal);	// horz line length
						pHdr->bHLL[0] = HIBYTE(iVal);	// horz line length
						break;
				case 7: pHdr->bVLL[1] = LOBYTE(iVal);	// vert line length
						pHdr->bVLL[0] = HIBYTE(iVal);	// vert line length	
			}
		}
	}

	return nRet;
}

int CNISTRecord::GetImage(CStdString& sStorageFormat, int *pnLength, const BYTE **ppData)
{
	int nRet = IW_ERR_INVALID_SUBFIELD_NUM;
	CNISTField *pField =  GetNISTField(GetDATField(m_nRecordType));

	sStorageFormat.Empty();
	*pnLength = 0;
	*ppData = 0;

	if (pField == NULL) goto done;

	*pnLength = pField->m_nImageLen;
	sStorageFormat = CNISTField::ImageExtFromImageFormat(pField->m_ImageFormat);		

	if (pField)
	{
		switch(pField->m_nRecordType)
		{
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				// Strip fingerprint header
				*ppData = pField->m_pImageData + sizeof(FINGERPRINT_HEADER);
				nRet = IW_SUCCESS;
				break;

			case 8:
				// Strip signature header
				*ppData = pField->m_pImageData + sizeof(SIGNATURE_HEADER);
				nRet = IW_SUCCESS;
				break;

			default:
				*ppData = pField->m_pImageData;
				nRet = IW_SUCCESS;
				break;
		}
	}

done:
	return nRet;
}

int CNISTRecord::GetImageInfo(CStdString& sStorageFormat, int *pnLength, int *pnHLL, int *pnVLL, int *pnBitsPerPixel)
{
	int nRet = IW_ERR_INVALID_SUBFIELD_NUM;
	CNISTField *pField =  GetNISTField(GetDATField(m_nRecordType));

	if (pField)
	{
		sStorageFormat = CNISTField::ImageExtFromImageFormat(pField->m_ImageFormat);
		*pnLength = pField->m_nImageLen;

		if (m_nRecordType == RECORD_TYPE11)
		{
			// Currently Type-11 Audio is still experimental and doesn't contain any additional fields
			nRet = IW_SUCCESS;
		}
		else
		{
			CStdString sData;

			FindItem(TYPE4_HLL, 1, 1, sData); // note: *_HLL are all the same
			*pnHLL = (int)_tcstol(sData, NULL, 10);
			FindItem(TYPE4_VLL, 1, 1, sData); // note: *_VLL are all the same
			*pnVLL = (int)_tcstol(sData, NULL, 10);

			if (m_nRecordType >= RECORD_TYPE3 && m_nRecordType <= RECORD_TYPE7)
			// These are always all grayscale
			{
				*pnBitsPerPixel = 8;
				nRet = IW_SUCCESS;
			}
			else if (m_nRecordType == 8)
			// signature is always binary
			{
				*pnBitsPerPixel = 1;
				nRet = IW_SUCCESS;
			}
			else if (m_nRecordType == 10)
			// could be color or grayscale
			{
				FindItem(TYPE10_CSP, 1, 1, sData);
				*pnBitsPerPixel = sData.CompareNoCase(_T("GRAY")) ? 24 : 8;
				nRet = IW_SUCCESS;
			}
			else if (m_nRecordType >= RECORD_TYPE13 && m_nRecordType <= RECORD_TYPE17)
			// Field has bits per pixel (*_BPX)
			{
				FindItem(TYPE13_BPX, 1, 1, sData);  // note: *_BPX are all the same
				*pnBitsPerPixel = (int)_tcstol(sData, NULL, 10);
				nRet = IW_SUCCESS;
			}
			else
			{
				// All cases have been dealt with
			}
		}
	}

	return nRet;
}

int CNISTRecord::GetImageResolution(double *pdPPMM)
{
	CNISTField *pField =  GetNISTField(GetDATField(m_nRecordType));

	if (pField)
	{
		CStdString	sData1;
		CStdString	sData2;
		int			nResCode;
		int			nScaleUnits;
		int			nHPixelScale;

		if (m_nRecordType >= RECORD_TYPE3 && m_nRecordType <= RECORD_TYPE8)
		{
			FindItem(TYPE4_ISR, 1, 1, sData1);			// note: *_ISR are all the same
			nResCode = _tcstol(sData1, NULL, 0);

			if (nResCode == 1)
				*pdPPMM = m_dNativeResolutionPPMM;
			else
				*pdPPMM = m_dNativeResolutionPPMM/2;	// minimum is 1/2 of native (?)
		}
		else
		{
			// For all other record types we look at the Scale and Horizontal resolution.
			// Currently we do not support differing horizontal and vertical resolutions
			// but this would be easy to do in the future if needed
			// (note: *_SLC and *_HPS are all the same)
			if ((FindItem(TYPE10_SLC, 1, 1, sData1) == IW_SUCCESS) &&
				(FindItem(TYPE10_HPS, 1, 1, sData2) == IW_SUCCESS))
			{
				nScaleUnits = _tcstol(sData1, NULL, 10);
				nHPixelScale = _tcstol(sData2, NULL, 10);
				if (nScaleUnits == 1) // pixels per inch
				{
					*pdPPMM = nHPixelScale / 25.4;
				}
				else if (nScaleUnits == 2) // pixels per cm
				{
					*pdPPMM = nHPixelScale * 10;
				}
				else
				{
					// We don't support 0 ("no scale units"), let's just assume 72dpi
					*pdPPMM = 72/25.4;
				}
			}
			else
			{
				// The mandatory fields are missing, let's assume a fixed resolution
				*pdPPMM = 72/25.4;
			}
		}
	}

	return IW_SUCCESS;
}

bool CNISTRecord::FormatSupportedInput(CStdString sFormat)
{
	OpenEBTSImageFormat fmt;

	fmt = CNISTField::ImageFormatFromImageExt(sFormat);
	if (fmt == fmtRAW || fmt == fmtBMP || fmt == fmtJPG || fmt == fmtJP2 ||
		fmt == fmtWSQ || fmt == fmtFX4 || fmt == fmtPNG || fmt == fmtCBEFF)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CNISTRecord::FormatSupportedEBTS(CStdString sFormat)
// This differs from the input/output formats: the EBTS file internally can support BMP.
{
	OpenEBTSImageFormat fmt;

	fmt = CNISTField::ImageFormatFromImageExt(sFormat);
	if (fmt == fmtRAW || fmt == fmtJPG || fmt == fmtJP2 || fmt == fmtWSQ ||
		fmt == fmtFX4 || fmt == fmtPNG || fmt == fmtCBEFF)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float CNISTRecord::CompressionToWSQRate(float fCompression)
// Do something similar to EB to convert the 1..100 Compression factor
// to a WSQ compression "rate" parameter. We want 85 to be approximately
// 1:15, which maps to FBI's default rate of 0.75, so we use 11.25/(100-Q), but
// we we make sure the rate never exceed 4 because this gives unpredictable
// result (and crashes).
// (In the end, this implies that Compression values of 100, 99, 98 are equivalent)
{
	int	  QFactor;
	float fRate;

	QFactor = 100 - (int)fCompression;
	QFactor = __min(__max(QFactor, 1), 100);
	fRate =  11.25f/QFactor;
	fRate = __min(fRate, 4.0f);

	return fRate;
}

float CNISTRecord::CompressionToJP2Rate(float fCompression)
// The openEBTS range is 1..100 smallest file to best quality.
// The JP2 compression rate is 1 to 100 rate of compression, e.g., 15 means 1:15.
// So we map 1 to 100, 85 to 16 (the default) and 100 to 1, so 101 - fCompression.
{
	int iCompression;
	float JP2Factor;

	iCompression = (int)fCompression;
	iCompression = __min(__max(iCompression, 1), 100);
	JP2Factor = 101.0f - iCompression;

	return JP2Factor;
}

int CNISTRecord::SetImage(CStdString sInputFormat, int nRecordIndex, int nLength, BYTE *pData,
						  CStdString sStorageFormat, float fCompression)
{
	int					nRet = IW_ERR_IMAGE_CONVERSION;
	CNISTField			*pField =  GetNISTField(GetDATField(m_nRecordType));
	BYTE				*pDataTmp = NULL;
	int					nLengthTmp;
	BYTE				*pDataOut = NULL;
	int					nLengthOut;
	OpenEBTSImageInfo	info;
	int					width;
	int					height;
	int					DPI;
	OpenEBTSImageFormat	fmtIn;
	OpenEBTSImageFormat	fmtOut;

	// Check for invalid inputs
	if (!pData || nLength == 0) return IW_ERR_NULL_POINTER;
	// Check for supported formats
	if (!CNISTRecord::FormatSupportedInput(sInputFormat))  return IW_ERR_UNSUPPORTED_IMAGE_FORMAT;
	if (!CNISTRecord::FormatSupportedEBTS(sStorageFormat)) return IW_ERR_UNSUPPORTED_IMAGE_FORMAT;

	fmtIn = CNISTField::ImageFormatFromImageExt(sInputFormat);
	fmtOut = CNISTField::ImageFormatFromImageExt(sStorageFormat);
	// Check for format congruency on "cbeff": this is not a real image, so it can't be converted
	if ((fmtIn == fmtCBEFF && fmtOut != fmtCBEFF) ||
		(fmtIn != fmtCBEFF && fmtOut == fmtCBEFF))
	{
		return IW_ERR_UNSUPPORTED_IMAGE_FORMAT;
	}

	IWS_BEGIN_EXCEPTION_METHOD("CNISTRecord::SetImage")
	IWS_BEGIN_CATCHEXCEPTION_BLOCK()

	// Add image field if it doesn't exist. Image fields only have one DAT field.
	if (!pField)
	{
		pField = new CNISTField;
		pField->m_nRecordType = m_nRecordType;
		pField->m_nField = GetDATField(m_nRecordType);
		AddField(pField);
	}

	// Ditch existing image data, if any
	if (pField->m_pImageData)
	{
		delete [] pField->m_pImageData;
		pField->m_pImageData = 0;
		pField->m_nImageLen = 0;
	}

	// Convert image if required, after this pDataOut should own the output image
	if (fmtIn != fmtOut)
	{
		// Possible combinations at this point are:
		// BMP -> RAW, FX4, JPG, JP2, PNG or WSQ
		// JPG -> RAW, JP2, PNG or WSQ
		// JP2 -> RAW, JPG, PNG or WSQ
		// PNG -> RAW, JPG, JP2 or WSQ
		// WSQ -> RAW, JPG, JP2 or PNG
		// FX4 -> RAW, PNG

		if (fmtIn == fmtBMP)
		{
			if (fmtOut == fmtRAW)
			{
				if (BMPtoRAW(pData, nLength, &pDataOut, &nLengthOut, &width, &height, &DPI)) goto done;
			}
			else if (fmtOut == fmtJPG)
			{
				if (BMPtoJPG(pData, nLength, (int)fCompression, &pDataOut, &nLengthOut)) goto done;
			}
			else if (fmtOut == fmtJP2)
			{	
				if (BMPtoJP2(pData, nLength, CompressionToJP2Rate(fCompression), &pDataOut, &nLengthOut)) goto done;
			}
			else if (fmtOut == fmtWSQ)
			{
				if (BMPtoWSQ(pData, nLength, CompressionToWSQRate(fCompression), &pDataOut, &nLengthOut)) goto done;
			}
			else if (fmtOut == fmtFX4)
			{
				if (BMPtoFX4(pData, nLength, &pDataOut, &nLengthOut)) goto done;
			}
			else if (fmtOut == fmtPNG)
			{
				if (BMPtoPNG(pData, nLength, &pDataOut, &nLengthOut)) goto done;
			}
			else // (shouldn't happen)
			{
				nRet = IW_ERR_UNSUPPORTED_IMAGE_FORMAT;
				goto done;
			}
		}
		else
		{
			// This will be a 2-step process, format must be converted to BMP first

			if (fmtIn == fmtJPG)
			{
				if (JPGtoBMP(pData, nLength, &pDataTmp, &nLengthTmp)) goto done;
			}
			else if (fmtIn == fmtJP2)
			{
				if (JP2toBMP(pData, nLength, &pDataTmp, &nLengthTmp)) goto done;
			}
			else if (fmtIn == fmtPNG)
			{
				if (PNGtoBMP(pData, nLength, &pDataTmp, &nLengthTmp)) goto done;
			}
			else if (fmtIn == fmtFX4)
			{
				if (FX4toBMP(pData, nLength, &pDataTmp, &nLengthTmp)) goto done;
			}
			else if (fmtIn == fmtWSQ)
			{
				if (WSQtoBMP(pData, nLength, &pDataTmp, &nLengthTmp)) goto done;
			}

			// And now convert from BMP to the final format

			if (fmtOut == fmtRAW)
			{
				if (BMPtoRAW(pDataTmp, nLengthTmp, &pDataOut, &nLengthOut, &width, &height, &DPI)) goto done;
			}
			else if (fmtOut == fmtJPG)
			{
				if (BMPtoJPG(pDataTmp, nLengthTmp, (int)fCompression, &pDataOut, &nLengthOut)) goto done;
			}
			else if (fmtOut == fmtJP2)
			{	
				if (BMPtoJP2(pDataTmp, nLengthTmp, CompressionToJP2Rate(fCompression), &pDataOut, &nLengthOut)) goto done;
			}
			else if (fmtOut == fmtFX4)
			{
				if (BMPtoFX4(pDataTmp, nLengthTmp, &pDataOut, &nLengthOut)) goto done;
			}
			else if (fmtOut == fmtPNG)
			{
				if (BMPtoPNG(pDataTmp, nLengthTmp, &pDataOut, &nLengthOut)) goto done;
			}
			else if (fmtOut == fmtWSQ)
			{
				if (BMPtoWSQ(pDataTmp, nLengthTmp, CompressionToWSQRate(fCompression), &pDataOut, &nLengthOut)) goto done;
			}
			else // (shouldn't happen)
			{
				nRet = IW_ERR_UNSUPPORTED_IMAGE_FORMAT;
				goto done;
			}

			// ditch the temp image, pDataOut now owns the output image
			delete [] pDataTmp;
			pDataTmp = NULL;
		}
	}
	else
	{
		// Image in is the same as image out, or input is "cbeff":
		// just use the image blob directly
		pDataOut = new BYTE[nLength];
		nLengthOut = nLength;
		memcpy(pDataOut, pData, nLength);
	}

	// If it's a "cbeff" or "raw" there's nothing else to do
	if (fmtIn == fmtCBEFF || fmtIn == fmtRAW)
	{
		pField->m_ImageFormat = fmtIn;
		pField->m_pImageData = pDataOut;
		pField->m_nImageLen = nLengthOut;
		pDataOut = NULL; // now belongs to m_pImageData
		nRet = IW_SUCCESS;
		goto done;
	}

	// Set field's image format constant based on extension
	pField->m_ImageFormat = CNISTField::ImageFormatFromImageExt(sStorageFormat);

	// We fetch the pertinent image information that will later be used to populate some
	// of the EBTS record's mandatory fields.
	// If the target format is RAW, we do this on the input image and then mod it slightly, 
	// otherwise directly on the output image.
	if (pField->m_ImageFormat == fmtRAW)
	{
		nRet = GetInfoFromImage(pData, nLength, sInputFormat, &info);
		if (nRet != IW_SUCCESS) goto done;
		// For RAW, we must change a few props
		strcpy(info.szCompression, "NONE");
		strcpy(info.szColorSpace, (info.bitsPerPixel == 24 ? "RGB" : "GRAY"));
		// 2:Gray Scale (Mono) raw image format
		// 4:RGB (color) raw image format
		info.compressionAlgo = GCA_NONE; // no compression
	}
	else
	{
		nRet = GetInfoFromImage(pDataOut, nLengthOut, sStorageFormat, &info);
		if (nRet != IW_SUCCESS) goto done;
	}

	if (!CNISTRecord::IsBinaryType(m_nRecordType))
	{
		// We're done, we just set the image in pField
		pField->m_pImageData = pDataOut;
		pField->m_nImageLen = nLengthOut;
		pDataOut = NULL; // now belongs to m_pImageData
	}
	else
	{
		// We must precede the image with the appropriate header
		// for fingerprint or signature
		if (CNISTRecord::IsBinaryFingerType(m_nRecordType))
		{
			pField->m_pImageData = new BYTE [nLengthOut + sizeof(FINGERPRINT_HEADER)];
			memcpy(pField->m_pImageData + sizeof(FINGERPRINT_HEADER), pDataOut, nLengthOut);
			pField->m_nImageLen = nLengthOut;
			// For binary records we also set the total record length, since we know it
			nLengthOut += sizeof(FINGERPRINT_HEADER);

			m_nRecordLen = nLengthOut;

			// ditch pDataOut, since we reallocated the whole thing
			delete [] pDataOut;
			pDataOut = NULL;
		}
		else if (m_nRecordType == RECORD_TYPE8)
		{
			pField->m_pImageData = new BYTE [nLengthOut + sizeof(SIGNATURE_HEADER)];
			memcpy(pField->m_pImageData + sizeof(SIGNATURE_HEADER), pDataOut, nLengthOut);
			pField->m_nImageLen = nLengthOut;
			// For binary records we also set the total record length, since we know it
			nLengthOut += sizeof(SIGNATURE_HEADER);

			m_nRecordLen = nLengthOut;

			// ditch pDataOut, since we reallocated the whole thing
			delete [] pDataOut;
			pDataOut = NULL;
		}
		else // (shouldn't happen)
		{
			goto done;
		}
	}

	// Finally, set any mandatory image fields we can based on the image data
	SetMandatoryImageFields(pField, info, nRecordIndex, nLengthOut);

	nRet = IW_SUCCESS;

	IWS_END_CATCHEXCEPTION_BLOCK()

done:

	if (pDataTmp != NULL) delete [] pDataTmp;
	if (pDataOut != NULL) delete [] pDataOut;

	return nRet;
}

int CNISTRecord::GetInfoFromImage(BYTE *pImage, int cbLength, CStdString sFmt, OpenEBTSImageInfo *pInfo)
// Currently supports BMP, JPG, JP2, PNG, FX4 and WSQ. Returns IW_ERR_UNSUPPORTED_IMAGE_FORMAT on most errors.
{
	int	 				nRet = IW_ERR_UNSUPPORTED_IMAGE_FORMAT;
	FIBITMAP			*fibmp = NULL;
	FREE_IMAGE_FORMAT	fifmt;
	BYTE 				wsqHdr[2] = { 0xFF, 0xA0 };
	char				szErr[FREEIMAGEERRSIZE] = { '\0' };

	IWS_BEGIN_EXCEPTION_METHOD("CNISTRecord::GetInfoFromImage")
	IWS_BEGIN_CATCHEXCEPTION_BLOCK()

	memset(pInfo, 0, sizeof(OpenEBTSImageInfo));

	sFmt.ToLower();

	// WSQ must be handled separately
	if (sFmt == _T("wsq"))
	{
		// Verify that we really have a WSQ file
		if (memcmp(pImage, wsqHdr, 2)) goto Exit;

		// Use code adapted from wsq_decode_mem to determine WSQ's dimensions
		unsigned short marker;         /* WSQ marker */
		unsigned char *cbufptr;        /* points to current byte in buffer */
		unsigned char *ebufptr;        /* points to end of buffer */

		init_wsq_decoder_resources();

		// Set memory buffer pointers
		cbufptr = pImage;
		ebufptr = pImage + cbLength;

		// Read the SOI marker
		if (getc_marker_wsq(&marker, SOI_WSQ, &cbufptr, ebufptr))
		{
			free_wsq_decoder_resources();
			goto Exit;
		}

		// Read in supporting tables up to the SOF marker
		if (getc_marker_wsq(&marker, TBLS_N_SOF, &cbufptr, ebufptr))
		{
			free_wsq_decoder_resources();
			goto Exit;
		}
		while (marker != SOF_WSQ)
		{
			if (getc_table_wsq(marker, &dtt_table, &dqt_table, dht_table, &cbufptr, ebufptr))
			{
				free_wsq_decoder_resources();
				goto Exit;
			}
			if (getc_marker_wsq(&marker, TBLS_N_SOF, &cbufptr, ebufptr))
			{
				free_wsq_decoder_resources();
			 	goto Exit;
			}
		}

		// Read in the Frame Header
		if (getc_frame_header_wsq(&frm_header_wsq, &cbufptr, ebufptr))
		{
			free_wsq_decoder_resources();
			goto Exit;
		}

		free_wsq_decoder_resources();

		pInfo->width = frm_header_wsq.width;
		pInfo->height = frm_header_wsq.height;
		// Assume 500 DPI for WSQ files for now
		pInfo->scaleUnits = 1; // = pixels/inch
		pInfo->HPS = 500;
		pInfo->VPS = 500;
		strcpy(pInfo->szCompression, "WSQ20");
		strcpy(pInfo->szColorSpace, "GRAY");
		pInfo->bitsPerPixel = 8;
		pInfo->compressionAlgo = GCA_WSQ;
	}
	else
	{
		fifmt = FreeImage_GetImageFormat(pImage, cbLength);
		if (FreeImageError(szErr)) goto Exit;
		fibmp = FreeImage_LoadImageFromMem(pImage, cbLength, fifmt);
		if (FreeImageError(szErr)) goto Exit;

		// Make sure image format is the same as the format specified
		if (sFmt == _T("bmp") && fifmt != FIF_BMP) goto Exit;
		if (sFmt == _T("jpg") && fifmt != FIF_JPEG) goto Exit;
		if (sFmt == _T("jp2") && fifmt != FIF_JP2) goto Exit;
		if (sFmt == _T("png") && fifmt != FIF_PNG) goto Exit;
		if (sFmt == _T("fx4") && fifmt != FIF_TIFF) goto Exit;	// TOD: test with Group 4 Fax format

		// Set common properties
		pInfo->width = FreeImage_GetWidth(fibmp);
		pInfo->height = FreeImage_GetHeight(fibmp);
		pInfo->bitsPerPixel = FreeImage_GetBPP(fibmp);
		pInfo->scaleUnits = 2; // = pixels/centimeter
		pInfo->HPS = (int)((float)FreeImage_GetDotsPerMeterX(fibmp)/100+0.5);
		pInfo->VPS = (int)((float)FreeImage_GetDotsPerMeterY(fibmp)/100+0.5);

		if (fifmt == FIF_BMP)
		{
			strcpy(pInfo->szCompression, "NONE");
			strcpy(pInfo->szColorSpace, (pInfo->bitsPerPixel == 24 ? "RGB" : "GRAY"));
			pInfo->compressionAlgo = GCA_NONE;
		}
		else if (fifmt == FIF_JPEG)
		{
			//TODO? Support "JPEGL" for lossless jpegs - how to tell?
			strcpy(pInfo->szCompression, "JPEGB");
			strcpy(pInfo->szColorSpace, (pInfo->bitsPerPixel == 8 ? "GRAY" : "YCC"));
			pInfo->compressionAlgo = GCA_JPEG;
		}
		else if (fifmt == FIF_JP2)
		{
			strcpy(pInfo->szCompression, "JP2");
			strcpy(pInfo->szColorSpace, (pInfo->bitsPerPixel == 8 ? "GRAY" : "SYCC"));
			pInfo->compressionAlgo = GCA_JPEG2K;
		}
		else if (fifmt == FIF_TIFF)
		{
			// Facsimile ANSI/EIA 538-1988 algorithm
			// Apparently the compression string is not defined by any standard, but the DOD EBTS uses this format.
			// (perhaps this is because 1 bit depth images do not use the compression string but just the binary
			// compression algorithm number)
			strcpy(pInfo->szCompression, "FAX");
			strcpy(pInfo->szColorSpace, "UNK");	// "GRAY" seems wrong
			pInfo->compressionAlgo = 1;	// for binary fields 1 means "compressed"
		}
		else if (fifmt == FIF_PNG)
		{
			strcpy(pInfo->szCompression, "PNG");
			strcpy(pInfo->szColorSpace, (pInfo->bitsPerPixel == 8 ? "GRAY" : "RGB"));
			pInfo->compressionAlgo = GCA_PNG;
		}
		else
		{
			goto Exit;	// unsupported format
		}
	}

	nRet = IW_SUCCESS;

	IWS_END_CATCHEXCEPTION_BLOCK()

Exit:
	if (fibmp != NULL) FreeImage_Unload(fibmp);

	if (szErr[0] != '\0' && IsLogging())
	{
		CStdString sMsg;
		sMsg.Format(_T("[CNISTRecord::GetInfoFromImage] FreeImage_LoadImageFromMem returns: %s"), szErr);
		LogMessage(sMsg);
	}

	return nRet;
}

void CNISTRecord::SetMandatoryImageFields(CNISTField *pNISTField, OpenEBTSImageInfo& info, int recordIndex, int fieldLength)
{
	IWS_BEGIN_EXCEPTION_METHOD("CNISTRecord::SetMandatoryImageFields")
	IWS_BEGIN_CATCHEXCEPTION_BLOCK()

	CStdString sData;

	if (CNISTRecord::IsBinaryFingerType(pNISTField->m_nRecordType))
	{
		// Note we use the Type4_* constants here, because the values are all the
		// same for Type-3 though Type-7

		// Here we should initialize the entire binary header:
		// BYTE bFieldLen[4];
		// BYTE bIDC;
		// BYTE bImp; // finger impression type
		// BYTE bFGP[6]; // finger position
		// BYTE bISR; // scanning resolution
		// BYTE bHLL[2]; // Horiz line length
		// BYTE bVLL[2]; // Vert line length
		// BYTE bComp; // compression algorithm used

		sData.Format(_T("%ld"), fieldLength);
		SetItem(sData, TYPE4_LEN, 1, 1);

		sData.Format(_T("%ld"), recordIndex);
		SetItem(sData, TYPE4_IDC, 1, 1);

		SetItem(_T("0"), TYPE4_IMP, 1, 1); // default impression to 0="Live-scan Plain"

		// Set all 6 fingers to "unknown"
		for (int i=1; i<=6; i++) SetItem(_T("255"), TYPE4_FGP, i, 1);

		// For now if the compression if 500dpi or more, we use 1 (Native Scanning Resolution), 
		// 0 (Minimum Scanning Resolution) otherwise
		sData.Format(_T("%ld"), (info.HPS >= 500 ? 1 : 0));
		SetItem(sData, TYPE4_ISR, 1, 1);

		sData.Format(_T("%ld"), info.width);
		SetItem(sData, TYPE4_HLL, 1, 1);

		sData.Format(_T("%ld"), info.height);
		SetItem(sData, TYPE4_VLL, 1, 1);

		sData.Format(_T("%ld"), info.compressionAlgo);
		SetItem(sData, TYPE4_GCA, 1, 1);
	}
	else if (pNISTField->m_nRecordType == RECORD_TYPE8)
	{
		// Here we should initialize the entire binary header:
		// BYTE bFieldLen[4];
		// BYTE bIDC;
		// BYTE bSIG; // signature type
		// BYTE bSRT; // signature representation type
		// BYTE bISR; // scanning resolution
		// BYTE bHLL[2]; // Horiz line length
		// BYTE bVLL[2]; // Vert line length

		sData.Format(_T("%ld"), fieldLength);
		SetItem(sData, TYPE8_LEN, 1, 1);

		sData.Format(_T("%ld"), recordIndex);
		SetItem(sData, TYPE8_IDC, 1, 1);

		SetItem(_T("0"), TYPE8_SIG, 1, 1); // default signature type, 0="subject's signature" 

		// TYPE8_SRT: "0" if the image is scanned and not compressed, a binary "1" if the
		// image is scanned and compressed, and the binary equivalent of "2" if the image
		// is vector data. We don't support vector data yet, so it's either 0 or 1.
		sData.Format(_T("%ld"), info.compressionAlgo);
		SetItem(sData, TYPE8_SRT, 1, 1);

		// For now if the compression if 500dpi or more, we use 1 (Native Scanning Resolution), 
		// 0 (Minimum Scanning Resolution) otherwise
		sData.Format(_T("%ld"), (info.HPS >= 500 ? 1 : 0));
		SetItem(sData, TYPE8_ISR, 1, 1);

		sData.Format(_T("%ld"), info.width);
		SetItem(sData, TYPE8_HLL, 1, 1);

		sData.Format(_T("%ld"), info.height);
		SetItem(sData, TYPE8_VLL, 1, 1);
	}
	else if (pNISTField->m_nRecordType == RECORD_TYPE10)
	{
		sData.Format(_T("%ld"), info.width);
		SetItem(sData, TYPE10_HLL, 1, 1);

		sData.Format(_T("%ld"), info.height);
		SetItem(sData, TYPE10_VLL, 1, 1);

		// For photographs the most useful default is to use scale units of 0
		// which means the pixel aspect is provided, and obviously we default to
		// square pixels, hence 1:1
		SetItem(_T("0"), TYPE10_SLC, 1, 1);
		SetItem(_T("1"), TYPE10_HPS, 1, 1);
		SetItem(_T("1"), TYPE10_VPS, 1, 1);

		SetItem(CStdString(info.szCompression), TYPE10_CGA, 1, 1);
		SetItem(CStdString(info.szColorSpace), TYPE10_CSP, 1, 1);
	}
	else if (pNISTField->m_nRecordType >= RECORD_TYPE13 && pNISTField->m_nRecordType <= RECORD_TYPE15)
	{
		// Note, we use the Type-14 constants, but they are the same for Type-13 and Type-15

		sData.Format(_T("%ld"), info.width);
		SetItem(sData, TYPE14_HLL, 1, 1);

		sData.Format(_T("%ld"), info.height);
		SetItem(sData, TYPE14_VLL, 1, 1);

		sData.Format(_T("%ld"), info.scaleUnits);
		SetItem(sData, TYPE14_SLC, 1, 1);

		sData.Format(_T("%ld"), info.HPS);
		SetItem(sData, TYPE14_HPS, 1, 1);

		sData.Format(_T("%ld"), info.VPS);
		SetItem(sData, TYPE14_VPS, 1, 1);

		SetItem(CStdString(info.szCompression), TYPE14_CGA, 1, 1);

		sData.Format(_T("%ld"), info.bitsPerPixel);
		SetItem(sData, TYPE14_BPX, 1, 1);
	}
	else if (pNISTField->m_nRecordType == RECORD_TYPE16)
	{
		sData.Format(_T("%ld"), info.width);
		SetItem(sData, TYPE16_HLL, 1, 1);

		sData.Format(_T("%ld"), info.height);
		SetItem(sData, TYPE16_VLL, 1, 1);

		sData.Format(_T("%ld"), info.scaleUnits);
		SetItem(sData, TYPE16_SLC, 1, 1);

		sData.Format(_T("%ld"), info.HPS);
		SetItem(sData, TYPE16_HPS, 1, 1);

		sData.Format(_T("%ld"), info.VPS);
		SetItem(sData, TYPE16_VPS, 1, 1);

		SetItem(info.szCompression, TYPE16_CGA, 1, 1);

		sData.Format(_T("%ld"), info.bitsPerPixel);
		SetItem(sData, TYPE16_BPX, 1, 1);
	}
	else if (pNISTField->m_nRecordType == RECORD_TYPE17)
	{
		sData.Format(_T("%ld"), info.width);
		SetItem(sData, TYPE17_HLL, 1, 1);

		sData.Format(_T("%ld"), info.height);
		SetItem(sData, TYPE17_VLL, 1, 1);

		sData.Format(_T("%ld"), info.scaleUnits);
		SetItem(sData, TYPE17_SLC, 1, 1);

		sData.Format(_T("%ld"), info.HPS);
		SetItem(sData, TYPE17_HPS, 1, 1);

		sData.Format(_T("%ld"), info.VPS);
		SetItem(sData, TYPE17_VPS, 1, 1);

		SetItem(info.szCompression, TYPE17_CGA, 1, 1);

		sData.Format(_T("%ld"), info.bitsPerPixel);
		SetItem(sData, TYPE17_BPX, 1, 1);

		SetItem(CStdString(info.szColorSpace), TYPE17_CSP, 1, 1);
	}

	IWS_END_CATCHEXCEPTION_BLOCK()
}

int CNISTRecord::GetDATField(int nRecordType)
{
	int iRet = -1;

	switch (nRecordType)
	{
		case 3:
		case 4:
		case 5:
		case 6:	 iRet = TYPE4_DAT;  break;
		case 7:  iRet = TYPE7_DAT;  break;
		case 8:  iRet = TYPE8_DAT;  break;
		case 10: iRet = TYPE10_DAT; break;
		case 11: iRet = TYPE11_DAT; break;
		case 13: iRet = TYPE13_DAT; break;
		case 14: iRet = TYPE14_DAT; break;
		case 15: iRet = TYPE15_DAT; break;
		case 16: iRet = TYPE16_DAT; break;
		case 17: iRet = TYPE17_DAT; break;
		case 99: iRet = TYPE99_DAT; break;
	}

	return iRet;
}

bool CNISTRecord::IsDATField(int nRecordType, int nField)
{
	return (nField == GetDATField(nRecordType));
}

bool CNISTRecord::IsBinaryFingerType(int nRecordType)
{
	if (nRecordType >= RECORD_TYPE3 && nRecordType <= RECORD_TYPE7)
		return true;
	else
		return false;	
}

bool CNISTRecord::IsBinaryType(int nRecordType)
// Binary fingerprint types plus signature type
{
	if (CNISTRecord::IsBinaryFingerType(nRecordType) || nRecordType == RECORD_TYPE8)
		return true;
	else
		return false;	
}

void CNISTRecord::AdjustRecordLength()
// Adjust internal record length (T[X]_LEN) and prepare record for serialization.
// Sets m_nRecordLen.
{
	size_t nFields = m_FieldList.size();
	CNISTField *pField;
	int nLen = 0;
	int nImagePos = 0;
	bool bImage = false;

	if (CNISTRecord::IsBinaryType(m_nRecordType))
	{
		// Binary types already have their m_nRecordLen set
		return;
	}

	// get length of all fields except for .01 field
	for (unsigned int i = 0; i < nFields; i++)
	{
		pField = m_FieldList.at(i);

		if (pField && pField->m_nField != 1)
			nLen += pField->GetWriteLen();

		if (IsDATField(pField->m_nRecordType, pField->m_nField))
		{
			bImage = true;
			nImagePos = i; 
		}
	}

	// make sure image is at end of ary
	if (bImage)
	{
		pField = m_FieldList.at(nImagePos);
		m_FieldList.erase(m_FieldList.begin() + nImagePos);
		m_FieldList.push_back(pField);
	}
			
	// now add .01 field
	char szFieldLabel[20];
	char szLen[30];

	CNISTField::GetFieldLabel(szFieldLabel, sizeof(szFieldLabel), m_nRecordType, 1);

	nLen += (int)strlen(szFieldLabel);
	nLen++; // for record seperator

	sprintf(szLen, "%d", nLen); // get the length of the len field

	int nLenDigits = (int)strlen(szLen);
	nLen += nLenDigits;

	// Put real total length in the string
	sprintf(szLen, "%d", nLen);

	// CAUTION! if the length happened to be 98 or 99 WITHOUT the length digits, with it it will be
	// 100 or 101 and hence require one extra byte!
	// Same holds for 998 and 999, etc..
	int nLenDigitsNew = (int)strlen(szLen);
	if (nLenDigits != nLenDigitsNew)
	{
		nLen++;
		sprintf(szLen, "%d", nLen);
	}

	pField = GetNISTField(REC_TAG_LEN);

	if (pField)
	{
		pField->SetSubField(1, 1, szLen);
	}

	if (IsLoggingVerbose())
	{
		CStdString sLogMessage;
		sLogMessage.Format(IDS_LOGRECGETRECLEN, m_nRecordType, nLen);
		LogMessageVerbose(sLogMessage);
	}

	m_nRecordLen = nLen;
}

int CNISTRecord::GetLength()
{
	return m_nRecordLen;
}

int CNISTRecord::WriteBinary(BYTE *pBuffer, int *poffset)
{
	if (CNISTRecord::IsBinaryType(m_nRecordType))
	{
		CNISTField* pField = GetNISTField(GetDATField(m_nRecordType));
		if(pField)
		{
			memcpy(pBuffer + *poffset, pField->m_pImageData, m_nRecordLen);
			*poffset = *poffset + m_nRecordLen;
		}
	}

	return IW_SUCCESS;
}

int CNISTRecord::Write(BYTE *pBuffer, int *poffset)
{
	size_t nFields = m_FieldList.size();
	CNISTField *pField;
	int nRet = IW_SUCCESS;
	int offsetStart;

	for (unsigned int i = 0; i < nFields && nRet == IW_SUCCESS; i++)
	{
		pField = m_FieldList.at(i);

		if (pField)
		{
			offsetStart = *poffset;

			nRet = pField->Write(pBuffer, poffset, i == nFields - 1);

			if (IsLoggingVerbose())
			{
				CStdString sLogMessage;
				sLogMessage.Format(_T("CNISTRecord::Write (mem): Wrote %d bytes for field %d"), *poffset -  offsetStart, i);
				LogMessageVerbose(sLogMessage);
			}
		}
	}

	return nRet;
}
