//
// OpenM1Sample4.cpp
//
// Create a BFID Package based on an XML Standard configuration file
// In this example we build a ISO 19794-4 Fingerprint Image package.
//

#include "stdafx.h"
#include "OpenM1.h"


bool ReportOpenM1Error(int errCode, TCHAR* szContext);
BYTE* ReadBLOBFromFile(TCHAR *szFilename, int *piFileSize);


int _tmain(int argc, _TCHAR* argv[])
{
	int			ret;
	TCHAR		szError[1024];
	BYTE		*pImage = NULL;
	int			iImageLen;
	M1Standard	*pStandard = NULL;
	M1Section	*pPackage = NULL;
	M1Section	*pGeneralRecordHeader;
	M1Section	*pFingerImageRecord;
	M1Section	*pFingerImageHeader;
	TCHAR		*szLog;
	TCHAR		*szErr;

	ret = M1ReadStandard(_T("..\\..\\Standards\\ISO 19794-4.xml"), &pStandard, 1024, szError);
	if (ReportOpenM1Error(ret, _T("M1ReadStandard")))
	{
		if (_tcslen(szError) != 0) _tprintf(szError);
		goto done;
	}

	ret = M1CreatePackage(_T("ISO 19794-4"), &pPackage);
	if (ReportOpenM1Error(ret, _T("M1CreatePackage"))) goto done;

	ret = M1ReturnSection(pPackage, _T("General Record Header"), 1, &pGeneralRecordHeader);
	if (ReportOpenM1Error(ret, _T("M1ReturnSection"))) goto done;

	// General Record Header
	{
		ret = M1SetItemNumber(pGeneralRecordHeader, _T("Vendor ID"), 1, 42);	// some bogus test value
		if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

		ret = M1SetItemNumber(pGeneralRecordHeader, _T("Scanner ID"), 1, 43);	// some bogus test value
		if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

		ret = M1SetItemNumber(pGeneralRecordHeader, _T("Software ID"), 1, 40);	// some bogus test value
		if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

		ret = M1SetItemNumber(pGeneralRecordHeader, _T("Scale Units"), 1, 1);	// 1 = dpi, 2 = pixels/cm
		if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

		ret = M1SetItemNumber(pGeneralRecordHeader, _T("Horizontal Scan Resolution"), 1, 500); // 500 dpi
		if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

		ret = M1SetItemNumber(pGeneralRecordHeader, _T("Vertical Scan Resolution"), 1, 500); // 500 dpi
		if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

		ret = M1SetItemNumber(pGeneralRecordHeader, _T("Horizontal Image Resolution"), 1, 500); // 500 dpi
		if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

		ret = M1SetItemNumber(pGeneralRecordHeader, _T("Vertical Image Resolution"), 1, 500); // 500 dpi
		if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

		ret = M1SetItemNumber(pGeneralRecordHeader, _T("Pixel Depth"), 1, 8); // 8 bpp
		if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

		ret = M1SetItemNumber(pGeneralRecordHeader, _T("Image Compression Algorithm"), 1, 3); // 3 = JPEG
		if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;
	}

	ret = M1ReturnSection(pPackage, _T("Finger Image Record"), 1, &pFingerImageRecord);
	if (ReportOpenM1Error(ret, _T("M1ReturnSection"))) goto done;

	// Finger Image Record
	{
		ret = M1ReturnSection(pFingerImageRecord, _T("Finger Image Header"), 1, &pFingerImageHeader);
		if (ReportOpenM1Error(ret, _T("M1ReturnSection"))) goto done;

		// Finger Image Header
		{
			ret = M1SetItemNumber(pFingerImageHeader, _T("Finger Position"), 1, 2); // 2 = Right index finger
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

			ret = M1SetItemNumber(pFingerImageHeader, _T("Count of Views"), 1, 1);
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

			ret = M1SetItemNumber(pFingerImageHeader, _T("View Number"), 1, 1);
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

			ret = M1SetItemNumber(pFingerImageHeader, _T("Finger Image Quality"), 1, 95); // 95%
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

			ret = M1SetItemNumber(pFingerImageHeader, _T("Impression Type"), 1, 0); // 0 = live-scan plain
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

			ret = M1SetItemNumber(pFingerImageHeader, _T("Horizontal Line Length"), 1, 500); // 500 pixels wide
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

			ret = M1SetItemNumber(pFingerImageHeader, _T("Vertical Line Length"), 1, 500); // 500 pixels tall
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;
		}

		// Image
		pImage = ReadBLOBFromFile(_T("SampleFingerprint.jpg"), &iImageLen);
		if (pImage == NULL) goto done;

		ret = M1SetItemImage(pFingerImageRecord, _T("Finger Image Data"), 1, pImage, iImageLen);
		if (ReportOpenM1Error(ret, _T("M1SetItemImage"))) goto done;

		// M1SetItemImage makes a copy of the image
		delete pImage;
		pImage = NULL;
	}

	ret = M1ValidatePackage(pPackage, pStandard, &szLog, &szErr);
	if (ReportOpenM1Error(ret, _T("M1ValidatePackage")))
	{
		_tprintf(_T("Validation failed: "));
		_tprintf(szErr);
		_tprintf(_T("\n"));
	}
	else
	{
		_tprintf(_T("Validation succeeded\n"));
	}

	_tprintf(_T("\n----------------------------------------\n"));
	_tprintf(szLog);
	_tprintf(_T("\n----------------------------------------\n"));

	if (ret == M1_OK)
	{
		ret = M1WritePackage(pPackage, _T("FingerprintImagePackageISO.bin"));
		if (ReportOpenM1Error(ret, _T("M1WritePackage")))
		{
			_tprintf(_T("M1WritePackage failed."));
		}
	}
	

done:
	if (pStandard != NULL) M1FreeStandard(pStandard);
	if (pPackage != NULL) M1FreePackage(pPackage);
	if (pImage != NULL) delete pImage;

	return 0;
}

BYTE* ReadBLOBFromFile(TCHAR *szFilename, int *piFileSize)
{
	FILE	*f;
	long	cbFile;
	long	cbRead;
	BYTE	*pBuf = NULL;

	_tfopen_s(&f, szFilename, _T("rb"));

	if (f != NULL)
	{
		fseek(f, 0, SEEK_END);
		cbFile = ftell(f);
		fseek(f, 0, SEEK_SET);

		pBuf = new BYTE [cbFile];

		cbRead = fread(pBuf, 1, cbFile, f);
		if (cbRead != cbFile)
		{
			delete [] pBuf;
			pBuf = NULL;
			_tprintf(_T("Failed to read file \"%s\"\n"), szFilename);
		}
		else
		{
			if (piFileSize != NULL) *piFileSize = cbFile;
		}

		fclose(f);
	}
	else
	{
		_tprintf(_T("Failed to open file \"%s\"\n"), szFilename);
	}

	return pBuf;
}

bool ReportOpenM1Error(int errCode, TCHAR* szContext)
{
	TCHAR szErrMsg[1024];

	if (errCode == M1_OK || errCode == M1_SECTIONNOTEXIST) return false;	// no error

	_sntprintf_s(szErrMsg, 1023, 1024, _T("OpenM1 error %d in %s\n"), errCode, szContext);
	_tprintf(szErrMsg);

	return true;
}
