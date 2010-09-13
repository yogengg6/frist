//
// OpenM1Sample1.cpp
//
// Read-in several XML Standard configuration files and output their
// structures in an easily readbale form.
//

#include "stdafx.h"
#include "OpenM1.h"


void TraverseContainerDef(M1ContainerDef *pContainer, int depth);
bool ReportOpenM1Error(int errCode, TCHAR* szContext);
void OutputStandard(TCHAR* szStandard);


int _tmain(int argc, _TCHAR* argv[])
{
	OutputStandard(_T("..\\..\\Standards\\ISO 19794-4.xml"));
	OutputStandard(_T("..\\..\\Standards\\INCITS 381-2004.xml"));
	OutputStandard(_T("..\\..\\Standards\\ISO 19794-5.xml"));
	OutputStandard(_T("..\\..\\Standards\\INCITS 385-2004.xml"));

	return 0;
}

void OutputStandard(TCHAR* szStandard)
{
	int				ret;
	TCHAR			szError[1024];
	M1Standard		*pStandard = NULL;
	M1ContainerDef	*pContainer = NULL;

	_tprintf(_T("Standard \"%s\":\n"), szStandard);

	ret = M1ReadStandard(szStandard, &pStandard, 1024, szError);
	if (ReportOpenM1Error(ret, _T("M1ReadStandard")))
	{
		if (_tcslen(szError) != 0) _tprintf(szError);
		goto done;
	}

	ret = M1GetHeadContainer(pStandard, &pContainer);
	if (ReportOpenM1Error(ret, _T("M1GetHeadContainer"))) goto done;

	TraverseContainerDef(pContainer, 0);

done:
	if (pStandard != NULL) M1FreeStandard(pStandard);

	_tprintf(_T("\n"));
}

void TraverseContainerDef(M1ContainerDef *pContainer, int depth)
{
	int				ret;
	TCHAR			*szName;
	TCHAR			*szDesc;
	bool			bIsItemDef;
	M1ContainerDef	*pContainerNext;
	M1ItemFormat	itemFormat;
	TCHAR*			szMin;
	TCHAR*			szMax;
	TCHAR*			szDefault;
	TCHAR*			szMinOcc;
	TCHAR*			szMaxOcc;
	bool			bReserved;

	// Get some general info about the container
	ret = M1GetContainerInfo(pContainer, &bIsItemDef, &szName, &szDesc, &szMinOcc, &szMaxOcc, &bReserved);
	if (ReportOpenM1Error(ret, _T("M1GetContainerInfo"))) goto done;

	// Prefix output with spacer to reflect depth of tree
	for (int i = 0; i < 4*depth; i++) _tprintf(_T("."));	

	// pContainer can be either an M1ItemDef or an M1SectionDef
	if (bIsItemDef)
	{
		// It's just an M1ItemDef, we output some info about it
		_tprintf(_T("ItemDef \"%s\":"), szName);

		// Get some additional ItemDef specific information
		ret = M1GetItemDefInfo(pContainer, &itemFormat, &szMin, &szMax, &szDefault);
		if (ReportOpenM1Error(ret, _T("M1GetItemDefInfo"))) goto done;

		_tprintf(_T(" fmt(%d)"), itemFormat);
		if (_tcslen(szMin) != 0)
		{
			_tprintf(_T(" min(%s)"), szMin);
		}
		if (_tcslen(szMax) != 0)
		{
			_tprintf(_T(" max(%s)"), szMax);
		}
		if (_tcslen(szDefault) != 0)
		{
			_tprintf(_T(" def(%s)"), szDefault);
		}
		if (_tcslen(szMinOcc) != 1)
		{
			_tprintf(_T(" minocc(%s)"), szMinOcc);
		}
		if (_tcslen(szMaxOcc) != 1)
		{
			_tprintf(_T(" maxocc(%s)"), szMaxOcc);
		}
		if (bReserved)
		{
			_tprintf(_T(" R"));
		}
		_tprintf(_T("\n"));
	}
	else
	{
		// It's an M1SectionDef, we recurse back into this function
		_tprintf(_T("SectionDef \"%s\":"), szName);

		if (_tcslen(szMinOcc) != 1)
		{
			_tprintf(_T(" minocc(%s)"), szMinOcc);
		}
		if (_tcslen(szMaxOcc) != 1)
		{
			_tprintf(_T(" maxocc(%s)"), szMaxOcc);
		}
		if (bReserved)
		{
			_tprintf(_T(" R"));
		}
		_tprintf(_T("\n"));

		M1EnumContainerReset(pContainer);

		while (M1EnumContainerGetNext(pContainer, &pContainerNext) == M1_OK)
		{
			TraverseContainerDef(pContainerNext, depth+1);
		}
	}
done:
	;
}

bool ReportOpenM1Error(int errCode, TCHAR* szContext)
{
	TCHAR szErrMsg[1024];

	if (errCode == M1_OK || errCode == M1_SECTIONNOTEXIST) return false;	// no error

	_sntprintf_s(szErrMsg, 1023, 1024, _T("OpenM1 error %d in %s\n"), errCode, szContext);
	_tprintf(szErrMsg);

	return true;
}
