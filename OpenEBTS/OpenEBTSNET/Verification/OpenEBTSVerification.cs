
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;


namespace OpenEBTSNet
{
	/// <summary>
	/// OpenEBTSVerification manages a Verification File
	/// </summary>
	public class OpenEBTSVerification : IDisposable
	{
		internal SafeHandleVerification Handle = new SafeHandleVerification(IntPtr.Zero);

		private Dictionary<int, OpenEBTSFieldDefinition> FieldDefinitionDic { get; set; }
		/// <summary>
		/// Returns the Transaction Categories found in the Verification File
		/// </summary>
		public OpenEBTSTransactionCategories TransactionCategories { get; private set; }

		public OpenEBTSVerification()
		{
			TransactionCategories = null;
			FieldDefinitionDic = new Dictionary<int,OpenEBTSFieldDefinition>();
		}

		public void Dispose()
		{
			Free();
		}

		private void Free()
		{
			Handle.Dispose();
		}

		/// <summary>
		/// Loads a Verification File
		/// </summary>
		/// <param name="filePath">Path of Verification File to load</param>
		/// <returns>Returns true is loading was successful, otherwise false. In the failure case ParseError 
		/// will be set to an error string describing the problem encountered.</returns>
		/// <seealso cref="ParseError"/>
		public void LoadFromFile(string filePath)
		{
			Free();

			// Read the verification file
			Handle = OpenEBTSInteropWrap.ReadVerification(filePath);

			// Get the ver file categories
			string[] transactionCategories = OpenEBTSInteropWrap.GetTransactionCategories(Handle);
			TransactionCategories = new OpenEBTSTransactionCategories();
			foreach (string category in transactionCategories)
			{
				Debug.WriteLine(category);

				// Process the Transaction Category
				OpenEBTSTransactionCategory transactionCategory = new OpenEBTSTransactionCategory();
				transactionCategory.Name = category;

				// Get the transaction Types (C# Transactions)
				Tuple<string, string>[] transactionTypes = OpenEBTSInteropWrap.GetTransactionTypes(Handle, transactionCategory.Name);

				// Process each Transactions
				transactionCategory.Transactions = new OpenEBTSTransactions();
				foreach (Tuple<string, string> transactionType in transactionTypes)
				{
					Debug.WriteLine("\t" + transactionType);

					// Create a new OpenEBTSTransaction
					OpenEBTSTransaction transaction = new OpenEBTSTransaction(this);
					transaction.Name = transactionType.Item1;

					// Get the type occurrences from OpenEBTS and create a new OpenEBTSRecordoccurences in the OpenEBTSTransaction
					Tuple<int[], int[], int[]> recordTypeOccurences = OpenEBTSInteropWrap.GetRecordTypeOccurrences(Handle, transaction.Name);
					transaction.RecordOccurrences = new OpenEBTSRecordOccurrences();

					// Process each record occurences creating and adding a new OpenEBTSRecordOccurrence
					int index = 0;
					foreach(int recordType in recordTypeOccurences.Item1)
					{
						OpenEBTSRecordOccurrence occurrence = new OpenEBTSRecordOccurrence();
						occurrence.RecordType = recordType;
						occurrence.OccurrencesMin = recordTypeOccurences.Item2[index];
						occurrence.OccurrencesMax = recordTypeOccurences.Item3[index];
						transaction.RecordOccurrences.Add(occurrence);
						index++;
					}
					transaction.Description = transactionType.Item2;
					transactionCategory.Transactions.Add(transaction);
				}
				TransactionCategories.Add(transactionCategory);
			}
		}
	}
}
