//
// OpenM1Sample3.cpp
//
// Read in a file based BFID Package using an Standard configuration file
//

#include "stdafx.h"
#include "OpenM1.h"


bool WriteBLOBToFile(TCHAR *szFilename, BYTE* pBlob, int iBlobLen);
bool ReportOpenM1Error(int errCode, TCHAR* szContext);


int _tmain(int argc, _TCHAR* argv[])
{
	int			ret;
	TCHAR		szError[1024];
	BYTE		*pImage = NULL;
	int			iImageLen;
	M1Standard	*pStandard = NULL;
	M1Section	*pPackage = NULL;
	TCHAR		*szErr = NULL;
	M1Section	*pFacialRecordHeader = NULL;
	M1Section	*pFacialRecordData = NULL;
	TCHAR		*szFormatIdentifier;
	int			iNumberFacialImages;

	ret = M1ReadStandard(_T("..\\..\\Standards\\ISO 19794-5.xml"), &pStandard, 1024, szError);
	if (ReportOpenM1Error(ret, _T("M1ReadStandard")))
	{
		if (_tcslen(szError) != 0) _tprintf(szError);
		goto done;
	}

	ret = M1ReadPackage(_T("FacePackageISO.bin"), &pPackage, pStandard, &szErr);
	if (ReportOpenM1Error(ret, _T("M1ReadPackage")))
	{
		_tprintf(_T("M1ReadPackage failed.\n"));
		if (szErr != NULL && _tcslen(szErr) != 0)
		{
			_tprintf(_T("Error: %s\n"), szErr);
		}
		goto done;
	}

	// Navigate to Facial Record Header and get a string and a number, as a test
	ret = M1ReturnSection(pPackage, _T("Facial Record Header"), 1, &pFacialRecordHeader);
	if (ret == M1_SECTIONNOTEXIST)
	{
		_tprintf(_T("Section \"Facial Record Header\" does not exist.\n"));
		goto done;
	}

	// Test getting a string via M1GetItemString
	ret = M1GetItemString(pFacialRecordHeader, _T("Format Identifier"), 1, &szFormatIdentifier);
	if (ret == M1_ITEMNOTEXIST)
	{
		_tprintf(_T("Item \"Format Identifier\" does not exist.\n"));
		goto done;
	}
	else
	{
		_tprintf(_T("Item \"Format Identifier\" has value \"%s\".\n"), szFormatIdentifier);
	}

	// Test getting a number via M1GetItemNumber
	ret = M1GetItemNumber(pFacialRecordHeader, _T("Number of Facial Images"), 1, &iNumberFacialImages);
	if (ret == M1_ITEMNOTEXIST)
	{
		_tprintf(_T("Item \"Number of Facial Images\" does not exist.\n"));
		goto done;
	}
	else
	{
		_tprintf(_T("Item \"Number of Facial Images\" has value \"%d\".\n"), iNumberFacialImages);
	}

	// Navigate to image binary and save it to file
	ret = M1ReturnSection(pPackage, _T("Facial Record Data"), 1, &pFacialRecordData);
	if (ret == M1_SECTIONNOTEXIST)
	{
		_tprintf(_T("Section \"Facial Record Data\" does not exist.\n"));
		goto done;
	}

	ret = M1GetItemImage(pFacialRecordData, _T("Image Data"), 1, &pImage, &iImageLen);
	if (ReportOpenM1Error(ret, _T("M1GetItemImage"))) goto done;

	if (!WriteBLOBToFile(_T("sample.jpg"), pImage, iImageLen))
	{
		_tprintf(_T("WriteBLOBToFile failed\n"));
		goto done;
	}

	//
	// This code is useful for verifying the read-in structure
	//
	//
	//TCHAR		*szLog;
	//
	//ret = M1ValidatePackage(pPackage, pStandard, &szLog, &szErr);
	//if (ReportOpenM1Error(ret, _T("M1ValidatePackage")))
	//{
	//	if (_tcslen(szError) != 0) _tprintf(szError);
	//	if (_tcslen(szLog) != 0) _tprintf(szLog);
	//	goto done;
	//}

	//ret = M1WritePackage(pPackage, _T("FacePackage2.bin"));
	//if (ReportOpenM1Error(ret, _T("M1WritePackage")))
	//{
	//	if (_tcslen(szError) != 0) _tprintf(szError);
	//	goto done;
	//}

done:
	if (pStandard != NULL) M1FreeStandard(pStandard);
	if (pPackage != NULL) M1FreePackage(pPackage);
	if (pImage != NULL) M1Free(pImage);

	return 0;
}

bool WriteBLOBToFile(TCHAR *szFilename, BYTE* pBlob, int iBlobLen)
{
	FILE	*f;
	long	cbWritten;

	_tfopen_s(&f, szFilename, _T("wb"));

	if (f != NULL)
	{
		cbWritten = fwrite(pBlob, 1, iBlobLen, f);
		if (cbWritten != iBlobLen)
		{
			_tprintf(_T("Failed to write file \"%s\"\n"), szFilename);
			return false;
		}

		fclose(f);
		return true;
	}
	else
	{
		_tprintf(_T("Failed to open file \"%s\"\n"), szFilename);
		return false;
	}
}

bool ReportOpenM1Error(int errCode, TCHAR* szContext)
{
	TCHAR szErrMsg[1024];

	if (errCode == M1_OK || errCode == M1_SECTIONNOTEXIST) return false;	// no error

	_sntprintf_s(szErrMsg, 1023, 1024, _T("OpenM1 error %d in %s\n"), errCode, szContext);
	_tprintf(szErrMsg);

	return true;
}
