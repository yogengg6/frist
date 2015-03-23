using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// Describes the Types of Transactions under a Transaction Category of a Verification File
	/// </summary>
	public class OpenEBTSTransactions : List<OpenEBTSTransaction>
	{

		/// <summary>
		/// Number of Transaction Types in this list
		/// </summary>
		public int TransactionCount
		{
			get
			{
				return Count;
			}
		}

		/// <summary>
		/// Retrieves the Transaction Type at the provided index
		/// </summary>
		/// <param name="index">Index of Transaction Type</param>
		/// <returns>Transaction Type descriptor</returns>
		/// <remarks> The index is 0-based</remarks>
		public OpenEBTSTransaction GetTransaction(int index)
		{
			return this[index];
		}
	}
}
