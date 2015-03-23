using System;
using System.Collections;

namespace OpenEBTSNet
{
	/// <summary>
	/// Represents one of the fields from a NIST record
	/// </summary>

	public class OpenEBTSField
	{
		private OpenEBTSFile EBTSFile;

		public OpenEBTSField(OpenEBTSFile ebtsFile, int recordType, int recordIndex, int fieldIndex)
		{
			EBTSFile = ebtsFile;
			RecordType = recordType;
			RecordIndex = recordIndex;
			FieldIndex = fieldIndex;
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
		/// Field Index this field holds in the NIST Transaction File
		/// </summary>
		public int FieldIndex { get; private set; }

		/// <summary>
		/// Number of subfields in this field
		/// </summary>
		public int SubfieldCount
		{
			get
			{
				return OpenEBTSInteropWrap.NumSubfields(EBTSFile.Handle, RecordType, RecordIndex, FieldIndex);
			}
		}

		/// <summary>
		/// Retrieves one of the field's subfields
		/// </summary>
		/// <param name="subfieldIndex">Index of desired subfield</param>
		/// <returns>Subfield at specified index</returns>
		/// <remarks>Index must be between 1 and SubfieldCount</remarks>
		public OpenEBTSSubfield GetSubfield(int subfieldIndex)
		{
			if (subfieldIndex >= 1 && subfieldIndex <= SubfieldCount)
			{
				return new OpenEBTSSubfield(EBTSFile, RecordType, RecordIndex, FieldIndex, subfieldIndex);
			}
			else
			{
				throw new IndexOutOfRangeException();
			}
		}

		#region Enumeration

		/// <summary>
		/// Returns an enumerator that enumerates the OpenEBTSSubfield items in the OpenEBTSField.
		/// </summary>
		/// <returns>An IEnumerator</returns>
		public IEnumerator GetEnumerator()
		{
			return new OpenEBTSField.Enumerator(this);
		}

		private class Enumerator : IEnumerator
		{
			private int currentIndex;
			private OpenEBTSField Field;

			public Enumerator(OpenEBTSField field)
			{
				Field = field;
				currentIndex = -1;
			}

			public object Current
			{
				get
				{
					// Note: OpenEBTS record, field, subfield and item indices are 1-based
					return Field.GetSubfield(currentIndex + 1);
				}
			}

			public bool MoveNext()
			{
				return ++currentIndex < Field.SubfieldCount;
			}

			public void Reset()
			{
				currentIndex = -1;
			}
		}

		#endregion Enumeration
	}
}
