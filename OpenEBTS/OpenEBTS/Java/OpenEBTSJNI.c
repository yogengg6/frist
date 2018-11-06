#include "Config.h"
#include "OpenEBTS.h"
#include "OpenEBTSErrors.h"
#include "OpenEBTSJNIHelpers.h"
#include "OpenEBTSJNI.h"
#include <malloc.h>

#define RETVAL			int ret = 0;
#define PACKAGERETVAL	PackageReturnValue(env, ret, joRet);

JNIEXPORT jlong JNICALL Java_com_obi_OpenEBTS_IWNew
  (JNIEnv *env, jobject obj, jstring jsTOT, jobject joRet)
{
	RETVAL
	CIWTransaction 	*pTra = NULL;
	const TCHAR		*szTOT = NULL;
	jlong 			nTransaction = 0;

	szTOT = JNIGetString(env, jsTOT);

	ret = IWNew(szTOT, NULL, &pTra);
	
	if (ret == IW_SUCCESS)
	{
		nTransaction = (jlong)pTra;
	}

	JNIReleaseString(env, jsTOT, szTOT);
	PACKAGERETVAL

	return nTransaction;
}

JNIEXPORT jlong JNICALL Java_com_obi_OpenEBTS_IWReadFromFile
  (JNIEnv *env, jobject obj, jstring jsPath, jlong nVerification, jobject joRet)
{
	RETVAL
	const TCHARPATH		*szPath = NULL;
	CIWVerification		*pVer = (CIWVerification*)nVerification;
	CIWTransaction		*pTra = NULL;
	jlong				nTransaction = 0;

	szPath = JNIGetStringPath(env, jsPath);

	ret = IWRead(szPath, pVer, &pTra);

	if (ret == IW_SUCCESS)
	{
		nTransaction = (jlong)pTra;
	}

	JNIReleaseStringPath(env, jsPath, szPath);
	PACKAGERETVAL

	return nTransaction;
}

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWWriteToFile
  (JNIEnv *env, jobject obj, jlong nTransaction, jstring jsPath, jobject joFmt, jobject joRet)
{
	RETVAL
	const TCHARPATH	*szPath = NULL;
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;

	szPath = JNIGetStringPath(env, jsPath);

	ret = IWWrite(pTra, szPath);

	JNIReleaseStringPath(env, jsPath, szPath);
	PACKAGERETVAL
}

JNIEXPORT jlong JNICALL Java_com_obi_OpenEBTS_IWReadFromMem
  (JNIEnv *env, jobject obj, jbyteArray jbaBuffer, jlong nVerification, jobject joRet)
{
	RETVAL
	CIWVerification	*pVer = (CIWVerification*)nVerification;
	CIWTransaction	*pTra = NULL;
	jlong			nTransaction = 0;
	jboolean		jzIsCopy = 0;
	int				cbBuffer = 0;
	jbyte			*pBuffer = NULL;

	cbBuffer = (*env)->GetArrayLength(env, jbaBuffer);
	if (cbBuffer == 0) goto done;

	// Get Java byte array as pointer to bytes
	pBuffer = (*env)->GetByteArrayElements(env, jbaBuffer, &jzIsCopy);
	if (pBuffer == NULL) goto done;

	ret = IWReadMem(pBuffer, cbBuffer, pVer, &pTra);

	if (ret == IW_SUCCESS)
	{
		nTransaction = (jlong)pTra;
	}

done:
	if (jzIsCopy) (*env)->ReleaseByteArrayElements(env, jbaBuffer, pBuffer, 0);

	PACKAGERETVAL

	return nTransaction;
}

