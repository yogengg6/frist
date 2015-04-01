using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;

namespace OpenEBTSNet
{
	/// <summary>
	/// NIST Transaction File wrapper that supports the ANSI/NIST-ITL standard, &quot;Data Format 
	/// for the Interchange of Fingerprint, Facial &amp; Other Biometric Information&quot;.
	/// </summary>
	public class OpenEBTSFile : IDisposable
	{
		internal SafeHandleTransaction Handle = new SafeHandleTransaction(IntPtr.Zero);

		public void Dispose()
		{
			Free();
		}

		private void Free()
		{
			Handle.Dispose();
		}

		/// <summary>
		/// Loads a NIST Transaction File
		/// </summary>
		/// <param name="filePath">Path of file to load</param>
		/// <seealso cref="LoadFromMem"/>
		/// <returns>OpenEBTS error code</returns>
		public void LoadFromFile(string filePath)
		{
			Free();

			Handle = OpenEBTSInteropWrap.Read(filePath);
			// Set our TransactionType property. It's always at index 4 of the first (and only) Type-1 record.
			TransactionType = OpenEBTSInteropWrap.FindItem(Handle, 1, 1, 4, 1, 1);
		}

		/// <summary>
		/// Loads an in-memory NIST Transaction File
		/// </summary>
		/// <param name="transactionFile">Transaction File as a byte array</param>
		/// <seealso cref="LoadFromFile"/>
		public void LoadFromMem(byte[] transactionBuffer)
		{
			Free();

			Handle = OpenEBTSInteropWrap.ReadFromMem(transactionBuffer);
			// Set our TransactionType property. It's always at index 4 of the first (and only) Type-1 record.
			TransactionType = OpenEBTSInteropWrap.FindItem(Handle, 1, 1, 4, 1, 1);
		}

		/// <summary>
		/// Initializes a new Transaction File and sets its Type of Transaction
		/// </summary>
		/// <param name="TOT">Type of Transaction</param>
		/// <remarks>Any previous transaction data will no longer be accessible</remarks>
		public void New(string TOT)
		{
			Free();

			Handle = OpenEBTSInteropWrap.New(TOT);
			TransactionType = TOT; // Save to property			
		}

		/// <summary>
		/// Outputs the Transaction File contents to the specified path
		/// </summary>
		/// <param name="filePath">Path of Transaction File to create</param>
		public void WriteToFile(string filePath)
		{
			OpenEBTSInteropWrap.Write(Handle, filePath);
		}

		/// <summary>
		/// Outputs the Transaction File contents to memory
		/// </summary>
		/// <returns>Transaction File contents</returns>
		public byte[] WriteToMem()
		{
			return OpenEBTSInteropWrap.WriteMem(Handle);
		}

		/// <summary>
		/// Associates the verification file with the transaction file, enabling referencing the data within
		/// via mnemonics and perfoming verification
		/// </summary>
		/// <param name="verification">OpenEBTSVerification object</param>
		/// <remarks>Before calling Verify the OpenEBTSVerification object must contain a valid Verification File,
		/// which is done by calling LoadFromFile.
		/// <para>
		/// This is the preferred way to associate a Verification File because it allows checking for any errors 
		/// in the Verification File itself, which can happen if in the process of producing a customized 
		/// Verification File.
		/// </para>
		/// </remarks>
		/// <seealso cref="OpenEBTSVerification"/>
		public void AssociateVerification(OpenEBTSVerification verification)
		{
			OpenEBTSInteropWrap.SetVerification(Handle, verification.Handle);
		}

