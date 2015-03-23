using System;
using System.Collections;

namespace OpenEBTSNet
{
	/// <summary>
	/// Represents one of the subfields from a NIST field
	/// </summary>
	public class OpenEBTSSubfield : IEnumerable
	{
		private OpenEBTSFile EBTSFile;

		public OpenEBTSSubfield(OpenEBTSFile ebtsFile, int recordType, int recordIndex, int fieldIndex, int subfieldIndex)
		{
			EBTSFile = ebtsFile;
			RecordType = recordType;
			RecordIndex = recordIndex;
			FieldIndex = fieldIndex;
			SubfieldIndex = subfieldIndex;
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
		/// Subfield Index this subfield holds in the NIST Transaction File
		/// </summary>
		public int SubfieldIndex { get; private set; }

		/// <summary>
		/// Number of items in this Subfield
		/// </summary>
		public int ItemCount
		{
			get
			{
				return OpenEBTSInteropWrap.NumItems(EBTSFile.Handle, RecordType, RecordIndex, FieldIndex, SubfieldIndex);
			}
		}

		/// <summary>
		/// Retrieves one of the subfield's items
		/// </summary>
		/// <param name="index">Index of desired item</param>
		/// <returns>Item at specified index</returns>
		/// <remarks>Index must be between 1 and ItemCount</remarks>
		public OpenEBTSItem GetItem(int itemIndex)
		{
			if (itemIndex >= 1 && itemIndex <= ItemCount)
			{
				return new OpenEBTSItem(EBTSFile, RecordType, RecordIndex, FieldIndex, SubfieldIndex, itemIndex);
			}
			else
			{
				throw new IndexOutOfRangeException();
			}
		}

		#region Enumeration

		/// <summary>
		/// Returns an enumerator that enumerates the OpenEBTSItem items in the OpenEBTSSubfield.
		/// </summary>
		/// <returns>An IEnumerator</returns>
		public IEnumerator GetEnumerator()
		{
			return new OpenEBTSSubfield.Enumerator(this);
		}

		private class Enumerator : IEnumerator
		{
			private int currentIndex;
			private OpenEBTSSubfield Subfield;

			public Enumerator(OpenEBTSSubfield subfield)
			{
				Subfield = subfield;
				currentIndex = -1;
			}

			public object Current
			{
				get
				{
					// Note: OpenEBTS record, field, subfield and item indices are 1-based
					return Subfield.GetItem(currentIndex + 1);
				}
			}

			public bool MoveNext()
			{
				return ++currentIndex < Subfield.ItemCount;
			}

			public void Reset()
			{
				currentIndex = -1;
			}
		}

		#endregion Enumeration
	}
}