JNIEXPORT jbyteArray JNICALL Java_com_obi_OpenEBTS_IWWriteToMem
  (JNIEnv *env, jobject obj, jlong nTransaction, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	int				cbBuffer = 0;
	unsigned char	*pBuffer = NULL;
	jbyteArray		jbaBuffer = NULL;

	ret = IWWriteMem(pTra, &pBuffer, &cbBuffer);

	if (ret == IW_SUCCESS && cbBuffer > 0)
	{
		jbaBuffer = (*env)->NewByteArray(env, cbBuffer);
		(*env)->SetByteArrayRegion(env, jbaBuffer, 0, cbBuffer, pBuffer);
		IWMemFree(&pBuffer);
	}

	PACKAGERETVAL
	return jbaBuffer;
}

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWClose
  (JNIEnv *env, jobject obj, jlong nTransaction, jobject joRet)
 {
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;

	ret = IWClose(&pTra);

	PACKAGERETVAL
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWAddRecord
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	jint			nRecordIndex;

	ret = IWAddRecord(pTra, nRecordType, &nRecordIndex);

	PACKAGERETVAL
	return nRecordIndex;
}

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWDeleteRecord
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;

	ret = IWDeleteRecord(pTra, nRecordType, nRecordIndex);

	PACKAGERETVAL
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWGetRecordTypeCount
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	int				count = 0;	

	ret = IWGetRecordTypeCount(pTra, nRecordType, &count);

	PACKAGERETVAL
	return count;
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWGetNumRecords
  (JNIEnv *env, jobject obj, jlong nTransaction, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	jint			nCount;

	ret = IWGetNumRecords(pTra, &nCount);

	PACKAGERETVAL
	return nCount;
}

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWSetDataViaMnemonic
  (JNIEnv *env, jobject obj, jlong nTransaction, jstring jsMNU, jint nRecordIndex, jint nSecondaryIndex,
  jstring jsData, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	const TCHAR		*szMNU = NULL;
	const TCHAR		*szData = NULL;

	szMNU = JNIGetString(env, jsMNU);
	szData = JNIGetString(env, jsData);

	ret = IWSet(pTra, szMNU, szData, nSecondaryIndex, nRecordIndex);

	JNIReleaseString(env, jsMNU, szMNU);
	JNIReleaseString(env, jsData, szData);
	PACKAGERETVAL
}

JNIEXPORT jstring JNICALL Java_com_obi_OpenEBTS_IWGetDataViaMnemonic
  (JNIEnv *env, jobject obj, jlong nTransaction, jstring jsMNU, jint nRecordIndex, jint nSecondaryIndex, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	const TCHAR		*szMNU = NULL;
	const TCHAR		*szData = NULL;
	jstring			jsData = NULL;

	szMNU = JNIGetString(env, jsMNU);

	ret = IWGet(pTra, szMNU, &szData, nSecondaryIndex, nRecordIndex);

	if (ret == IW_SUCCESS)
	{
		jsData = JNINewString(env, szData);
	}

	JNIReleaseString(env, jsMNU, szMNU);
	PACKAGERETVAL

	return jsData;
}

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWSetImage
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jbyteArray jbaImage,
  jint nFmtIn, jint nFmtOut, jint nCompression, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	jboolean		jzIsCopy = 0;
	int				cbImage = 0;
	jbyte			*pImage = NULL;

	cbImage = (*env)->GetArrayLength(env, jbaImage);
	if (cbImage == 0) goto done;

	// Get Java byte array as pointer to bytes
	pImage = (*env)->GetByteArrayElements(env, jbaImage, &jzIsCopy);
	if (pImage == NULL) goto done;

	ret = IWSetImage(pTra, nRecordType, nRecordIndex, ToFormatString(nFmtIn), cbImage, pImage,
					 ToFormatString(nFmtOut), (float)nCompression);

done:
	if (jzIsCopy) (*env)->ReleaseByteArrayElements(env, jbaImage, pImage, 0);

	PACKAGERETVAL
}