		/// <summary>
		/// Verifies the Transaction File's contents against a pre-loaded Verification File. 
		/// In the case of failure, produces a list of errors accessible via GetError and 
		/// ErrorCount.
		/// </summary>
		/// <returns>True if NIST file passes verification, false otherwise</returns>
		/// <remarks>Note that before calling Verify, a Verification File containing all the rules must be associated 
		/// with this NIST Transaction File via AssociateVerification</remarks>
		/// <seealso cref="AssociateVerification"/>
		/// <seealso cref="VerifyErrors"/>
		public bool Verify()
		{
			List<string> verifyErrors;

			bool ret = OpenEBTSInteropWrap.Verify(Handle, out verifyErrors);
			VerifyErrors = verifyErrors;

			return ret;
		}

		/// <summary>
		/// Returns EBTS file verifciation errors as a list of error description string
		/// </summary>
		/// <returns>Error string</returns>
		/// <remarks>The index is 0-based, going from 0 to ErrorCount-1</remarks>
		/// <seealso cref="Verify"/>
		public List<string> VerifyErrors { get; private set; }


		/// <summary>
		/// Type of transaction, commonly referred to as "TOT"
		/// </summary>
		public string TransactionType { get; private set; }

		/// <summary>
		/// Adds a NIST record of the specified Record Type to this NIST Transaction File
		/// </summary>
		/// <param name="recordType">Record Type to add</param>
		/// <returns>
		/// Record index within type (not within the file) of the newly added record. 
		/// The value is effectively the new count of records of this type as indexes
		/// are numbered starting at 1.
		/// </returns>
		/// <remarks>
		/// The returned index can be used in Get/SetDataViaMnemonic and methods 
		/// where the record type and index are supplied; for example, DeleteRecord, GetImage, etc.
		/// </remarks>
		/// <seealso cref="GetRecord"/>
		public int AddRecord(int recordType)
		{
			return OpenEBTSInteropWrap.AddRecord(Handle, recordType);
		}

		/// <summary>
		/// Removes a record from the Transaction File
		/// </summary>
		/// <param name="recordType">Record Type of record to remove</param>
		/// <param name="recordIndex">Index of this Record Type to remove</param>
		public void DeleteRecord(int recordType, int recordIndex)
		{
			OpenEBTSInteropWrap.DeleteRecord(Handle, recordType, recordIndex);
		}

		/// <summary>
		/// Retrieves textual data from the Transaction File by referring to an item by its hierarchical
		/// set of idexes
		/// </summary>
		/// <param name="recordType">Type of EBTS record</param>
		/// <param name="recordIndex">1-based index of record</param>
		/// <param name="fieldIndex">1-based field index</param>
		/// <param name="subfieldIndex">1-based subfield index</param>
		/// <param name="itemIndex">1-based item index</param>
		/// <returns>Text at the specified location in the EBTS file</returns>
		/// <seealso cref="SetDataViaIndexes"/>
		/// <seealso cref="SetDataViaMnemonic"/>
		/// <seealso cref="TrySetDataViaMnemonic"/>
		/// <seealso cref="GetDataViaMnemonic"/>
		/// <seealso cref="TryGetDataViaMnemonic"/>
		public string GetDataViaIndexes(int recordType, int recordIndex, int fieldIndex, int subfieldIndex, int itemIndex)
		{
			return OpenEBTSInteropWrap.FindItem(Handle, recordType, recordIndex, fieldIndex, subfieldIndex, itemIndex);
		}

		/// <summary>
		/// Retrieves textual data from the Transaction File by referring to an item by its mnemonic, as 
		/// defined in its associated Verification File
		/// </summary>
		/// <param name="mnemonic">Mnemonic</param>
		/// <param name="recordIndex">Record index for that particular Record Type</param>
		/// <param name="secondaryIndex">Field Index, or depending on the mnemonic, Subitem or Item index</param>
		/// <returns>Data at the specified location</returns>
		/// <remarks>To use mnemonics to refer to items in the Transaction File a Verification File must first 
		/// be associated with the Transaction File</remarks>
		/// <seealso cref="AssociateVerification"/>
		/// <seealso cref="SetDataViaIndexes"/>
		/// <seealso cref="SetDataViaMnemonic"/>
		/// <seealso cref="TrySetDataViaMnemonic"/>
		/// <seealso cref="GetDataViaIndexes"/>
		/// <seealso cref="TryGetDataViaMnemonic"/>
		public string GetDataViaMnemonic(string mnemonic, int recordIndex, int secondaryIndex)
		{
			return OpenEBTSInteropWrap.Get(Handle, mnemonic, recordIndex, secondaryIndex);
		}


