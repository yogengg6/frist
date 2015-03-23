using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// List of optional or mandatory values that may be associated with a field
	/// </summary>
	public class OpenEBTSFieldValues : List<OpenEBTSFieldValue>
	{
		/// <summary>
		/// If set, the field must take on the value of one of the provided values in the list, otherwise 
		/// the list is only suggestive
		/// </summary>
		public bool IsMandatory { get; internal set; }

		/// <summary>
		/// Number of values
		/// </summary>
		public int FieldValueCount
		{
			get
			{
				return Count;
			}
		}

		/// <summary>
		/// If the name-value pairs are stored in an external file this property hold the filename
		/// </summary>
		public string ExternalFilename
		{
			get
			{
				return "";
			}
		}

		/// <summary>
		/// Retrieves value's definition from list
		/// </summary>
		/// <param name="index">Index of value</param>
		/// <returns>Value definition</returns>
		/// <remarks>Index is 0-based</remarks>
		public OpenEBTSFieldValue GetValue(int index)
		{
			return this[index];
		}
	}
}
