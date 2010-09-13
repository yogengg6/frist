
#ifndef _OPENM1_H_
#define _OPENM1_H_

#ifdef OPENM1_EXPORTS
#define OPENM1_API __declspec(dllexport)
#else
#define OPENM1_API __declspec(dllimport)
#endif

#define BYTE unsigned char

enum M1ItemFormat {IF_undefined, IF_bit, IF_byte, IF_word, IF_dword, IF_tword, IF_string, IF_binary,
				   IF_jpegorjpeg2000, IF_image };

#define M1_OK				0
#define M1_SECTIONNOTEXIST	1
#define M1_ITEMNOTEXIST		2
#define M1_EXCEPTION		3
#define M1_FAIL				4
#define M1_INVALIDPARAM		5
#define M1_MEMORYFAULT		6
#define M1_FILEERROR		7
#define M1_NOTVALIDATED		8

#ifdef  __cplusplus
  class M1Standard;
  class M1ContainerDef;
  class M1Section;
#else
  typedef struct M1Standard M1Standard;
  typedef struct M1ContainerDef M1ContainerDef;
  typedef struct M1Section M1Section;
#endif // __cplusplus


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//
// Functions relating to the M1Standard XML configuration object
//

OPENM1_API int __stdcall M1ReadStandard(
	const TCHAR* szPath,				// Path name to file
	M1Standard** ppStandard,			// Returns new standard pointer
	int nMaxError,						// Maximum size of szError input
	TCHAR* szError						// Potential errors encountered in standard file
);

OPENM1_API int __stdcall M1FreeStandard(
	M1Standard* pStandard
);

OPENM1_API int __stdcall M1GetHeadContainer(
	M1Standard* pStandard,
	M1ContainerDef** ppContainer		// Returns container pointer
);

OPENM1_API int __stdcall M1GetContainerInfo(
	M1ContainerDef* pContainer,			// Input container
	bool*			pbIsItemDef,
	TCHAR**			pszName,
	TCHAR**			pszDescription,
	TCHAR**			pszMinOccurrences,
	TCHAR**			pszMaxOccurrences,
	bool			*pbIsReserved
);

OPENM1_API int __stdcall M1EnumContainerReset(
	M1ContainerDef* pContainer			// Input container
);

OPENM1_API int __stdcall M1EnumContainerGetNext(
	M1ContainerDef* pContainer,			// Input container
	M1ContainerDef** ppContainerNext	// Output container
);

OPENM1_API int __stdcall M1GetItemDefInfo(
	M1ContainerDef*	pContainer,			// Input container
	M1ItemFormat*	pItemFormat,
	TCHAR**			pszMinValue,
	TCHAR**			pszMaxValue,
	TCHAR**			pszDefaultValue
);

//
// Functions relating to the creating or modification of an M1Package
//

OPENM1_API int __stdcall M1CreatePackage(
	const TCHAR*	szPackageName,
	M1Section**		ppHeadSection
);

OPENM1_API int __stdcall M1FreePackage(
	M1Section*		pHeadSection
);

OPENM1_API int __stdcall M1ReturnSection(
	M1Section*		pParentSection,
	const TCHAR*	szSectionName,
	int				iSection,
	M1Section**		ppSection
);

OPENM1_API int __stdcall M1SetItemString(
	M1Section*		pSection,
	const TCHAR*	szItemName,
	int				iItem,
	const TCHAR*	szValue
);

OPENM1_API int __stdcall M1SetItemNumber(
	M1Section*		pSection,
	const TCHAR*	szItemName,
	int				iItem,
	int				iValue
);

OPENM1_API int __stdcall M1SetItemBit(
	M1Section*		pSection,
	const TCHAR*	szItemName,
	int				iItem,
	bool			bValue
);

OPENM1_API int __stdcall M1SetItemImage(
	M1Section*		pSection,
	const TCHAR*	szItemName,
	int				iItem,
	BYTE*			pImage,
	int				iImageLen
);

OPENM1_API int __stdcall M1GetItemString(
	M1Section*		pSection,
	const TCHAR*	szItemName,
	int				iItem,
	TCHAR**			pszValue
);

OPENM1_API int __stdcall M1GetItemNumber(
	M1Section*		pSection,
	const TCHAR*	szItemName,
	int				iItem,
	int*			piValue
);

OPENM1_API int __stdcall M1GetItemImage(
	M1Section*		pSection,
	const TCHAR*	szItemName,
	int				iItem,
	BYTE**			ppImage,
	int*			piImageLen
);

OPENM1_API int __stdcall M1Free(
	BYTE*			pImage
);

//
// Package Validation and Input/Output
//

OPENM1_API int __stdcall M1ValidatePackage(
	M1Section*		pHeadSection,
	M1Standard*		pStandard,
	TCHAR**			pszLog,
	TCHAR**			pszError
);

OPENM1_API int __stdcall M1WritePackage(
	M1Section*		pHeadSection,
	const TCHAR*	szFile
);

OPENM1_API int __stdcall M1ReadPackage(
	const TCHAR*	szFile,
	M1Section**		ppHeadSection,
	M1Standard*		pStandard,
	TCHAR**			pszError
);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _OPENM1_H_