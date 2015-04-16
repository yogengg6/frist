using ImageWare.CodeSigning;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace OpenEBTSNet
{
	internal class OpenEBTSInteropWrap
	{
		#region EBTS file related

		internal static SafeHandleTransaction Read(string filePath)
		{
			CheckSignatures();
			IntPtr transactionPtr = IntPtr.Zero;

			int errCode = OpenEBTSInterop.IWRead(filePath, IntPtr.Zero, ref transactionPtr);
			if (errCode != (int) OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWRead", errCode);
			}

			return new SafeHandleTransaction(transactionPtr);
		}

		internal static SafeHandleTransaction ReadFromMem(byte[] transactionBuffer)
		{
			CheckSignatures();
			IntPtr transactionPtr = IntPtr.Zero;
			IntPtr unmanagedPointer = IntPtr.Zero;

			try
			{
				unmanagedPointer = Marshal.AllocHGlobal(transactionBuffer.Length);
				Marshal.Copy(transactionBuffer, 0, unmanagedPointer, transactionBuffer.Length);

				int errCode = OpenEBTSInterop.IWReadMem(unmanagedPointer, transactionBuffer.Length, IntPtr.Zero, ref transactionPtr);

				if (errCode != (int) OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
				{
					throw new OpenEBTSException("IWReadMem", errCode);
				}
			}
			finally
			{
				if (unmanagedPointer != IntPtr.Zero)
				{
					Marshal.FreeHGlobal(unmanagedPointer);
				}
			}

			return new SafeHandleTransaction(transactionPtr);
		}

		internal static SafeHandleTransaction New(string TOT)
		{
			CheckSignatures();
			IntPtr transactionPtr = IntPtr.Zero;

			// Prepare a blank transaction. Don't pass in verification, it gets associated
			// explicitly with AssociateVerification.
			int errCode = OpenEBTSInterop.IWNew(TOT, IntPtr.Zero, ref transactionPtr);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWNew", errCode);
			}

			return new SafeHandleTransaction(transactionPtr);
		}

		internal static void Write(SafeHandleTransaction handle, string filePath)
		{
			CheckSignatures();
			int errCode = OpenEBTSInterop.IWWrite(handle.DangerousGetHandle(), filePath);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWWrite", errCode);
			}
		}

		internal static byte[] WriteMem(SafeHandleTransaction handle)
		{
			CheckSignatures();
			IntPtr dataPtr = IntPtr.Zero;

			try
			{
				int size = 0;
				int errCode = OpenEBTSInterop.IWWriteMem(handle.DangerousGetHandle(), ref dataPtr, ref size);

				if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
				{
					throw new OpenEBTSException("IWWriteMem", errCode);
				}

				byte[] transactionBuffer = new byte[size];
				Marshal.Copy(dataPtr, transactionBuffer, 0, transactionBuffer.Length);

				return transactionBuffer;
			}
			finally
			{
				if (dataPtr != IntPtr.Zero)
				{
					OpenEBTSInterop.IWMemFree(ref dataPtr);
				}
			}
		}

		internal static IntPtr Close(IntPtr transaction)
		{
			IntPtr retVal = transaction;

			int errCode = OpenEBTSInterop.IWClose(ref retVal);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWClose", errCode, null);
			}

			return retVal;
		}

		internal static void SetVerification(SafeHandleTransaction handle, SafeHandleVerification verification)
		{
			int errCode = OpenEBTSInterop.IWSetVerification(handle.DangerousGetHandle(), verification.DangerousGetHandle());

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWSetVerification", errCode);
			}
		}

		internal static string GetError(SafeHandleTransaction handle, int index)
		{
			int code = 0;
			IntPtr pszDesc;

			int errCode = OpenEBTSInterop.IWGetError(handle.DangerousGetHandle(), index, ref code, out pszDesc);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetError", errCode);
			}

			return Marshal.PtrToStringUni(pszDesc);
		}

		internal static bool Verify(SafeHandleTransaction handle, out List<string> errors)
		{
			errors = new List<string>();

			int errCode = OpenEBTSInterop.IWVerify(handle.DangerousGetHandle());

			bool ret = (errCode == (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS);

			if (!ret)
			{
				// If the error is just due to failed verification, there are probably associated error messages
				if (errCode == (int) OpenEBTSErrors.ErrorCodes.IW_WARN_TRANSACTION_FAILED_VERIFICATION)
				{
					int numErrors = OpenEBTSInterop.IWGetErrorCount(handle.DangerousGetHandle());
					for (int i = 0; i < numErrors; i++)
					{
						errors.Add(GetError(handle, i));
					}
				}
				else
				{
					throw new OpenEBTSException("IWVerify", errCode);
				}
			}

			return ret;
		}

		internal static int GetNumRecords(SafeHandleTransaction handle)
		{
			int numRecords = 0;

			int errCode = OpenEBTSInterop.IWGetNumRecords(handle.DangerousGetHandle(), ref numRecords);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetNumRecords", errCode);
			}

			return numRecords;
		}

		internal static string FindItem(SafeHandleTransaction handle, int recordType, int recordIndex, int fieldIndex, int subfieldIndex, int itemIndex)
		{
			IntPtr retVal = IntPtr.Zero;

			int errCode = OpenEBTSInterop.IWFindItem(handle.DangerousGetHandle(), recordType, recordIndex, fieldIndex, subfieldIndex, itemIndex, out retVal);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWFindItem", errCode);
			}

			return Marshal.PtrToStringUni(retVal);
		}

		internal static void SetItem(SafeHandleTransaction handle, int recordType, int recordIndex, int fieldIndex, int subfieldIndex, int itemIndex, string data)
		{
			int errCode = OpenEBTSInterop.IWSetItem(handle.DangerousGetHandle(), data, recordType, recordIndex, fieldIndex, subfieldIndex, itemIndex);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWSetItem", errCode);
			}
		}

		internal static int AddRecord(SafeHandleTransaction handle, int recordType)
		{
			int recordIndex = 0;

			int errCode = OpenEBTSInterop.IWAddRecord(handle.DangerousGetHandle(), recordType, ref recordIndex);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWAddRecord", errCode);
			}

			return recordIndex;
		}

		internal static void DeleteRecord(SafeHandleTransaction handle, int recordType, int recordIndex)
		{
			int errCode = OpenEBTSInterop.IWDeleteRecord(handle.DangerousGetHandle(), recordType, recordIndex);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWDeleteRecord", errCode);
			}
		}

		internal static void Set(SafeHandleTransaction handle, string mnemonic, int recordIndex, int secondaryIndex, string data)
		{
			int errCode = OpenEBTSInterop.IWSet(handle.DangerousGetHandle(), mnemonic, data,  secondaryIndex, recordIndex);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWSet", errCode);
			}
		}

		internal static bool TrySet(SafeHandleTransaction handle, string mnemonic, int recordIndex, int secondaryIndex, string data)
		{
			int errCode = OpenEBTSInterop.IWSet(handle.DangerousGetHandle(), mnemonic, data, secondaryIndex, recordIndex);

			return (errCode == (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS);
		}

		internal static void ImportImage(SafeHandleTransaction handle, int recordType, int recordIndex, string filePath,
			OpenEBTSImageFormat inputFormat, OpenEBTSImageFormat outputFormat, int compression)
		{
			string inFormat = ToFormatString(inputFormat);
			string outFormat = ToFormatString(outputFormat);

			int errCode = OpenEBTSInterop.IWImportImage(handle.DangerousGetHandle(), recordType, recordIndex, filePath, outFormat, compression, inFormat);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWImportImage", errCode);
			}
		}

		internal static void SetImage(SafeHandleTransaction handle, int recordType, int recordIndex, byte[] imageData,
			OpenEBTSImageFormat inputFormat, OpenEBTSImageFormat outputFormat, int compression)
		{
			string inFormat = ToFormatString(inputFormat);
			string outFormat = ToFormatString(outputFormat);
			IntPtr unmanagedPointer = IntPtr.Zero;

			try
			{
				unmanagedPointer = Marshal.AllocHGlobal(imageData.Length);
				Marshal.Copy(imageData, 0, unmanagedPointer, imageData.Length);

				int errCode = OpenEBTSInterop.IWSetImage(handle.DangerousGetHandle(), recordType, recordIndex, inFormat, imageData.Length, unmanagedPointer, outFormat, compression);

				if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
				{
					throw new OpenEBTSException("IWSetImage", errCode);
				}
			}
			finally
			{
				if (unmanagedPointer != IntPtr.Zero)
				{
					Marshal.FreeHGlobal(unmanagedPointer);
				}
			}
		}

		internal static void GetImageInfo(SafeHandleTransaction handle, int recordType, int recordIndex, out OpenEBTSImageFormat format, out int width, out int height, out int bpp)
		{
			IntPtr formatPtr = IntPtr.Zero;
			int byteCount = 0;
			width = 0;
			height = 0;
			bpp = 0;

			int errCode = OpenEBTSInterop.IWGetImageInfo(handle.DangerousGetHandle(), recordType, recordIndex, out formatPtr, ref byteCount, ref width, ref height, ref bpp);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetImageInfo", errCode);
			}

			string formatString = Marshal.PtrToStringUni(formatPtr);
			format = FromFormatString(formatString);
		}

		internal static byte[] GetImage(SafeHandleTransaction handle, int recordType, int recordIndex, out OpenEBTSImageFormat format)
		{
			IntPtr formatPtr = IntPtr.Zero;
			IntPtr dataPtr = IntPtr.Zero;
			int size = 0;

			int errCode = OpenEBTSInterop.IWGetImage(handle.DangerousGetHandle(), recordType, recordIndex, ref formatPtr, ref size, ref dataPtr);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetImage", errCode);
			}

			string formatString = Marshal.PtrToStringUni(formatPtr);
			format = FromFormatString(formatString);

			byte[] data = new byte[size];
			Marshal.Copy(dataPtr, data, 0, data.Length);

			// Note: dataPtr doe not need to be released, it belongs to OpenEBTS.dll and is just a pointer to an image within

			return data;
		}

		internal static byte[] ExportImageMem(SafeHandleTransaction handle, int recordType, int recordIndex, OpenEBTSImageFormat format)
		{
			IntPtr dataPtr = IntPtr.Zero;

			try
			{
				string formatstring = ToFormatString(format);
				int size = 0;

				int errCode = OpenEBTSInterop.IWExportImageMem(handle.DangerousGetHandle(), recordType, recordIndex, ref dataPtr, ref size, formatstring);

				if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
				{
					throw new OpenEBTSException("IWExportImageMem", errCode);
				}

				byte[] data = new byte[size];
				Marshal.Copy(dataPtr, data, 0, data.Length);

				// Note: dataPtr doe not need to be released, it belongs to OpenEBTS.dll and is just a pointer to an image within

				return data;
			}
			finally
			{
				if (dataPtr != IntPtr.Zero)
				{
					OpenEBTSInterop.IWMemFree(ref dataPtr);
				}
			}
		}

		internal static void ExportImage(SafeHandleTransaction handle, int recordType, int recordIndex, string filePath, OpenEBTSImageFormat format)
		{
			string formatstring = ToFormatString(format);

			int errCode = OpenEBTSInterop.IWExportImage(handle.DangerousGetHandle(), recordType, recordIndex, filePath, formatstring);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWExportImage", errCode);
			}
		}

		internal static string ToFormatString(OpenEBTSImageFormat fmt)
		{
			switch (fmt)
			{
				case OpenEBTSImageFormat.imageFormatRAW:	return "RAW";
				case OpenEBTSImageFormat.imageFormatBMP:	return "BMP";
				case OpenEBTSImageFormat.imageFormatJPG:	return "JPG";
				case OpenEBTSImageFormat.imageFormatWSQ:	return "WSQ";
				case OpenEBTSImageFormat.imageFormatJP2:	return "JP2";
				case OpenEBTSImageFormat.imageFormatPNG:	return "PNG";
				case OpenEBTSImageFormat.imageFormatFX4:	return "FX4";
				case OpenEBTSImageFormat.imageFormatCBEFF:	return "CBEFF";
				default:									return "?";	// shouldn't happen
			}
		}

		internal static OpenEBTSImageFormat FromFormatString(string fmt)
		{
			switch (fmt.ToUpper())
			{
				case "RAW":		return OpenEBTSImageFormat.imageFormatRAW;
				case "BMP":		return OpenEBTSImageFormat.imageFormatBMP;
				case "JPG":		return OpenEBTSImageFormat.imageFormatJPG;
				case "WSQ":		return OpenEBTSImageFormat.imageFormatWSQ;
				case "JP2":		return OpenEBTSImageFormat.imageFormatJP2;
				case "PNG":		return OpenEBTSImageFormat.imageFormatPNG;
				case "FX4":		return OpenEBTSImageFormat.imageFormatFX4;
				case "CBEFF":	return OpenEBTSImageFormat.imageFormatCBEFF;
				default:
					return OpenEBTSImageFormat.imageFormatRAW; //  shouldn't happen
			}
		}

		internal static string Get(SafeHandleTransaction handle, string mnemonic, int recordIndex, int secondaryIndex)
		{
			IntPtr retVal = IntPtr.Zero;

			int errCode = OpenEBTSInterop.IWGet(handle.DangerousGetHandle(), mnemonic, ref retVal, secondaryIndex, recordIndex);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGet", errCode);
			}

			return Marshal.PtrToStringUni(retVal);
		}

		internal static bool TryGet(SafeHandleTransaction handle, string mnemonic, int recordIndex, int secondaryIndex, out string data)
		{
			IntPtr retVal = IntPtr.Zero;

			int errCode = OpenEBTSInterop.IWGet(handle.DangerousGetHandle(), mnemonic, ref retVal, secondaryIndex, recordIndex);

			if (errCode == (int) OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				data = Marshal.PtrToStringUni(retVal);
				return true;
			}
			if (errCode == (int) OpenEBTSErrors.ErrorCodes.IW_ERR_MNEMONIC_NOT_FOUND ||
				errCode == (int) OpenEBTSErrors.ErrorCodes.IW_ERR_RECORD_NOT_FOUND)
			{
				data = "";
				return false;
			}
			else
			{
				throw new OpenEBTSException("IWGet", errCode);
			}
		}

		internal static int GetRecordTypeCount(SafeHandleTransaction handle, int recordType)
		{
			int recordTypeCount = 0;

			int errCode = OpenEBTSInterop.IWGetRecordTypeCount(handle.DangerousGetHandle(), recordType, ref recordTypeCount);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetRecordTypeCount", errCode);
			}

			return recordTypeCount;
		}

		internal static int GetNumFields(SafeHandleTransaction handle, int recordType, int recordIndex)
		{
			int fieldCount = 0;

			int errCode = OpenEBTSInterop.IWGetNumFields(handle.DangerousGetHandle(), recordType, recordIndex, ref fieldCount);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetNumFields", errCode);
			}

			return fieldCount;
		}

		internal static int GetNextField(SafeHandleTransaction handle, int recordType, int recordIndex, int fieldIndex)
		{
			int nextFieldIndex = 0;

			int errCode = OpenEBTSInterop.IWGetNextField(handle.DangerousGetHandle(), recordType, recordIndex, fieldIndex, ref nextFieldIndex);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetNextField", errCode);
			}

			return nextFieldIndex;			
		}

		internal static int NumSubfields(SafeHandleTransaction handle, int recordType, int recordIndex, int fieldIndex)
		{
			int subfieldCount = 0;

			int errCode = OpenEBTSInterop.IWNumSubfields(handle.DangerousGetHandle(), recordType, recordIndex, fieldIndex, ref subfieldCount);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWNumSubfields", errCode);
			}

			return subfieldCount;
		}

		internal static int NumItems(SafeHandleTransaction handle, int recordType, int recordIndex, int fieldIndex, int subfieldIndex)
		{
			int itemCount = 0;

			int errCode = OpenEBTSInterop.IWNumItems(handle.DangerousGetHandle(), recordType, recordIndex, fieldIndex, subfieldIndex, ref itemCount);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWNumItems", errCode);
			}

			return itemCount;
		}

		#endregion EBTS file related

		#region Verification file related

		internal static SafeHandleVerification ReadVerification(string filePath)
		{
			CheckSignatures();
			const int maxChars = 1024;
			StringBuilder parseError = new StringBuilder(maxChars);
			IntPtr verificationPtr = IntPtr.Zero;

			int errCode = OpenEBTSInterop.IWReadVerification(filePath, ref verificationPtr, maxChars, parseError);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWReadVerification", errCode, parseError.ToString());
			}

			return new SafeHandleVerification(verificationPtr);
		}

		internal static IntPtr CloseVerification(IntPtr verification)
		{
			IntPtr retVal = verification;
			
			int errCode = OpenEBTSInterop.IWCloseVerification(ref retVal);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWCloseVerification", errCode, null);
			}

			return retVal;
		}

		internal static string[] GetTransactionCategories(SafeHandleVerification handle)
		{
			int numCategories = 0;

			int errCode = OpenEBTSInterop.IWGetTransactionCategories(handle.DangerousGetHandle(), 0, null, ref numCategories);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetTransactionCategories", errCode);
			}

			IntPtr[] categories = new IntPtr[numCategories];

			errCode = OpenEBTSInterop.IWGetTransactionCategories(handle.DangerousGetHandle(), numCategories, categories, ref numCategories);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetTransactionCategories", errCode);
			}

			string[] retVal = new string[numCategories];
			for (int i = 0; i < numCategories; i++)
			{
				retVal[i] = Marshal.PtrToStringUni(categories[i]);
			}

			return retVal;
		}

		internal static Tuple<string, string>[] GetTransactionTypes(SafeHandleVerification handle, string transactionCategoryName)
		{
			const int maxTransactionsPerCategory = 256;
			IntPtr[] transactionDataArray = new IntPtr[maxTransactionsPerCategory];
			IntPtr[] transactionDescArray = new IntPtr[maxTransactionsPerCategory];
			int numTransactionTypes = 0;

			int errCode = OpenEBTSInterop.IWGetTransactionTypes(handle.DangerousGetHandle(), maxTransactionsPerCategory, transactionDataArray, transactionDescArray, ref numTransactionTypes, transactionCategoryName);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetTransactionTypes", errCode);
			}

			Tuple<string, string>[] retVal = new Tuple<string, string>[numTransactionTypes];
			for (int i = 0; i < numTransactionTypes; i++)
			{
				retVal[i] = new Tuple<string, string>(Marshal.PtrToStringUni(transactionDataArray[i]), Marshal.PtrToStringUni(transactionDescArray[i]));
			}

			return retVal;
		}

		internal static Tuple<int[], int[], int[]> GetRecordTypeOccurrences(SafeHandleVerification handle, string TOT)
		{
			int numEntries = 0;

			int errCode = OpenEBTSInterop.IWGetRecordTypeOccurrences(handle.DangerousGetHandle(), 0, null, null, null, ref numEntries, TOT);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetRecordTypeOccurrences", errCode);
			}

			int[] recordType = new int[numEntries];
			int[] minOccurences = new int[numEntries];
			int[] maxOccurences = new int[numEntries];

			errCode = OpenEBTSInterop.IWGetRecordTypeOccurrences(handle.DangerousGetHandle(), numEntries, recordType, minOccurences, maxOccurences, ref numEntries, TOT);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetRecordTypeOccurrences", errCode);
			}

			return new Tuple<int[], int[], int[]>(recordType, minOccurences, maxOccurences);
		}

		static internal Tuple<string, string>[] GetMnemonics(SafeHandleVerification handle, string transactionName)
		{
			int numFields = 0;

			int errCode = OpenEBTSInterop.IWGetMnemonics(handle.DangerousGetHandle(), transactionName, 0, null, null, ref numFields);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetMnemonics", errCode);
			}

			IntPtr[] fieldsDataArray = new IntPtr[numFields];
			IntPtr[] fieldsDescArray = new IntPtr[numFields];

			errCode = OpenEBTSInterop.IWGetMnemonics(handle.DangerousGetHandle(), transactionName, numFields, fieldsDataArray, fieldsDescArray, ref numFields);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetMnemonics", errCode);
			}

			Tuple<string, string>[] retVal = new Tuple<string, string>[numFields];

			for (int i = 0; i < numFields; i++)
			{
				retVal[i] = new Tuple<string, string>(Marshal.PtrToStringUni(fieldsDataArray[i]), Marshal.PtrToStringUni(fieldsDescArray[i]));
			}

			return retVal;
		}

		internal static OpenEBTSFieldRules GetRuleRestrictions(SafeHandleVerification handle, string transactionName, string fieldDefinitionMnemonic)
		{
			IntPtr pDesc;
			IntPtr pLongDesc;
			IntPtr pCharType;
			IntPtr pSpecialChars;
			IntPtr pDateFormat;
			IntPtr pAvancedRule;
			OpenEBTSFieldRules retVal = new OpenEBTSFieldRules();

			int errCode = OpenEBTSInterop.IWGetRuleRestrictions(handle.DangerousGetHandle(), transactionName, fieldDefinitionMnemonic,
				ref retVal.recordType, ref retVal.field, ref retVal.subfield, ref retVal.item,
				out pDesc, out pLongDesc, out pCharType, out pSpecialChars, out pDateFormat, out pAvancedRule,
				ref retVal.sizeMin, ref retVal.sizeMax, ref retVal.occMin, ref retVal.occMax,
				ref retVal.offset, ref retVal.automaticallySet, ref retVal.mandatory);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetRuleRestrictions", errCode);
			}

			retVal.desc = Marshal.PtrToStringUni(pDesc);
			retVal.longDesc = Marshal.PtrToStringUni(pLongDesc);
			retVal.charType = Marshal.PtrToStringUni(pCharType);
			retVal.specialChars = Marshal.PtrToStringUni(pSpecialChars);
			retVal.dateFormat = Marshal.PtrToStringUni(pDateFormat);
			retVal.advancedRule = Marshal.PtrToStringUni(pAvancedRule);

			return retVal;
		}

		internal static string GetLocactionIndex(SafeHandleVerification handle, string transactionName, string fieldDefinitionMnemonic)
		{
			IntPtr pLocationIndex;

			int retVal = OpenEBTSInterop.IWGetLocationIndex(handle.DangerousGetHandle(), transactionName, fieldDefinitionMnemonic, out pLocationIndex);

			if (retVal != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetLocationIndex", retVal);
			}

			return Marshal.PtrToStringUni(pLocationIndex);
		}

		internal static Tuple<bool, Tuple<string, string>[]> GetValueList(SafeHandleVerification handle, string transactionType, string fieldDefinitionMnemonic)
		{
			int numValues = 0;
			int mandatory = 0;

			int errCode = OpenEBTSInterop.IWGetValueList(handle.DangerousGetHandle(), transactionType, fieldDefinitionMnemonic, ref mandatory, 0, null, null, ref numValues);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetValueList", errCode);
			}

			IntPtr[] fieldsValuesNameArray = new IntPtr[numValues];
			IntPtr[] fieldsValuesDescArray = new IntPtr[numValues];

			errCode = OpenEBTSInterop.IWGetValueList(handle.DangerousGetHandle(), transactionType, fieldDefinitionMnemonic, ref mandatory, numValues, fieldsValuesNameArray, fieldsValuesDescArray, ref numValues);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetValueList", errCode);
			}

			Tuple<string, string>[] valueTuples = new Tuple<string, string>[numValues];
			Tuple<bool, Tuple<string, string>[]> retVal = new Tuple<bool, Tuple<string, string>[]>(mandatory == 0 ? false : true, valueTuples);

			for (int i = 0; i < numValues; i++)
			{
				valueTuples[i] = new Tuple<string, string>(Marshal.PtrToStringUni(fieldsValuesNameArray[i]), Marshal.PtrToStringUni(fieldsValuesDescArray[i]));
			}

			return retVal;
		}

		internal static string GetValueListFilename(SafeHandleVerification handle, string transactionType, string fieldDefinitionMnemonic)
		{
			IntPtr filename;

			int errCode = OpenEBTSInterop.IWGetValueListFilename(handle.DangerousGetHandle(), transactionType, fieldDefinitionMnemonic, out filename);

			if (errCode != (int)OpenEBTSErrors.ErrorCodes.IW_SUCCESS)
			{
				throw new OpenEBTSException("IWGetValueListFilename", errCode);
			}

			return Marshal.PtrToStringUni(filename);
		}

		#endregion Verification file related

		#region Code Signing

		private static bool signatureCheckPassed = false;

		private static void CheckSignatures()
		{
#if SIG_CHECK
			if (signatureCheckPassed)
			{
				return;
			}
			CheckSignature("OpenEBTS.dll");
			CheckSignature("FreeImage.dll");
			signatureCheckPassed = true;
#endif
		}

		private static void CheckSignature(string module)
		{
			AuthenticodeVerifier verifier = new AuthenticodeVerifier();
			string path = GetLibraryPath(module);
			if (string.IsNullOrEmpty(path))
			{
				throw new OpenEBTSException("CheckSiginature", -1, "File OpenEBTS.dll was not found");
			}
			verifier.Initialize(path, false);
			if (!(verifier.WinVerifySignature() && verifier.ChainIsValid))
			{
				throw new OpenEBTSException("CheckSignature", -1, "An integrity error occurred while loading OpenEBTS.dll;.  This is likely caused by an unauthorized modification of this file. Please contact your system administrator and perform a re-install of this application.");
			}
		}

		private static string GetLibraryPath(string dllName)
		{
			string retVal = null;

			IntPtr lib = OpenEBTSInterop.LoadLibrary(dllName);

			if (lib != IntPtr.Zero)
			{
				try
				{
					StringBuilder sb = new StringBuilder(256);
					OpenEBTSInterop.GetModuleFileName(lib, sb, 256);
					retVal = sb.ToString();
				}
				finally
				{
					OpenEBTSInterop.FreeLibrary(lib);
				}
			}
			else
			{
				return null;
			}
			return retVal;
		}
		#endregion
	}
}