		/// <summary>
		/// Same function as GetDataViaMnemonic but returns false if either the mnemonic or the record does not exist
		/// </summary>
		/// <param name="mnemonic">Mnemonic</param>
		/// <param name="recordIndex">Record index for that particular Record Type</param>
		/// <param name="secondaryIndex">Field Index, or depending on the mnemonic, Subitem or Item index</param>
		/// <param name="data">Data is returned here on success</param>
		/// <returns>True if the data was obtained</returns>
		/// <remarks>To use mnemonics to refer to items in the Transaction File a Verification File must first 
		/// be associated with the Transaction File</remarks>
		/// <seealso cref="AssociateVerification"/>
		/// <seealso cref="SetDataViaIndexes"/>
		/// <seealso cref="SetDataViaMnemonic"/>
		/// <seealso cref="TrySetDataViaMnemonic"/>
		/// <seealso cref="GetDataViaIndexes"/>
		/// <seealso cref="GetDataViaMnemonic"/>
		public bool TryGetDataViaMnemonic(string mnemonic, int recordIndex, int secondaryIndex, out string data)
		{
			return OpenEBTSInteropWrap.TryGet(Handle, mnemonic, recordIndex, secondaryIndex, out data);
		}

		/// <summary>
		/// Retrieves the image from the NIST Transaction File as-is, into a byte array
		/// </summary>
		/// <param name="recordType">Record Type of image</param>
		/// <param name="recordIndex">Index into records of this Record Type</param>
		/// <param name="format">Returned format of image</param>
		/// <param name="width">Returned width of image</param>
		/// <param name="height">Returned height of image</param>
		/// <param name="bitDepth">Returned bit depth of image</param>
		/// <returns>Image data</returns>
		/// <seealso cref="SetImageFromFile"/>
		/// <seealso cref="SetImageFromMem"/>
		/// <seealso cref="SetImage"/>
		/// <seealso cref="GetImageToFile"/>
		/// <seealso cref="GetImage"/>
		public byte[] GetImageToMem(int recordType, int recordIndex, out OpenEBTSImageFormat format,
			out int width, out int height, out int bitDepth)
		{
			OpenEBTSInteropWrap.GetImageInfo(Handle, recordType, recordIndex, out format, out width, out height, out bitDepth);
			return OpenEBTSInteropWrap.GetImage(Handle, recordType, recordIndex, out format);
		}

		/// <summary>
		/// Returns an image from the NIST Transaction File
		/// </summary>
		/// <param name="recordType">Record Type of the image</param>
		/// <param name="recordIndex">Index into records of this Record Type</param>
		/// <returns>Image object wrapping the NIST image</returns>
		/// <seealso cref="SetImageFromFile"/>
		/// <seealso cref="SetImageFromMem"/>
		/// <seealso cref="SetImage"/>
		/// <seealso cref="GetImageToFile"/>
		/// <seealso cref="GetImageToMem"/>
		public Image GetImage(int recordType, int recordIndex)
		{
			byte[] imageData = OpenEBTSInteropWrap.ExportImageMem(Handle, recordType, recordIndex, OpenEBTSImageFormat.imageFormatBMP);

			MemoryStream ms = new System.IO.MemoryStream(imageData);
			return Image.FromStream(ms);
		}