JNIEXPORT jbyteArray JNICALL Java_com_obi_OpenEBTS_IWGetImage
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	const TCHAR		*szFmt = NULL;
	int				cbImage = 0;
	const void		*pImage = NULL;
	jbyteArray		jbaImage = NULL;

	ret = IWGetImage(pTra, nRecordType, nRecordIndex, &szFmt, &cbImage, &pImage);

	if (ret == IW_SUCCESS && cbImage > 0)
	{
		jbaImage = (*env)->NewByteArray(env, cbImage);
		(*env)->SetByteArrayRegion(env, jbaImage, 0, cbImage, pImage);
	}

	PACKAGERETVAL
	return jbaImage;
}

ImageInfo IWGetImageFormatHelper
(JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, int *pret)
//
// Unfortunately there aren't separate methods for these properties so this function
// may get called up to 4 times per image.
//
{
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	const TCHAR		*szFmt = NULL;
	int				cbImage;
	int				nW;
	int				nH;
	int				nBPP;
	ImageInfo		ii;

	*pret = IWGetImageInfo(pTra, nRecordType, nRecordIndex, &szFmt, &cbImage, &nW, &nH, &nBPP);

	ii.nWidth = nW;
	ii.nHeight = nH;
	ii.nDepth = nBPP;
	ii.nFormat = FromFormatString(szFmt);

	return ii;
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWGetImageFormat
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jobject joRet)
{
	RETVAL
	ImageInfo	ii;

	ii = IWGetImageFormatHelper(env, obj, nTransaction, nRecordType, nRecordIndex, &ret);

	PACKAGERETVAL
	return ii.nFormat;
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWGetImageWidth
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jobject joRet)
{
	RETVAL
	ImageInfo ii;

	ii = IWGetImageFormatHelper(env, obj, nTransaction, nRecordType, nRecordIndex, &ret);

	PACKAGERETVAL
	return ii.nWidth;
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWGetImageHeight
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jobject joRet)
{
	RETVAL
	ImageInfo ii;

	ii = IWGetImageFormatHelper(env, obj, nTransaction, nRecordType, nRecordIndex, &ret);

	PACKAGERETVAL
	return ii.nHeight;
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWGetImageDepth
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jobject joRet)
{
	RETVAL
	ImageInfo ii;

	ii = IWGetImageFormatHelper(env, obj, nTransaction, nRecordType, nRecordIndex, &ret);

	PACKAGERETVAL
	return ii.nDepth;
}

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWSetImageFromFile
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex,
  jstring jsPath, jint nFmtIn, jint nFmtOut, jint nCompression, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	const TCHARPATH	*szPath = NULL;

	szPath = JNIGetStringPath(env, jsPath);

	ret = IWImportImage(pTra, nRecordType, nRecordIndex, szPath, ToFormatString(nFmtOut),
						(float)nCompression, ToFormatString(nFmtIn));

	JNIReleaseStringPath(env, jsPath, szPath);
	PACKAGERETVAL
}

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWGetImageAsToFile
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jstring jsPath, jint nFmt, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	const TCHARPATH	*szPath = NULL;

	szPath = JNIGetStringPath(env, jsPath);

	ret = IWExportImage(pTra, nRecordType, nRecordIndex, szPath, ToFormatString(nFmt));

	JNIReleaseStringPath(env, jsPath, szPath);
	PACKAGERETVAL
}

JNIEXPORT jbyteArray JNICALL Java_com_obi_OpenEBTS_IWGetImageAs
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jint nFmt, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	jbyteArray		jbaImage = NULL;
	int				cbImage = 0;
	jbyte			*pImage = NULL;

	ret = IWExportImageMem(pTra, nRecordType, nRecordIndex, &pImage, &cbImage, ToFormatString(nFmt));

	if (ret == IW_SUCCESS && cbImage > 0)
	{
		jbaImage = (*env)->NewByteArray(env, cbImage);
		(*env)->SetByteArrayRegion(env, jbaImage, 0, cbImage, pImage);
		IWMemFree(&pImage);
	}

	PACKAGERETVAL
	return jbaImage;
}

