using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// Describes one Transaction Category of a Verification File
	/// </summary>
	public class OpenEBTSTransactionCategory
	{
		/// <summary>
		/// List of Transaction Types in this Transaction Category
		/// </summary>
		public OpenEBTSTransactions Transactions { get; internal set; }

		/// <summary>
		/// Name of the Transaction Category, e.g., &quot;Tenprint Fingerprint Identification Submissions&quot;
		/// </summary>
		public string Name { get; internal set; }

	}
}