		/// <summary>
		/// Retrieves an image from the NIST Transaction File and writes its contents to a file
		/// </summary>
		/// <param name="recordType">Record Type of image</param>
		/// <param name="recordIndex">Index into records of this Record Type</param>
		/// <param name="filePath">Path of destination image file</param>
		/// <param name="format">Format to output image in</param>
		/// <seealso cref="SetImageFromFile"/>
		/// <seealso cref="SetImageFromMem"/>
		/// <seealso cref="SetImage"/>
		/// <seealso cref="GetImageToMem"/>
		/// <seealso cref="GetImage"/>
		public void GetImageToFile(int recordType, int recordIndex, string filePath, OpenEBTSImageFormat format)
		{
			OpenEBTSInteropWrap.ExportImage(Handle, recordType, recordIndex, filePath, format);
		}

		/// <summary>
		/// Number of NIST records in this NIST Transaction File
		/// </summary>
		public int RecordCount
		{
			get
			{
				return OpenEBTSInteropWrap.GetNumRecords(Handle);
			}
		}

		/// <summary>
		/// Retrieves the record at location index
		/// </summary>
		/// <param name="index">Index into set of all Transaction File records</param>
		/// <returns>OpenEBTSRecord object describing record</returns>
		/// <remarks>
		/// The index is 1-based, going from 1 to RecordCount.
		/// <para>
		/// Note that this index is not the same index commonly referred to as Record Index. This index 
		/// traverses all records in the Transaction File, whereas the Record Index is the index of all 
		/// records of the same Record Type.
		/// </para>
		/// </remarks>
		public OpenEBTSRecord GetRecord(int index)
		{
			int currIndex = 0;
			bool found = false;
			int recordType = 0;
			int recordIndex = 0;

			// Since OpenEBTS does not have a way to index ALL records from 1 to N, we re-index them using
			// the existing RecordType/RecordIndex structure. Not the most elegant solution but in the
			// realm of a 'handful' or records this is a valid in the nameof more elegant calling code.

			for (recordType = 1; recordType < 100; recordType++)
			{
				int numRecords = GetRecordCountOfType(recordType);
				for (recordIndex = 1; recordIndex <= numRecords; recordIndex++)
				{
					currIndex++; // increment external global record index
					if (currIndex == index)
					{
						// We got a hit, the global index "index" specifies
						// the record of type "recordType" at record index "recordIndex"
						found = true;
						break;
					}
				}

				if (found) break;
			}

			if (!found)
			{
				throw new IndexOutOfRangeException();
			}

			return GetRecordOfType(recordType, recordIndex);
		}

		/// <summary>
		/// Number of records of the specified Record Type
		/// </summary>
		/// <param name="recordType">Record Type of desired record count</param>
		/// <returns>Number of records</returns>
		public int GetRecordCountOfType(int recordType)
		{
			return OpenEBTSInteropWrap.GetRecordTypeCount(Handle, recordType);
		}

		/// <summary>
		/// Retrieves the record of the specified Record Type at the specified index into record of that Record Type
		/// </summary>
		/// <param name="recordType">Record Type of record desired</param>
		/// <param name="recordIndex">Index into records of this Record Type</param>
		/// <returns>OpenEBTSRecord object describing record</returns>
		/// <remarks>recordIndex must be between 1 and GetRecordCountOfType(recordType)</remarks>
		public OpenEBTSRecord GetRecordOfType(int recordType, int recordIndex)
		{
			int numRecordsOfType = GetRecordCountOfType(recordType);
			if (recordIndex >= 1 && recordIndex <= numRecordsOfType)
			{
				return new OpenEBTSRecord(this, recordType, recordIndex);
			}
			else
			{
				throw new IndexOutOfRangeException();
			}
		}

