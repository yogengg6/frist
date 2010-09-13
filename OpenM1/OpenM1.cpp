// OpenM1.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"



//
// Functions relating to the XML configuration of the Standard 
//

OPENM1_API int __stdcall M1ReadStandard(const TCHAR* szPath,
									    M1Standard** ppStandard,
										int nMaxError,
										TCHAR* szError)
{
	M1_EXCEPTION_TRY

	CStdString	sError;
	M1Standard	*pStandard = NULL;

	if (!szPath) return M1_INVALIDPARAM;

	if (nMaxError > 0)
	{
		szError[0] = '\0';
	}

	pStandard = new M1Standard;
	if (pStandard == NULL) return M1_MEMORYFAULT;

	if (pStandard->LoadFromFile(szPath, sError))
	{
		*ppStandard = pStandard;
		return M1_OK;
	}
	else
	{
		delete pStandard;
		if (!sError.IsEmpty())
		{
			_tcsncpy(szError, sError, nMaxError-1);
			szError[nMaxError-1] = '\0';
		}

		return M1_FAIL;
	}

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1FreeStandard(M1Standard* pStandard)
{
	M1_EXCEPTION_TRY

	if (pStandard == NULL) return M1_INVALIDPARAM;

	delete pStandard;

	return M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1GetHeadContainer(M1Standard* pStandard,
											M1ContainerDef** ppContainer)
{
	M1_EXCEPTION_TRY

	if (pStandard == NULL) return M1_INVALIDPARAM;
	if (ppContainer == NULL) return M1_INVALIDPARAM;

	// M1Standard is derived from CSection, so we just return a re-casted pointer to pStandard
	*ppContainer = pStandard;

	return M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1GetContainerInfo(M1ContainerDef* pContainer,
											bool*		 pbIsItemDef,
											TCHAR**		 pszName,
											TCHAR**		 pszDescription,
											TCHAR**		 pszMinOccurrences,
											TCHAR**		 pszMaxOccurrences,
											bool*		 pbIsReserved)
{
	M1_EXCEPTION_TRY

	if (pContainer == NULL) return M1_INVALIDPARAM;
	if (pbIsItemDef == NULL) return M1_INVALIDPARAM;
	if (pszName == NULL) return M1_INVALIDPARAM;
	if (pszDescription == NULL) return M1_INVALIDPARAM;
	if (pszMinOccurrences == NULL) return M1_INVALIDPARAM;
	if (pszMaxOccurrences == NULL) return M1_INVALIDPARAM;
	if (pbIsReserved == NULL) return M1_INVALIDPARAM;

	*pbIsItemDef = pContainer->GetIsItemDef();
	*pszName = pContainer->GetStandard()->CreateNewStringSlot(pContainer->GetName());
	*pszDescription = pContainer->GetStandard()->CreateNewStringSlot(pContainer->GetDescription());
	*pszMinOccurrences = pContainer->GetStandard()->CreateNewStringSlot(pContainer->GetMinOccurrences());
	*pszMaxOccurrences = pContainer->GetStandard()->CreateNewStringSlot(pContainer->GetMaxOccurrences());
	*pbIsReserved = pContainer->GetReserved();

	return M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1EnumContainerReset(M1ContainerDef* pContainer)
{
	M1_EXCEPTION_TRY

	if (pContainer == NULL) return M1_INVALIDPARAM;
	// Ensure it's not an ItemDef
	if (pContainer->GetIsItemDef()) return M1_INVALIDPARAM;

	((M1SectionDef*)pContainer)->ResetContainerDefList();

	return M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1EnumContainerGetNext(M1ContainerDef* pContainer,
												M1ContainerDef** ppContainerNext)
{
	M1_EXCEPTION_TRY

	if (pContainer == NULL) return M1_INVALIDPARAM;
	// Ensure input is not an ItemDef
	if (pContainer->GetIsItemDef()) return M1_INVALIDPARAM;
	if (ppContainerNext == NULL) return M1_INVALIDPARAM;

	((M1SectionDef*)pContainer)->GetNextContainerDef(ppContainerNext);

	if (*ppContainerNext == NULL)
	{
		return M1_FAIL;
	}
	else
	{
		return M1_OK;
	}

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1GetItemDefInfo(M1ContainerDef*	pContainer,
										  M1ItemFormat*	pItemFormat,
										  TCHAR**		pszMinValue,
										  TCHAR**		pszMaxValue,
										  TCHAR**		pszDefaultValue)
{
	M1_EXCEPTION_TRY

	if (pContainer == NULL) return M1_INVALIDPARAM;
	// Ensure input IS an ItemDef
	if (!pContainer->GetIsItemDef()) return M1_INVALIDPARAM;
	if (pItemFormat == NULL) return M1_INVALIDPARAM;
	if (pszMinValue == NULL) return M1_INVALIDPARAM;
	if (pszMaxValue == NULL) return M1_INVALIDPARAM;
	if (pszDefaultValue == NULL) return M1_INVALIDPARAM;

	M1ItemDef *pItemDef = (M1ItemDef*)pContainer;

	*pItemFormat = pItemDef->GetFormat();
	*pszMinValue = pContainer->GetStandard()->CreateNewStringSlot(pItemDef->GetMinValue());
	*pszMaxValue = pContainer->GetStandard()->CreateNewStringSlot(pItemDef->GetMaxValue());
	*pszDefaultValue = pContainer->GetStandard()->CreateNewStringSlot(pItemDef->GetDefaultValue());

	return M1_OK;

	M1_EXCEPTION_CATCH
}


//
// Functions relating to the creating or modification of an M1Package
//

OPENM1_API int __stdcall M1CreatePackage(const TCHAR* szPackageName, M1Section** ppHeadSection)
// Allocate and return a new M1Section object, associated with the M1Standard
{
	M1_EXCEPTION_TRY

	if (ppHeadSection == NULL) return M1_INVALIDPARAM;

	*ppHeadSection = new M1Section(szPackageName, 1, NULL);

	return M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1FreePackage(M1Section* pHeadSection)
// Free the M1Section object allocated by M1CreatePackage
{
	M1_EXCEPTION_TRY

	if (pHeadSection == NULL) return M1_INVALIDPARAM;

	try
	{
		delete pHeadSection;
	}
	catch (...)
	{
		return M1_MEMORYFAULT;
	}

	return M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1ReturnSection(M1Section*   pParentSection,
										 const TCHAR* szSectionName,
										 int		  iSection,
										 M1Section**  ppSection)
// Return the M1Section object defined by the section's name and the
// 1-based index of the section. If the section does not exist it will
// first be added to the parent section. Returns M1_OK if the section
// already existed, otherwise returns M1_SECTIONOTEXIST.
{
	M1_EXCEPTION_TRY

	bool bNewSectionAdded = false;

	if (pParentSection == NULL) return M1_INVALIDPARAM;
	if (ppSection == NULL) return M1_INVALIDPARAM;
	if (szSectionName == NULL) return M1_INVALIDPARAM;

	*ppSection = pParentSection->Section(szSectionName, iSection, &bNewSectionAdded);

	return bNewSectionAdded ? M1_SECTIONNOTEXIST : M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1SetItemString(M1Section*	  pSection,
										 const TCHAR* szItemName,
										 int		  iItem,
										 const TCHAR* szValue)
{
	M1_EXCEPTION_TRY

	if (pSection == NULL) return M1_INVALIDPARAM;
	if (szItemName == NULL) return M1_INVALIDPARAM;
	if (szValue == NULL) return M1_INVALIDPARAM;

	pSection->SetItemString(szItemName, iItem, szValue);

	return M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1SetItemNumber(M1Section*	  pSection,
										 const TCHAR* szItemName,
										 int		  iItem,
										 int		  iValue)
{
	M1_EXCEPTION_TRY

	if (pSection == NULL) return M1_INVALIDPARAM;
	if (szItemName == NULL) return M1_INVALIDPARAM;

	pSection->SetItemNumber(szItemName, iItem, iValue);

	return M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1SetItemBit(M1Section*	pSection,
									  const TCHAR*	szItemName,
									  int			iItem,
									  bool			bValue)
{
	M1_EXCEPTION_TRY

	if (pSection == NULL) return M1_INVALIDPARAM;
	if (szItemName == NULL) return M1_INVALIDPARAM;

	pSection->SetItemBit(szItemName, iItem, bValue);

	return M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1SetItemImage(M1Section*		pSection,
										const TCHAR*	szItemName,
										int				iItem,
										BYTE*			pImage,
										int				iImageLen)
{
	M1_EXCEPTION_TRY

	if (pSection == NULL) return M1_INVALIDPARAM;
	if (szItemName == NULL) return M1_INVALIDPARAM;
	if (pImage == NULL) return M1_INVALIDPARAM;

	pSection->SetItemImage(szItemName, iItem, pImage, iImageLen);

	return M1_OK;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1GetItemString(M1Section*		pSection,
										 const TCHAR*	szItemName,
										 int			iItem,
										 TCHAR**		pszValue)
{
	M1_EXCEPTION_TRY

	if (pSection == NULL) return M1_INVALIDPARAM;
	if (szItemName == NULL) return M1_INVALIDPARAM;
	if (pszValue == NULL) return M1_INVALIDPARAM;

	CStdString sValue;

	if (pSection->GetItemString(szItemName, iItem, sValue))
	{
		*pszValue = pSection->CreateNewStringSlot(sValue);
		return M1_OK;
	}
	else
	{
		return M1_ITEMNOTEXIST;
	}

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1GetItemNumber(M1Section*		pSection,
										 const TCHAR*	szItemName,
										 int			iItem,
										 int*			piValue)
{
	M1_EXCEPTION_TRY

	if (pSection == NULL) return M1_INVALIDPARAM;
	if (szItemName == NULL) return M1_INVALIDPARAM;
	if (piValue == NULL) return M1_INVALIDPARAM;

	int iValue;

	if (pSection->GetItemNumber(szItemName, iItem, iValue))
	{
		*piValue = iValue;
		return M1_OK;
	}
	else
	{
		return M1_ITEMNOTEXIST;
	}

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1GetItemImage(M1Section*		pSection,
										const TCHAR*	szItemName,
										int				iItem,
										BYTE**			ppImage,
										int*			piImageLen)
{
	M1_EXCEPTION_TRY

	if (pSection == NULL) return M1_INVALIDPARAM;
	if (szItemName == NULL) return M1_INVALIDPARAM;
	if (ppImage == NULL) return M1_INVALIDPARAM;
	if (piImageLen == NULL) return M1_INVALIDPARAM;

	BYTE*	pImage;
	int		iImageLen;

	if (pSection->GetItemImage(szItemName, iItem, &pImage, &iImageLen))
	{
		*ppImage = pImage;
		*piImageLen = iImageLen;
		return M1_OK;
	}
	else
	{
		return M1_ITEMNOTEXIST;
	}

	M1_EXCEPTION_CATCH	
}

OPENM1_API int __stdcall M1Free(BYTE* pImage)
{
	M1_EXCEPTION_TRY

	if (pImage == NULL) return M1_INVALIDPARAM;

	delete [] pImage;

	return M1_OK;

	M1_EXCEPTION_CATCH	
}


//
// Package Validation and Output
//

OPENM1_API int __stdcall M1ValidatePackage(M1Section*	pHeadSection,
										   M1Standard*	pStandard,
										   TCHAR**		pszLog,
										   TCHAR**		pszError)
{
	M1_EXCEPTION_TRY

	if (pHeadSection == NULL) return M1_INVALIDPARAM;
	if (pStandard == NULL) return M1_INVALIDPARAM;
	if (pszLog == NULL) return M1_INVALIDPARAM;
	if (pszError == NULL) return M1_INVALIDPARAM;

	CStdString sLog;
	CStdString sError;
	bool	   bRet;

	bRet = pHeadSection->Validate(pStandard, sLog, sError);

	*pszLog = pStandard->CreateNewStringSlot(sLog);
	*pszError = pStandard->CreateNewStringSlot(sError);

	return bRet ? M1_OK : M1_FAIL;

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1WritePackage(M1Section*		pHeadSection,
										const TCHAR*	szFile)
{
	M1_EXCEPTION_TRY

	if (pHeadSection == NULL) return M1_INVALIDPARAM;
	if (szFile == NULL) return M1_INVALIDPARAM;

	if (!pHeadSection->GetIsValidated()) return M1_NOTVALIDATED;

	if (pHeadSection->WriteToFile(szFile))
	{
		return M1_OK;
	}
	else
	{
		return M1_FILEERROR;
	}

	M1_EXCEPTION_CATCH
}

OPENM1_API int __stdcall M1ReadPackage(const TCHAR*		szFile,
									   M1Section**		ppHeadSection,
									   M1Standard*		pStandard,
									   TCHAR**			pszError)
{
	M1_EXCEPTION_TRY

	if (szFile == NULL) return M1_INVALIDPARAM;
	if (ppHeadSection == NULL) return M1_INVALIDPARAM;
	if (pStandard == NULL) return M1_INVALIDPARAM;
	if (pszError == NULL) return M1_INVALIDPARAM;

	CStdString	sError;
	bool		bRet;

	// Create new empty section and read file into it

	*ppHeadSection = new M1Section(pStandard->GetName(), 1, NULL);

	bRet = (*ppHeadSection)->ReadFromFile(szFile, *pStandard, sError);

	*pszError = pStandard->CreateNewStringSlot(sError);

	if (bRet)
	{
		return M1_OK;
	}
	else
	{
		return M1_FAIL;
	}

	M1_EXCEPTION_CATCH
}
