using System;
using System.Runtime.InteropServices;
using System.Text;


namespace OpenEBTSNet
{
	internal static class OpenEBTSInterop
	{
		#region EBTS file related

		/// Return Type: int
		///szPath: wchar_t*
		///pIWVer: void*
		///ppIWTrans: void**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWRead")]
		internal static extern int IWRead([InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szPath, IntPtr pIWVer, ref IntPtr ppIWTrans);

		/// Return Type: int
		///pBuffer: unsigned char*
		///nBufferSize: int
		///pIWVer: void*
		///ppIWTrans: void**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWReadMem")]
		internal static extern int IWReadMem(IntPtr pBuffer, int nBufferSize, IntPtr pIWVer, ref IntPtr ppIWTrans);

		/// Return Type: int
		///pIWTrans: void*
		///szPath: wchar_t*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWWrite")]
		internal static extern int IWWrite(IntPtr pIWTrans, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szPath);

		/// Return Type: int
		///pIWTrans: void*
		///ppBuffer: unsigned char**
		///pSize: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWWriteMem")]
		internal static extern int IWWriteMem(IntPtr pIWTrans, ref IntPtr ppBuffer, ref int pSize);

		/// Return Type: int
		///ppIWTrans: void**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWClose")]
		internal static extern int IWClose(ref IntPtr ppIWTrans);

		/// Return Type: int
		///szTransactionType: wchar_t*
		///pIWVer: void*
		///ppIWTrans: void**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWNew")]
		internal static extern int IWNew([InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szTransactionType, IntPtr pIWVer, ref IntPtr ppIWTrans);

		/// Return Type: int
		///ppBuffer: unsigned char**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWMemFree")]
		internal static extern int IWMemFree(ref IntPtr ppBuffer);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///pnRecordIndex: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWAddRecord")]
		internal static extern int IWAddRecord(IntPtr pIWTrans, int nRecordType, ref int pnRecordIndex);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///pnRecordTypeCount: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetRecordTypeCount")]
		internal static extern int IWGetRecordTypeCount(IntPtr pIWTrans, int nRecordType, ref int pnRecordTypeCount);

		/// Return Type: int
		///pIWTrans: void*
		///szPath: wchar_t*
		///bValidate: int
		[DllImport("OpenEBTS.dll", EntryPoint = "IWWriteXML")]
		internal static extern int IWWriteXML(IntPtr pIWTrans, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szPath, int bValidate);

		/// Return Type: int
		///pIWTrans: void*
		///pnRecords: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetNumRecords")]
		internal static extern int IWGetNumRecords(IntPtr pIWTrans, ref int pnRecords);

		/// Return Type: int
		///pIWTrans: void*
		///szMnemonic: wchar_t*
		///pszData: wchar_t**
		///nIndex: int
		///nRecordIndex: int
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGet")]
		internal static extern int IWGet(IntPtr pIWTrans, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szMnemonic, ref IntPtr pszData, int nIndex, int nRecordIndex);

		/// Return Type: int
		///pIWTrans: void*
		///szMnemonic: wchar_t*
		///szData: wchar_t*
		///nStartIndex: int
		///nRecordIndex: int
		[DllImport("OpenEBTS.dll", EntryPoint = "IWSet")]
		internal static extern int IWSet(IntPtr pIWTrans, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szMnemonic, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szData, int nStartIndex, int nRecordIndex);

		/// Return Type: int
		///pIWTrans: void*
		///szMnemonic: wchar_t*
		///nIndex: int
		///nRecordIndex: int
		[DllImport("OpenEBTS.dll", EntryPoint = "IWRemove")]
		internal static extern int IWRemove(IntPtr pIWTrans, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szMnemonic, int nIndex, int nRecordIndex);

		/// Return Type: int
		///pIWTrans: void*
		///szMnemonic: wchar_t*
		///pnOccurrences: int*
		///nRecordIndex: int
		[DllImport("OpenEBTS.dll", EntryPoint = "IWOccurrences")]
		internal static extern int IWOccurrences(IntPtr pIWTrans, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szMnemonic, ref int pnOccurrences, int nRecordIndex);

		/// Return Type: int
		///pIWTrans: void*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWVerify")]
		internal static extern int IWVerify(IntPtr pIWTrans);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///nField: int
		///pnCount: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWNumSubfields")]
		internal static extern int IWNumSubfields(IntPtr pIWTrans, int nRecordType, int nRecordIndex, int nField, ref int pnCount);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///nField: int
		///nSubfield: int
		///pnCount: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWNumItems")]
		internal static extern int IWNumItems(IntPtr pIWTrans, int nRecordType, int nRecordIndex, int nField, int nSubfield, ref int pnCount);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///nField: int
		///nSubfield: int
		///nItem: int
		///pszData: wchar_t**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWFindItem")]
		internal static extern int IWFindItem(IntPtr pIWTrans, int nRecordType, int nRecordIndex, int nField, int nSubfield, int nItem, out IntPtr pszData);

		/// Return Type: int
		///pIWTrans: void*
		///szData: wchar_t*
		///nRecordType: int
		///nRecordIndex: int
		///nField: int
		///nSubfield: int
		///nItem: int
		[DllImport("OpenEBTS.dll", EntryPoint = "IWSetItem")]
		internal static extern int IWSetItem(IntPtr pIWTrans, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szData, int nRecordType, int nRecordIndex, int nField, int nSubfield, int nItem);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///nField: int
		///nSubfield: int
		///nItem: int
		[DllImport("OpenEBTS.dll", EntryPoint = "IWRemoveItem")]
		internal static extern int IWRemoveItem(IntPtr pIWTrans, int nRecordType, int nRecordIndex, int nField, int nSubfield, int nItem);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///pnCount: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetNumFields")]
		internal static extern int IWGetNumFields(IntPtr pIWTrans, int nRecordType, int nRecordIndex, ref int pnCount);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///nField: int
		///pnNextField: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetNextField")]
		internal static extern int IWGetNextField(IntPtr pIWTrans, int nRecordType, int nRecordIndex, int nField, ref int pnNextField);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///pszStorageFormat: wchar_t**
		///pnLength: int*
		///Data: void**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetImage")]
		internal static extern int IWGetImage(IntPtr pIWTrans, int nRecordType, int nRecordIndex, ref IntPtr pszStorageFormat, ref int pnLength, ref IntPtr Data);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///szInputFormat: wchar_t*
		///nLength: int
		///pData: void*
		///szStorageFormat: wchar_t*
		///fCompression: float
		[DllImport("OpenEBTS.dll", EntryPoint = "IWSetImage")]
		internal static extern int IWSetImage(IntPtr pIWTrans, int nRecordType, int nRecordIndex, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szInputFormat, int nLength, IntPtr pData, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szStorageFormat, float fCompression);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///szPath: wchar_t*
		///szStorageFormat: wchar_t*
		///fCompression: float
		///szInputFormat: wchar_t*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWImportImage")]
		internal static extern int IWImportImage(IntPtr pIWTrans, int nRecordType, int nRecordIndex, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szPath, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szStorageFormat, float fCompression, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szInputFormat);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///szPath: wchar_t*
		///szOutputFormat: wchar_t*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWExportImage")]
		internal static extern int IWExportImage(IntPtr pIWTrans, int nRecordType, int nRecordIndex, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szPath, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szOutputFormat);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///ppBuffer: unsigned char**
		///pSize: int*
		///szOutputFormat: wchar_t*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWExportImageMem")]
		internal static extern int IWExportImageMem(IntPtr pIWTrans, int nRecordType, int nRecordIndex, ref IntPtr ppBuffer, ref int pSize, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szOutputFormat);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		///pszStorageFormat: wchar_t**
		///pnLength: int*
		///pnHLL: int*
		///pnVLL: int*
		///pnBitsPerPixel: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetImageInfo")]
		internal static extern int IWGetImageInfo(IntPtr pIWTrans, int nRecordType, int nRecordIndex, out IntPtr pszStorageFormat, ref int pnLength, ref int pnHLL, ref int pnVLL, ref int pnBitsPerPixel);

		/// Return Type: int
		///pIWTrans: void*
		///nRecordType: int
		///nRecordIndex: int
		[DllImport("OpenEBTS.dll", EntryPoint = "IWDeleteRecord")]
		internal static extern int IWDeleteRecord(IntPtr pIWTrans, int nRecordType, int nRecordIndex);

		/// Return Type: int
		///pImage: unsigned char*
		[DllImport("OpenEBTS.dll", EntryPoint = "MemFree")]
		internal static extern int MemFree(IntPtr pImage);

		/// Return Type: int
		///pIWTrans: void*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetErrorCount")]
		internal static extern int IWGetErrorCount(IntPtr pIWTrans);

		/// Return Type: int
		///pIWTrans: void*
		///Index: int
		///Code: int*
		///pszDesc: wchar_t**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetError")]
		internal static extern int IWGetError(IntPtr pIWTrans, int Index, ref int Code, out IntPtr pszDesc);

		/// Return Type: int
		///pIWTrans: void*
		///pIWVer: void*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWSetVerification")]
		internal static extern int IWSetVerification(IntPtr pIWTrans, IntPtr pIWVer);

		#endregion EBTS file related

		#region Verification file related

		/// Return Type: int
		///szPath: wchar_t*
		///ppIWVer: void**
		///nMaxParseError: int
		///szParseError: wchar_t*
		[DllImport(@"OpenEBTS.dll", EntryPoint = "IWReadVerification", CharSet = CharSet.Unicode)]
		internal static extern int IWReadVerification([InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szPath,
			ref IntPtr ppIWVer, int nMaxParseError,
			[MarshalAsAttribute(UnmanagedType.LPWStr)] StringBuilder parseError);

		/// Return Type: int
		///ppIWVer: void**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWCloseVerification", CharSet = CharSet.Unicode)]
		internal static extern int IWCloseVerification(ref IntPtr ppIwVer);

		/// Return Type: int
		///pIWVer: void*
		///nDataArraySize: int
		///rgszDataArray: wchar_t**
		///pnEntries: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetTransactionCategories", CharSet = CharSet.Unicode)]
		internal static extern int IWGetTransactionCategories(IntPtr pIWVer, 
			int nDataArraySize,
			[MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr)] IntPtr[] str, 
			ref int pnEntries);

		/// Return Type: int
		///pIWVer: void*
		///nDataArraySize: int
		///rgszDataArray: wchar_t**
		///rgszDescArray: wchar_t**
		///pnEntries: int*
		///szCategory: wchar_t*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetTransactionTypes", CharSet = CharSet.Unicode)]
		internal static extern int IWGetTransactionTypes(IntPtr pIWVer, int nDataArraySize,
			[MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr)] IntPtr[] strData,
			[MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr)] IntPtr[] strDesc,
			ref int pnEntries,
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szCategory);