		/// <summary>
		/// Sets textual data to the Transaction File by referring to an item by its hierarchical
		/// set of idexes
		/// </summary>
		/// <param name="recordType">Type of EBTS record</param>
		/// <param name="recordIndex">1-based index of record</param>
		/// <param name="fieldIndex">1-based field index</param>
		/// <param name="subfieldIndex">1-based subfield index</param>
		/// <param name="itemIndex">1-based item index</param>
		/// <param name="itemIndex">Text to set</param>
		/// <seealso cref="SetDataViaMnemonic"/>
		/// <seealso cref="TrySetDataViaMnemonic"/>
		/// <seealso cref="GetDataViaIndexes"/>
		/// <seealso cref="GetDataViaMnemonic"/>
		/// <seealso cref="TryGetDataViaMnemonic"/>
		public void SetDataViaIndexes(int recordType, int recordIndex, int fieldIndex, int subfieldIndex, int itemIndex, string data)
		{
			OpenEBTSInteropWrap.SetItem(Handle, recordType, recordIndex, fieldIndex, subfieldIndex, itemIndex, data);
		}

		/// <summary>
		/// Places textual data into the Transaction File item described by a mnemonic
		/// </summary>
		/// <param name="mnemonic">Mnemonic</param>
		/// <param name="recordIndex">Record index for that particular Record Type</param>
		/// <param name="secondaryIndex">Field Index, or depending on the mnemonic, Subitem or Item index</param>
		/// <param name="data">Text to place in the Transaction File</param>
		/// <remarks>To use mnemonics to refer to items in the Transaction File a Verification File must first 
		/// be associated with the Transaction File</remarks>
		/// <seealso cref="AssociateVerification"/>
		/// <seealso cref="SetDataViaIndexes"/>
		/// <seealso cref="TrySetDataViaMnemonic"/>
		/// <seealso cref="GetDataViaIndexes"/>
		/// <seealso cref="GetDataViaMnemonic"/>
		/// <seealso cref="TryGetDataViaMnemonic"/>
		public void SetDataViaMnemonic(string mnemonic, int recordIndex, int secondaryIndex, string data)
		{
			OpenEBTSInteropWrap.Set(Handle, mnemonic, recordIndex, secondaryIndex, data);
		}

		/// <summary>
		/// Places textual data into the Transaction File item described by a mnemonic. Unlike SetDataViaMnemonic
		/// this version of the function will not throw an exception on failure but simply return false
		/// </summary>
		/// <param name="mnemonic">Mnemonic</param>
		/// <param name="recordIndex">Record index for that particular Record Type</param>
		/// <param name="secondaryIndex">Field Index, or depending on the mnemonic, Subitem or Item index</param>
		/// <param name="data">Text to place in the Transaction File</param>
		/// <remarks>To use mnemonics to refer to items in the Transaction File a Verification File must first 
		/// be associated with the Transaction File</remarks>
		/// <seealso cref="AssociateVerification"/>
		/// <seealso cref="SetDataViaIndexes"/>
		/// <seealso cref="SetDataViaMnemonic"/>
		/// <seealso cref="GetDataViaIndexes"/>
		/// <seealso cref="GetDataViaMnemonic"/>
		/// <seealso cref="TryGetDataViaMnemonic"/>
		public bool TrySetDataViaMnemonic(string mnemonic, int recordIndex, int secondaryIndex, string data)
		{
			return OpenEBTSInteropWrap.TrySet(Handle, mnemonic, recordIndex, secondaryIndex, data);
		}

		// Support overloaded version to allow choice
		/// <summary>
		/// Places an image into the NIST Transaction File
		/// </summary>
		/// <param name="recordType">Record Type of image</param>
		/// <param name="recordIndex">Index into records of this Record Type</param>
		/// <param name="image">Image to place in Transaction File</param>
		/// <param name="outputFormat">Desired image format inside Transaction File</param>
		/// <param name="compression">Desired image compression, if it applies</param>
		/// <seealso cref="SetImageFromFile"/>
		/// <seealso cref="SetImageFromMem"/>
		/// <seealso cref="GetImageToFile"/>
		/// <seealso cref="GetImageToMem"/>
		/// <seealso cref="GetImage"/>
		public void SetImage(int recordType, int recordIndex, Image image, OpenEBTSImageFormat outputFormat, int compression)
		{
			// For simplicity we always convert the input to BMP first
			byte[] imageData;

			MemoryStream ms = new MemoryStream();
			image.Save(ms, System.Drawing.Imaging.ImageFormat.Bmp);
			imageData = ms.ToArray();

			SetImageFromMem(recordType, recordIndex, imageData, OpenEBTSImageFormat.imageFormatBMP, outputFormat, compression);
		}