JNIEXPORT jlong JNICALL Java_com_obi_OpenEBTS_IWReadVerification
  (JNIEnv *env, jobject obj, jstring jsPath, jobject jobsbParseErrorOut, jobject joRet)
{
	RETVAL
	const TCHARPATH		*szPath = NULL;
	CIWVerification		*pVer = NULL;
	jlong				nVerification = 0;
	TCHAR				szParseError[1024];

	szPath = JNIGetStringPath(env, jsPath);

	ret = IWReadVerification(szPath, &pVer, 1024, szParseError);

	if (ret == IW_SUCCESS)
	{
		nVerification = (jlong)pVer;
	}
	else
	{
		if (_tcslen(szParseError))
		{
			JNIAppendStringtoStringBuffer(env, jobsbParseErrorOut, szParseError);
		}
	}

	JNIReleaseStringPath(env, jsPath, szPath);
	PACKAGERETVAL

	return nVerification;
}

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWCloseVerification
  (JNIEnv *env, jobject obj, jlong nVerification, jobject joRet)
{
	RETVAL
	CIWVerification	*pVer = (CIWVerification*)nVerification;

	ret = IWCloseVerification(&pVer);

	PACKAGERETVAL
}

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWSetVerification
  (JNIEnv *env, jobject obj, jlong nTransaction, jlong nVerification, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	CIWVerification	*pVer = (CIWVerification*)nVerification;

	ret = IWSetVerification(pTra, pVer);

	PACKAGERETVAL
}

JNIEXPORT jlong JNICALL Java_com_obi_OpenEBTS_IWCloneVerification
  (JNIEnv *env, jobject obj, jlong nVerification, jobject joRet)
{
	RETVAL
	CIWVerification	*pVer = (CIWVerification*)nVerification;
	CIWVerification	*pVerClone;
	jlong			nVerificationClone = 0;

	ret = IWCloneVerification(pVer, &pVerClone);

	if (ret == IW_SUCCESS)
	{
		nVerificationClone = (jlong)pVerClone;
	}

	PACKAGERETVAL

	return nVerificationClone;
}

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWVerify
  (JNIEnv *env, jobject obj, jlong nTransaction, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;

	ret = IWVerify(pTra);

	PACKAGERETVAL
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWGetErrorCount
  (JNIEnv *env, jobject obj, jlong nTransaction, jobject joRet)
// Note, in this function ret is st explicitly to IW_SUCCESS since
// IWGetErrorCount doesn't return an error code.
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	int				nCount = 0;

	nCount = IWGetErrorCount(pTra);
	ret = IW_SUCCESS;

	PACKAGERETVAL
	return nCount;
}

JNIEXPORT jstring JNICALL Java_com_obi_OpenEBTS_IWGetErrorString
 (JNIEnv *env, jobject obj, jlong nTransaction, jint nIndex, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	int				nCode = 0;
	const TCHAR		*szError = NULL;
	jstring			jsError = NULL;

	ret = IWGetError(pTra, nIndex, &nCode, &szError);

	if (ret == IW_SUCCESS && _tcslen(szError))
	{
		jsError = JNINewString(env, szError);
	}

	PACKAGERETVAL
	return jsError;
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWGetErrorCode
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nIndex, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	const TCHAR		*szError = NULL;
	int				nCode = 0;

	ret = IWGetError(pTra, nIndex, &nCode, &szError);

	PACKAGERETVAL
	return nCode;
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWGetFieldCount
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	int				count = 0;	

	ret = IWGetNumFields(pTra, nRecordType, nRecordIndex, &count);

	PACKAGERETVAL
	return count;
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWGetNextField
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jint nFieldIndex, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	int				nFieldIndexNext = 0;	

	ret = IWGetNextField(pTra, nRecordType, nRecordIndex, nFieldIndex, &nFieldIndexNext);

	PACKAGERETVAL
	return nFieldIndexNext;
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWNumSubfields
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jint nFieldIndex,
  jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	int				count = 0;

	ret = IWNumSubfields(pTra, nRecordType, nRecordIndex, nFieldIndex, &count);

	PACKAGERETVAL
	return count;
}

