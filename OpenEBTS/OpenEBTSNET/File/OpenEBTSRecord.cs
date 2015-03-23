using System;
using System.Collections;
using System.Collections.Generic;

namespace OpenEBTSNet
{
	/// <summary>
	/// Represents one of the records from a NIST Transaction File
	/// </summary>
	public class OpenEBTSRecord
	{
		private OpenEBTSFile EBTSFile;

		public OpenEBTSRecord(OpenEBTSFile ebtsfile, int recordType, int recordIndex)
		{
			EBTSFile = ebtsfile;
			RecordType = recordType;
			RecordIndex = recordIndex;
		}

		/// <summary>
		/// Record type of this record
		/// </summary>
		public int RecordType { get; private set; }

		/// <summary>
		/// Record index of this record in the set of all records of this Record Type
		/// </summary>
		/// <remarks>This index is 1-based</remarks>
		public int RecordIndex { get; private set; }

		/// <summary>
		/// Number of fields in this record
		/// </summary>
		public int FieldCount
		{
			get
			{
				return OpenEBTSInteropWrap.GetNumFields(EBTSFile.Handle, RecordType, RecordIndex);
			}
		}

		/// <summary>
		/// Retrieves one of the record's fields
		/// </summary>
		/// <param name="index">Index of desired field</param>
		/// <returns>Field at specified index</returns>
		/// <remarks>index must be between 1 and FieldCount</remarks>
		public OpenEBTSField GetField(int index)
		{
			// Because OpenEBTS only has an enumerator function and the field indexes can be non-continous
			// we need to enumerate till we reach the desired index.

			if (index >= 1 && index <= FieldCount)
			{
				int fieldIndex = 0;
				int numFields = 0;

				do
				{
					fieldIndex = OpenEBTSInteropWrap.GetNextField(EBTSFile.Handle, RecordType, RecordIndex, fieldIndex);
					numFields++;
				} while (fieldIndex != 0 && numFields != index);

				return new OpenEBTSField(EBTSFile, RecordType, RecordIndex, fieldIndex);
			}
			else
			{
				throw new IndexOutOfRangeException();
			}
		}

		#region Enumeration

		/// <summary>
		/// Returns an enumerator that enumerates the OpenEBTSField items in the OpenEBTSRecord.
		/// </summary>
		/// <returns>An IEnumerator</returns>
		public IEnumerator GetEnumerator()
		{
			return new Enumerator(this);
		}

		private class Enumerator : IEnumerator
		{
			private int currentIndex;
			private OpenEBTSRecord Record;

			public Enumerator(OpenEBTSRecord record)
			{
				Record = record;
				currentIndex = -1;
			}

			public object Current
			{
				get
				{
					// Note: OpenEBTS record, field, subfield and item indices are 1-based
					return Record.GetField(currentIndex + 1);
				}
			}

			public bool MoveNext()
			{
				return ++currentIndex < Record.FieldCount;
			}

			public void Reset()
			{
				currentIndex = -1;
			}
		}

		#endregion Enumeration
	}
}
