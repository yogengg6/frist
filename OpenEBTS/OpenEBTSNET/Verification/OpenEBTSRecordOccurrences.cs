using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// Describes limitations on the number of times Record Types can be present in a Transaction File
	/// </summary>
	public class OpenEBTSRecordOccurrences : List<OpenEBTSRecordOccurrence>
	{
		/// <summary>
		/// Number of OpenEBTSRecordOccurrence occurrence descriptors
		/// </summary>
		public int RecordOccurrenceCount
		{
			get
			{
				return Count;
			}
		}

		/// <summary>
		/// Retrieves an OpenEBTSRecordOccurrence occurrence descriptor from the list
		/// </summary>
		/// <param name="index">Index of desired descriptor</param>
		/// <returns>Record occurrence descriptor</returns>
		public OpenEBTSRecordOccurrence GetRecordOccurrence(int index)
		{
			return this[index];
		}
	}
}
