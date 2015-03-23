using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// List all NIST fields and their detailed rule definitions for a particular Transaction Type
	/// </summary>
	public class OpenEBTSFieldDefinitions : List<OpenEBTSFieldDefinition>
	{
		/// <summary>
		/// Number of Field Definitions in this list
		/// </summary>
		public int FieldDefinitionCount
		{
			get
			{
				return Count;
			}
		}

		/// <summary>
		/// Retrieves the Field Definition at the provided index
		/// </summary>
		/// <param name="index">Index of desired Field Definition</param>
		/// <returns>Field Definition</returns>
		/// <remarks>Index is 0-based</remarks>
		public OpenEBTSFieldDefinition GetFieldDefinition(int index)
		{
			return this[index];
		}
	}
}