JNIEXPORT jint JNICALL Java_com_obi_OpenEBTS_IWNumItems
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jint nFieldIndex,
  jint nSubfieldIndex, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	int				count = 0;

	ret = IWNumItems(pTra, nRecordType, nRecordIndex, nFieldIndex, nSubfieldIndex, &count);

	PACKAGERETVAL
	return count;
}

JNIEXPORT jstring JNICALL Java_com_obi_OpenEBTS_IWFindItem
  (JNIEnv *env, jobject obj, jlong nTransaction, jint nRecordType, jint nRecordIndex, jint nFieldIndex,
  jint nSubfieldIndex, jint nItemIndex, jobject joRet)
 {
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	const TCHAR		*szData = NULL;
	jstring			jsData = NULL;

	ret = IWFindItem(pTra, nRecordType, nRecordIndex, nFieldIndex, nSubfieldIndex, nItemIndex, &szData);

	if (ret == IW_SUCCESS)
	{
		jsData = JNINewString(env, szData);
	}

	PACKAGERETVAL
	return jsData;
 }

JNIEXPORT void JNICALL Java_com_obi_OpenEBTS_IWSetItem
  (JNIEnv *env, jobject obj, jlong nTransaction, jstring jsData, jint nRecordType, jint nRecordIndex,
  jint nFieldIndex, jint nSubfieldIndex, jint nItemIndex, jobject joRet)
{
	RETVAL
	CIWTransaction	*pTra = (CIWTransaction*)nTransaction;
	const TCHAR		*szData = NULL;

	szData = JNIGetString(env, jsData);

	ret = IWSetItem(pTra, szData, nRecordType, nRecordIndex, nFieldIndex, nSubfieldIndex, nItemIndex);

	JNIReleaseString(env, jsData, szData);
	PACKAGERETVAL
}

JNIEXPORT jobjectArray JNICALL Java_com_obi_OpenEBTS_IWGetTransactionCategories
  (JNIEnv *env, jobject obj, jlong nVerification, jobject joRet)
{
	RETVAL
	CIWVerification	*pVer = (CIWVerification*)nVerification;
	int				nCategories = 0;
	const TCHAR		*szCategories[100];	// a reasonable amount of categories
	jobjectArray	jsaCategories = NULL;

	ret = IWGetTransactionCategories(pVer, 100, (const TCHAR**)&szCategories, &nCategories);

	if (ret == IW_SUCCESS && nCategories > 0)
	{
		jsaCategories = JNICreateStringArray(env, szCategories, nCategories);
	}

	PACKAGERETVAL
	return jsaCategories;
}

JNIEXPORT jobjectArray JNICALL Java_com_obi_OpenEBTS_IWGetTransactionTypeNames
  (JNIEnv *env, jobject obj, jlong nVerification, jstring jsCategory, jobject joRet)
 {
	RETVAL
	CIWVerification	*pVer = (CIWVerification*)nVerification;
	int				nTOTs = 0;
	const TCHAR		*szNames[100];	// a reasonable amount of TOTs
	const TCHAR		*szCategory = NULL;
	jobjectArray	jsaNames = NULL;

	szCategory = JNIGetString(env, jsCategory);

	ret = IWGetTransactionTypes(pVer, 100, (const TCHAR**)&szNames, NULL, &nTOTs, szCategory);

	if (ret == IW_SUCCESS && nTOTs > 0)
	{
		jsaNames = JNICreateStringArray(env, szNames, nTOTs);
	}

	JNIReleaseString(env, jsCategory, szCategory);

	PACKAGERETVAL
	return jsaNames;
}

