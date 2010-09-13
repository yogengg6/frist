//
// OpenM1Sample2.cpp
//
// Create a BFID Package based on an XML Standard configuration file
// In this example we build a ISO 19794-5 Face package.
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
	M1Section	*pFaceInfoBlock;
	M1Section	*pFeatureMask;
	M1Section	*pFacialRecordData;
	M1Section	*pImageInfo;
	M1Section	*pFeaturePoints;
	TCHAR		*szLog;
	TCHAR		*szErr;

	ret = M1ReadStandard(_T("..\\..\\Standards\\ISO 19794-5.xml"), &pStandard, 1024, szError);
	if (ReportOpenM1Error(ret, _T("M1ReadStandard")))
	{
		if (_tcslen(szError) != 0) _tprintf(szError);
		goto done;
	}

	ret = M1CreatePackage(_T("ISO 19794-5"), &pPackage);
	if (ReportOpenM1Error(ret, _T("M1CreatePackage"))) goto done;

	ret = M1ReturnSection(pPackage, _T("Facial Record Data"), 1, &pFacialRecordData);
	if (ReportOpenM1Error(ret, _T("M1ReturnSection"))) goto done;

	// Facial Record Data
	{
		ret = M1ReturnSection(pFacialRecordData, _T("Facial Information Block"), 1, &pFaceInfoBlock);
		if (ReportOpenM1Error(ret, _T("M1ReturnSection"))) goto done;

		// Facial Information Block
		{
			ret = M1SetItemNumber(pFaceInfoBlock, _T("Gender"), 1, 1);
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

			ret = M1SetItemNumber(pFaceInfoBlock, _T("Eye Color"), 1, 4);
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

			ret = M1SetItemNumber(pFaceInfoBlock, _T("Hair Color"), 1, 4);
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

			ret = M1SetItemNumber(pFaceInfoBlock, _T("Expression"), 1, 1);
			if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

			ret = M1ReturnSection(pFaceInfoBlock, _T("Feature Mask"), 1, &pFeatureMask);
			if (ReportOpenM1Error(ret, _T("M1ReturnSection"))) goto done;

			// Feature Mask
			{
				ret = M1SetItemBit(pFeatureMask, _T("Features Specified"), 1, true);
				if (ReportOpenM1Error(ret, _T("M1SetItemBit"))) goto done;

				ret = M1SetItemBit(pFeatureMask, _T("Glasses"), 1, true);
				if (ReportOpenM1Error(ret, _T("M1SetItemBit"))) goto done;

				ret = M1SetItemBit(pFeatureMask, _T("Moustache"), 1, true);
				if (ReportOpenM1Error(ret, _T("M1SetItemBit"))) goto done;

			}

			ret = M1ReturnSection(pFacialRecordData, _T("Feature Points"), 1, &pFeaturePoints);
			if (ReportOpenM1Error(ret, _T("M1ReturnSection"))) goto done;

			// Feature Points [1]
			{
				ret = M1SetItemNumber(pFeaturePoints, _T("Feature Point"), 1, 193);	// right eye
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

				ret = M1SetItemNumber(pFeaturePoints, _T("Horizontal Position"), 1, 242);
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

				ret = M1SetItemNumber(pFeaturePoints, _T("Vertical Position"), 1, 270);
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;
			}

			ret = M1ReturnSection(pFacialRecordData, _T("Feature Points"), 2, &pFeaturePoints);
			if (ReportOpenM1Error(ret, _T("M1ReturnSection"))) goto done;

			// Feature Points [2]
			{
				ret = M1SetItemNumber(pFeaturePoints, _T("Feature Point"), 1, 194);	// left eye
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;
				
				ret = M1SetItemNumber(pFeaturePoints, _T("Horizontal Position"), 1, 140);
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

				ret = M1SetItemNumber(pFeaturePoints, _T("Vertical Position"), 1, 262);
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;
			}

			ret = M1ReturnSection(pFacialRecordData, _T("Image Information"), 1, &pImageInfo);
			if (ReportOpenM1Error(ret, _T("M1ReturnSection"))) goto done;

			// Image Information
			{
				ret = M1SetItemNumber(pImageInfo, _T("Face Image Type"), 1, 1);
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

				ret = M1SetItemNumber(pImageInfo, _T("Image Color Space"), 1, 1);
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

				ret = M1SetItemNumber(pImageInfo, _T("Source Type"), 1, 2);
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

				ret = M1SetItemNumber(pImageInfo, _T("Width"), 1, 360);
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

				ret = M1SetItemNumber(pImageInfo, _T("Height"), 1, 540);
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;

				ret = M1SetItemNumber(pImageInfo, _T("Image Data Type"), 1, 0);
				if (ReportOpenM1Error(ret, _T("M1SetItemNumber"))) goto done;
			}

			// Image
			pImage = ReadBLOBFromFile(_T("sample.jpg"), &iImageLen);
			if (pImage == NULL) goto done;

			ret = M1SetItemImage(pFacialRecordData, _T("Image Data"), 1, pImage, iImageLen);
			if (ReportOpenM1Error(ret, _T("M1SetItemImage"))) goto done;

			// M1SetItemImage makes a copy of the image
			delete pImage;
			pImage = NULL;
		}
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
		ret = M1WritePackage(pPackage, _T("FacePackageISO.bin"));
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
