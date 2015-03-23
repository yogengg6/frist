using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// Describes the various Transaction Categories of a Verification File
	/// </summary>
	public class OpenEBTSTransactionCategories : List<OpenEBTSTransactionCategory>
	{
		/// <summary>
		/// Number of Transaction Categories in the Verification File
		/// </summary>
		public int TransactionCategoryCount
		{
			get
			{
				return Count;
			}
		}

		/// <summary>
		/// Retrieves the Transaction Category at the specified index
		/// </summary>
		/// <param name="index">Index of desired Transaction Category</param>
		/// <returns>Transaction Category at the given index</returns>
		/// <remarks>The index is 0-based</remarks>
		public OpenEBTSTransactionCategory GetTransactionCategory(int index)
		{
			return this[index];
		}
	}
}