JNIEXPORT jobjectArray JNICALL Java_com_obi_OpenEBTS_IWGetTransactionTypeDescriptions
  (JNIEnv *env, jobject obj, jlong nVerification, jstring jsCategory, jobject joRet)
{
	RETVAL
	CIWVerification	*pVer = (CIWVerification*)nVerification;
	int				nTOTs = 0;
	const TCHAR		*szDescriptions[100];	// a reasonable amount of TOTs
	const TCHAR		*szCategory = NULL;
	jobjectArray	jsaDescriptions = NULL;

	szCategory = JNIGetString(env, jsCategory);

	ret = IWGetTransactionTypes(pVer, 100, NULL, szDescriptions, &nTOTs, szCategory);

	if (ret == IW_SUCCESS && nTOTs > 0)
	{
		jsaDescriptions = JNICreateStringArray(env, szDescriptions, nTOTs);
	}

	JNIReleaseString(env, jsCategory, szCategory);

	PACKAGERETVAL
	return jsaDescriptions;
}

JNIEXPORT jobjectArray JNICALL Java_com_obi_OpenEBTS_IWGetRecordTypeOccurrences
  (JNIEnv *env, jobject obj, jlong nVerification, jstring jsTOT, jobject joRet)
// IWGetRecordTypeOccurrences returns a 2 dimensional array of ints of size
// 3 x n, where index 0 indexes the Record Type array, and index 1 and 2 the min
// and max allowed occurrences arrays, respectively.
{
	RETVAL
	jobjectArray	jnaOccurrences = NULL;
	CIWVerification	*pVer = (CIWVerification*)nVerification;
	int				nOccurrences = 0;
	const TCHAR		*szTOT = NULL;
	int				rgnRecordType[100];
	int				rgnMin[100];
	int				rgnMax[100];

	szTOT = JNIGetString(env, jsTOT);

	ret = IWGetRecordTypeOccurrences(pVer, 100, rgnRecordType, rgnMin, rgnMax, &nOccurrences, szTOT);

	if (ret == IW_SUCCESS && nOccurrences > 0)
	{
		jnaOccurrences = JNICreate3xNIntArray(env, rgnRecordType, rgnMin, rgnMax, nOccurrences);
	}

	JNIReleaseString(env, jsTOT, szTOT);
	PACKAGERETVAL
	return jnaOccurrences;
}


JNIEXPORT jobjectArray JNICALL Java_com_obi_OpenEBTS_IWGetMnemonicsNames
  (JNIEnv *env, jobject obj, jlong nVerification, jstring jsTOT, jobject joRet)
{
	RETVAL
	CIWVerification	*pVer = (CIWVerification*)nVerification;
	int				nMNUs = 0;
	const TCHAR		*szTOT = NULL;
	const TCHAR		*szNames[1000];	// a reasonable amount of MNUs
	jobjectArray	jsaNames = NULL;

	szTOT = JNIGetString(env, jsTOT);

	ret = IWGetMnemonics(pVer, szTOT, 1000, szNames, NULL, &nMNUs);

	if (ret == IW_SUCCESS && nMNUs > 0)
	{
		jsaNames = JNICreateStringArray(env, szNames, nMNUs);
	}

	JNIReleaseString(env, jsTOT, szTOT);

	PACKAGERETVAL
	return jsaNames;
}

JNIEXPORT jobjectArray JNICALL Java_com_obi_OpenEBTS_IWGetMnemonicsDescriptions
  (JNIEnv *env, jobject obj, jlong nVerification, jstring jsTOT, jobject joRet)
{
	RETVAL
	CIWVerification	*pVer = (CIWVerification*)nVerification;
	int				nMNUs = 0;
	const TCHAR		*szDescriptions[1000];	// a reasonable amount of MNUs
	jobjectArray	jsaDescriptions = NULL;
	const TCHAR		*szTOT = NULL;

	szTOT = JNIGetString(env, jsTOT);

	ret = IWGetMnemonics(pVer, szTOT, 1000, NULL, szDescriptions, &nMNUs);

	if (ret == IW_SUCCESS && nMNUs > 0)
	{
		jsaDescriptions = JNICreateStringArray(env, szDescriptions, nMNUs);
	}

	JNIReleaseString(env, jsTOT, szTOT);
	PACKAGERETVAL
	return jsaDescriptions;
}

