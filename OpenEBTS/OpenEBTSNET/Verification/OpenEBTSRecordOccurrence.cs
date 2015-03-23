using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// Describes limitations on the number of times a Record Type can be present in a Transaction File
	/// </summary>
	public class OpenEBTSRecordOccurrence
	{
		/// <summary>
		/// Maximum amount of times a record of type RecordType can be present in a Transaction File
		/// </summary>
		/// <remarks>
		/// An occurrence figure of -1 implies there is no maximum
		/// </remarks>
		public int OccurrencesMax { get; internal set; }

		/// <summary>
		/// Minimum amount of times a record of type RecordType can be present in a Transaction File
		/// </summary>
		/// <remarks>
		/// An occurrence figure of -1 implies there is no minimum
		/// </remarks>
		public int OccurrencesMin { get; internal set; }

		/// <summary>
		/// RecordType for which minimum and maximum occurrences are provided
		/// </summary>
		/// <remarks>
		/// The special RecordType 414 implies &quot;the sum of all records of Type-4 and Type-14&quot;, 
		/// and the special RecordType 713 implies &quot;the sum of all records of Type-3 and Type-13&quot;.
		/// </remarks>
		public int RecordType { get; internal set; }
	}
}
