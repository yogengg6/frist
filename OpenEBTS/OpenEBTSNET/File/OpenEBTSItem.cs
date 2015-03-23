
using System.Collections;

namespace OpenEBTSNet
{
	/// <summary>
	/// Represents one of the items from a NIST subfield
	/// </summary>
	public class OpenEBTSItem
	{
		private OpenEBTSFile EBTSFile;

		public OpenEBTSItem(OpenEBTSFile ebtsFile, int recordType, int recordIndex, int fieldIndex, int subfieldIndex, int itemIndex)
		{
			EBTSFile = ebtsFile;
			RecordType = recordType;
			RecordIndex = recordIndex;
			FieldIndex = fieldIndex;
			SubfieldIndex = subfieldIndex;
			ItemIndex = itemIndex;
		}

		/// <summary>
		/// Record Type of parent record
		/// </summary>
		public int RecordType { get; private set; }

		/// <summary>
		/// Record index of parent record
		/// </summary>
		public int RecordIndex { get; private set; }

		/// <summary>
		/// Field Index of parent field
		/// </summary>
		public int FieldIndex { get; private set; }

		/// <summary>
		/// Subfield Index of parent subfield
		/// </summary>
		public int SubfieldIndex { get; private set; }

		/// <summary>
		/// Item Index this subfield holds in the NIST Transaction File
		/// </summary>
		public int ItemIndex { get; private set; }

		/// <summary>
		/// Item's text data
		/// </summary>
		public string Data
		{
			get
			{
				return OpenEBTSInteropWrap.FindItem(EBTSFile.Handle, RecordType, RecordIndex, FieldIndex, SubfieldIndex, ItemIndex);
			}
			set
			{
				OpenEBTSInteropWrap.SetItem(EBTSFile.Handle, RecordType, RecordIndex, FieldIndex, SubfieldIndex, ItemIndex, value);
			}
		}
	}
}