JNIEXPORT jobject JNICALL Java_com_obi_OpenEBTS_IWGetRuleRestrictions
  (JNIEnv *env, jobject obj, jlong nVerification, jstring jsTOT, jstring jsMNU, jobject joRet)
// Because IWGetRuleRestrictions returns so many parameters in C, in Java we return an entire
// structure, NISTFieldRules.
{
	RETVAL
	CIWVerification	*pVer = (CIWVerification*)nVerification;
	const TCHAR		*szTOT = NULL;
	const TCHAR		*szMNU = NULL;
	jobject			jobjRules = NULL;
	int				nRecordType;
	int				nField;
	int				nSubfield;
	int				nItem;
	const TCHAR		*szDesc = NULL;
	const TCHAR		*szLongDesc = NULL;
	const TCHAR		*szCharType = NULL;
	const TCHAR		*szSpecialChars = NULL;
	const TCHAR		*szDateFormat = NULL;
	const TCHAR		*szAdvancedRule = NULL;
	int				nSizeMin;
	int				nSizeMax;
	int				nOccMin;
	int				nOccMax;
	int				nOffset;
	int				bAutomaticallySet;
	int				bMandatory;

	szTOT = JNIGetString(env, jsTOT);
	szMNU = JNIGetString(env, jsMNU);

	ret = IWGetRuleRestrictions(pVer, szTOT, szMNU,
								&nRecordType, &nField, &nSubfield, &nItem, &szDesc, &szLongDesc, &szCharType,
								&szSpecialChars, &szDateFormat, &szAdvancedRule, &nSizeMin, &nSizeMax, &nOccMin,
								&nOccMax, &nOffset, &bAutomaticallySet, &bMandatory);

	if (ret == IW_SUCCESS)
	{
		jobjRules = JNICreateNISTFieldRules(env, szMNU,
						nRecordType, nField, nSubfield, nItem, szDesc, szLongDesc, szCharType,
						szSpecialChars, szDateFormat, szAdvancedRule, nSizeMin, nSizeMax, nOccMin,
						nOccMax, nOffset, bAutomaticallySet, bMandatory);
	}

	JNIReleaseString(env, jsTOT, szTOT);
	JNIReleaseString(env, jsMNU, szMNU);
	PACKAGERETVAL
	return jobjRules;
}

JNIEXPORT jobject JNICALL Java_com_obi_OpenEBTS_IWGetValueList
  (JNIEnv *env, jobject obj, jlong nVerification, jstring jsTOT, jstring jsMNU, jobject joRet)
 {
	RETVAL
	CIWVerification	*pVer = (CIWVerification*)nVerification;
	const TCHAR		*szTOT = NULL;
	const TCHAR		*szMNU = NULL;
	const TCHAR		**szName = NULL;
	const TCHAR		**szValue = NULL;
	int				nValues;
	int				bMandatory;
	jobject			jobjValues = NULL;

	szTOT = JNIGetString(env, jsTOT);
	szMNU = JNIGetString(env, jsMNU);

	// Calling IWGetValueList with nDataArraySize of 0 gives us the number of entries
	ret = IWGetValueList(pVer, szTOT, szMNU, NULL, 0, NULL, NULL, &nValues);
	if (ret == IW_SUCCESS && nValues > 0)
	{
		szName = malloc(sizeof(TCHAR*) * nValues);
		szValue = malloc(sizeof(TCHAR*) * nValues);
		ret = IWGetValueList(pVer, szTOT, szMNU, &bMandatory, nValues, szValue, szName, &nValues);
	}

	if (ret == IW_SUCCESS && nValues >  0)
	{
		jobjValues = JNICreateNISTValueList(env, szTOT, szMNU, bMandatory, szName, szValue, nValues);
	}

	if (szName != NULL) free((void*)szName);
	if (szValue != NULL) free((void*)szValue);
	JNIReleaseString(env, jsTOT, szTOT);
	JNIReleaseString(env, jsMNU, szMNU);
	PACKAGERETVAL
	return jobjValues;
 }