		/// <summary>
		/// Places an image from file into the Transaction File
		/// </summary>
		/// <param name="recordType">Record Type of image</param>
		/// <param name="recordIndex">Index into records of this Record Type</param>
		/// <param name="filePath">Path of source image file</param>
		/// <param name="inputFormat">Specifies format of source image file</param>
		/// <param name="outputFormat">Desired image format inside Transaction File</param>
		/// <param name="compression">Desired image compression, if it applies</param>
		/// <seealso cref="SetImageFromMem"/>
		/// <seealso cref="SetImage"/>
		/// <seealso cref="GetImageToFile"/>
		/// <seealso cref="GetImageToMem"/>
		/// <seealso cref="GetImage"/>
		public void SetImageFromFile(int recordType, int recordIndex, string filePath,
			OpenEBTSImageFormat inputFormat, OpenEBTSImageFormat outputFormat, int compression)
		{
			OpenEBTSInteropWrap.ImportImage(Handle, recordType, recordIndex, filePath, inputFormat, outputFormat, compression);
		}

		/// <summary>
		/// Places an image from memory into the Transaction File
		/// </summary>
		/// <param name="recordType">Record Type of image</param>
		/// <param name="recordIndex">Index into records of this Record Type</param>
		/// <param name="imageData">Image to place in Tranaction File</param>
		/// <param name="inputFormat">Specifies format of source image file</param>
		/// <param name="outputFormat">Desired image format inside Transaction File</param>
		/// <param name="compression">Desired image compression, if it applies</param>
		/// <seealso cref="SetImageFromFile"/>
		/// <seealso cref="SetImage"/>
		/// <seealso cref="GetImageToFile"/>
		/// <seealso cref="GetImageToMem"/>
		/// <seealso cref="GetImage"/>
		public void SetImageFromMem(int recordType, int recordIndex, byte[] imageData,
			OpenEBTSImageFormat inputFormat, OpenEBTSImageFormat outputFormat, int compression)
		{
			OpenEBTSInteropWrap.SetImage(Handle, recordType, recordIndex, imageData, inputFormat, outputFormat, compression);
		}

		#region Enumeration

		/// <summary>
		/// Returns an enumerator that enumerates the OpenEBTSRecord items in the OpenEBTSFile.
		/// </summary>
		/// <returns>An IEnumerator</returns>
		public IEnumerator GetEnumerator()
		{
			return new OpenEBTSFile.Enumerator(this);
		}

		private class Enumerator : IEnumerator
		{
			private int currentIndex;
			private OpenEBTSFile EBTSFile;

			public Enumerator(OpenEBTSFile ebtsFile)
			{
				EBTSFile = ebtsFile;
				currentIndex = -1;
			}

			public object Current
			{
				get
				{
					// Note: OpenEBTS record, field, subfield and item indices are 1-based
					return EBTSFile.GetRecord(currentIndex + 1);
				}
			}

			public bool MoveNext()
			{
				return ++currentIndex < EBTSFile.RecordCount;
			}

			public void Reset()
			{
				currentIndex = -1;
			}
		}

		#endregion Enumeration

		public static string ImageFormatToFormatString(OpenEBTSImageFormat fmt)
		{
			return OpenEBTSInteropWrap.ToFormatString(fmt);
		}

		public static OpenEBTSImageFormat ImageFormatStringToFormat(string fmt)
		{
			return OpenEBTSInteropWrap.FromFormatString(fmt);
		}
	}
}