		/// Return Type: int
		///pIWVer: void*
		///nDataArraySize: int
		///rgnRecordType: int*
		///rgnMinOccurences: int*
		///rgnMaxOccurences: int*
		///pnEntries: int*
		///szTOT: wchar_t*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetRecordTypeOccurrences", CharSet = CharSet.Unicode)]
		internal static extern int IWGetRecordTypeOccurrences(IntPtr pIWVer,
			int nDataArraySize,
			[MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.SysInt)] int[] rgnRecordType,
			[MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.SysInt)] int[] rgnMinOccurences,
			[MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.SysInt)] int[] rgnMaxOccurences,
			ref int pnEntries,
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szTOT);

		/// Return Type: int
		///pIWVer: void*
		///szTransactionType: wchar_t*
		///nDataArraySize: int
		///rgszDataArray: wchar_t**
		///rgszDescArray: wchar_t**
		///pnEntries: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetMnemonics", CharSet = CharSet.Unicode)]
		internal static extern int IWGetMnemonics(
			IntPtr pIWVer, [InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szTransactionType,
			int nDataArraySize,
			[MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr)] IntPtr[] dataArray,
			[MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr)] IntPtr[] descArray,
			ref int pnEntries);

		/// Return Type: int
		///pIWVer: void*
		///szTransactionType: wchar_t*
		///szMnemonic: wchar_t*
		///pnRecordType: int*
		///pnField: int*
		///pnSubfield: int*
		///pnItem: int*
		///pszDesc: wchar_t**
		///pszLongDesc: wchar_t**
		///pszCharType: wchar_t**
		///pszSpecialChars: wchar_t**
		///pszDateFormat: wchar_t**
		///pszAdvancedRule: wchar_t**
		///pnSizeMin: int*
		///pnSizeMax: int*
		///pnOccMin: int*
		///pnOccMax: int*
		///pnOffset: int*
		///pbAutomaticallySet: int*
		///pbMandatory: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetRuleRestrictions", CharSet = CharSet.Unicode)]
		internal static extern int IWGetRuleRestrictions(
			IntPtr pIWVer,
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szTransactionType,
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szMnemonic,
			ref int pnRecordType,
			ref int pnField,
			ref int pnSubfield,
			ref int pnItem,
			out IntPtr pszDesc,
			out IntPtr pszLongDesc,
			out IntPtr pszCharType,
			out IntPtr pszSpecialChars,
			out IntPtr pszDateFormat,
			out IntPtr pszAdvancedRule,
			ref int pnSizeMin,
			ref int pnSizeMax,
			ref int pnOccMin,
			ref int pnOccMax,
			ref int pnOffset,
			ref int pbAutomaticallySet,
			ref int pbMandatory);

		/// Return Type: int
		///pIWVer: void*
		///szTransactionType: wchar_t*
		///szMnemonic: wchar_t*
		///pszLocationIndex: wchar_t**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetLocationIndex", CharSet = CharSet.Unicode)]
		internal static extern int IWGetLocationIndex(IntPtr pIWVer,
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szTransactionType,
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szMnemonic,
			out IntPtr pLocationIndex);

		/// Return Type: int
		///pIWVer: void*
		///szTransactionType: wchar_t*
		///szMnemonic: wchar_t*
		///pbMandatory: int*
		///nDataArraySize: int
		///rgszDataArray: wchar_t**
		///rgszDescArray: wchar_t**
		///pnEntries: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetValueList", CharSet = CharSet.Unicode)]
		internal static extern int IWGetValueList(IntPtr pIWVer,
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szTransactionType,
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szMnemonic,
			ref int pbMandatory,
			int nDataArraySize,
			[MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr)] IntPtr[] dataArray,
			[MarshalAsAttribute(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr)] IntPtr[] descArray,
			ref int pnEntries);

		/// Return Type: int
		///pIWVer: void*
		///szTransactionType: wchar_t*
		///szMnemonic: wchar_t*
		///pszFilename: wchar_t**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWGetValueListFilename", CharSet = CharSet.Unicode)]
		internal static extern int IWGetValueListFilename(IntPtr pIWVer,
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szTransactionType,
			[InAttribute()] [MarshalAsAttribute(UnmanagedType.LPWStr)] string szMnemonic,
			out IntPtr pfilename);

		/// Return Type: int
		///pIWVer: void*
		///ppIWVer: void**
		[DllImport("OpenEBTS.dll", EntryPoint = "IWCloneVerification", CharSet = CharSet.Unicode)]
		private static extern int IWCloneVerification(IntPtr pIWVer, ref IntPtr ppIWVer);

		#endregion Verification file related

		#region Image conversion

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///ppImageOut: unsigned char**
		///pcbOut: int*
		///pWidth: int*
		///pHeight: int*
		///pDPI: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "BMPtoRAW")]
		internal static extern int BMPtoRAW(IntPtr pImageIn, int cbIn, ref IntPtr ppImageOut, ref int pcbOut, ref int pWidth, ref int pHeight, ref int pDPI);

		/// Return Type: int
		///width: int
		///height: int
		///DPI: int
		///depth: int
		///pImageIn: unsigned char*
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "RAWtoBMP")]
		internal static extern int RAWtoBMP(int width, int height, int DPI, int depth, IntPtr pImageIn, ref IntPtr ppImageOut, ref int pcbOut);

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///fRate: float
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "BMPtoWSQ")]
		internal static extern int BMPtoWSQ(IntPtr pImageIn, int cbIn, float fRate, ref IntPtr ppImageOut, ref int pcbOut);

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "WSQtoBMP")]
		internal static extern int WSQtoBMP(IntPtr pImageIn, int cbIn, ref IntPtr ppImageOut, ref int pcbOut);

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///nCompression: int
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "BMPtoJPG")]
		internal static extern int BMPtoJPG(IntPtr pImageIn, int cbIn, int nCompression, ref IntPtr ppImageOut, ref int pcbOut);

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "JPGtoBMP")]
		internal static extern int JPGtoBMP(IntPtr pImageIn, int cbIn, ref IntPtr ppImageOut, ref int pcbOut);

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///fRate: float
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "BMPtoJP2")]
		internal static extern int BMPtoJP2(IntPtr pImageIn, int cbIn, float fRate, ref IntPtr ppImageOut, ref int pcbOut);

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "JP2toBMP")]
		internal static extern int JP2toBMP(IntPtr pImageIn, int cbIn, ref IntPtr ppImageOut, ref int pcbOut);

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "BMPtoFX4")]
		internal static extern int BMPtoFX4(IntPtr pImageIn, int cbIn, ref IntPtr ppImageOut, ref int pcbOut);

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "FX4toBMP")]
		internal static extern int FX4toBMP(IntPtr pImageIn, int cbIn, ref IntPtr ppImageOut, ref int pcbOut);

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "PNGtoBMP")]
		internal static extern int PNGtoBMP(IntPtr pImageIn, int cbIn, ref IntPtr ppImageOut, ref int pcbOut);

		/// Return Type: int
		///pImageIn: unsigned char*
		///cbIn: int
		///ppImageOut: unsigned char**
		///pcbOut: int*
		[DllImport("OpenEBTS.dll", EntryPoint = "BMPtoPNG")]
		internal static extern int BMPtoPNG(IntPtr pImageIn, int cbIn, ref IntPtr ppImageOut, ref int pcbOut);

		#endregion Image conversion
	}
}
