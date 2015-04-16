using System.Collections.Generic;
using System.Diagnostics;
using OpenEBTSNet;
using System;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Drawing.Imaging;

namespace OpenEBTSNETSample
{
	class Program
	{
		private static string SamplesFolder;

		static void Main(string[] args)
		{
			SamplesFolder = @"C:\Users\Public\My Code\openbiometrics-code\OpenEBTS\OpenEBTSSamples\Samples";
			if (string.IsNullOrEmpty(SamplesFolder))
			{
				Console.WriteLine("You must set the environment variable OPENEBTSSAMPLESFOLDER to the folder containing the OpenEBTS sample files");
			}
			else
			{
				Console.WriteLine("Press enter to start...");
				Console.ReadLine();
				RunMultipleTests();
			}

			Console.WriteLine("Press enter to close...");
			Console.ReadLine();
		}

		private static void RunMultipleTests()
		{
			TestOpenEBTSFile_ReadAndWrite(GetFullPath("NISTFile2.ebts"));
			TestOpenEBTSFile_ReadAndVerify(GetFullPath("NISTFile2.ebts"), GetFullPath("EBTS9.1_ENUS.txt"));
			TestOpenEBTSFile_ReadAndVerify(GetFullPath("NISTFile3(bad).ebts"), GetFullPath("EBTS9.1_ENUS.txt"));
			TestOpenEBTSFile_ReadAndEnumerate(GetFullPath("NISTFile2.ebts"));
			TestOpenEBTSFile_CreateNew(GetFullPath("NewEBTSFile_out.ebts"), GetFullPath("EBTS9.1_ENUS.txt"));
			TestOpenEBTSFile_ObjectReferences(GetFullPath("NISTFile2.ebts"));

			TestOpenEBTSVerification(GetFullPath("EBTS9.1_ENUS.txt"));
			TestOpenEBTSVerification(GetFullPath("EBTS9.1_ENUS(bad).txt"));
			TestOpenEBTSVerification_ObjectReferences(GetFullPath("EBTS9.1_ENUS.txt"));
		}

		private static string GetFullPath(string filename)
		{
			return string.Format(@"{0}\{1}", SamplesFolder, filename);
		}

		private static void TestOpenEBTSFile_ReadAndWrite(string ebtsFilePath)
		{
			try
			{
				using (OpenEBTSFile ebtsFile = new OpenEBTSFile())
				{
					ebtsFile.LoadFromFile(ebtsFilePath);
					ConsoleWriteLoadSuccess("LoadFromFile", ebtsFilePath, ebtsFile);

					ebtsFile.WriteToFile(GetFullPath("NewEBTSFile(from file)_out.ebts"));
					Console.WriteLine("WriteToFile succeeded");
				}

				using (OpenEBTSFile ebtsFile = new OpenEBTSFile())
				{
					ebtsFile.LoadFromMem(File.ReadAllBytes(ebtsFilePath));
					ConsoleWriteLoadSuccess("LoadFromMem", ebtsFilePath, ebtsFile);

					byte[] ebtsMem = ebtsFile.WriteToMem();
					Console.WriteLine("WriteToMem succeeded");
					File.WriteAllBytes(GetFullPath("NewEBTSFile(from mem)_out.ebts"), ebtsMem);
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);				
			}
		}

		private static void TestOpenEBTSFile_ReadAndVerify(string ebtsFilePath, string verFilePath)
		{
			try
			{
				using (OpenEBTSFile ebtsFile = new OpenEBTSFile())
				{
					ebtsFile.LoadFromFile(ebtsFilePath);
					ConsoleWriteLoadSuccess("LoadFromFile", ebtsFilePath, ebtsFile);

					using (OpenEBTSVerification verFile = new OpenEBTSVerification())
					{
						verFile.LoadFromFile(verFilePath);
						ebtsFile.AssociateVerification(verFile);
						DoVerifyHelper(ebtsFile);
					}
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		private static void TestOpenEBTSFile_ReadAndEnumerate(string ebtsFilePath)
		{
			try
			{
				OpenEBTSFile ebtsFile = new OpenEBTSFile();

				ebtsFile.LoadFromFile(ebtsFilePath);
				ConsoleWriteLoadSuccess("LoadFromFile", ebtsFilePath, ebtsFile);

				foreach (OpenEBTSRecord record in ebtsFile)
				{
					Console.WriteLine(string.Format("Type-{0} record with index {1}", record.RecordType, record.RecordIndex));

					foreach (OpenEBTSField field in record)
					{
						foreach (OpenEBTSSubfield subfield in field)
						{
							foreach (OpenEBTSItem item in subfield)
							{
								Console.WriteLine(string.Format("  Field {0} : Subfield {1} : Item {2} : {3}", field.FieldIndex, subfield.SubfieldIndex, item.ItemIndex, item.Data));
							}
						}
					}

					// Any record with record type >= 4 contains an image, let's get some info about it too and write it to file
					// in three different ways
					if (record.RecordType >= 4)
					{
						OpenEBTSImageFormat format;
						int width;
						int height;
						int bitDepth;

						// Get the image into to a byte array exactly as-in in the EBTS file, then write it to file
						byte[] imageData = ebtsFile.GetImageToMem(record.RecordType, record.RecordIndex, out format, out width, out height, out bitDepth);
						string filePath = OutputImage(imageData, record.RecordType, record.RecordIndex, format, width, height, bitDepth);
						string filename = Path.GetFileName(filePath);
						Console.WriteLine(string.Format("  Image of format {0}, dimensions {1} x {2}, bit depth {3} written to {4}", format.ToString(), width, height, bitDepth, filename));

						// Get the image into a .NET Image, then to file as BMP
						Image image = ebtsFile.GetImage(record.RecordType, record.RecordIndex);
						Debug.Assert(image.Width == width);
						Debug.Assert(image.Height == height);
						image.Save(filePath + ".NETImage.BMP");

						// Finally, get the image directly to file as a PNG
						ebtsFile.GetImageToFile(record.RecordType, record.RecordIndex, filePath + ".ToFile.PNG", OpenEBTSImageFormat.imageFormatPNG);
					}
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		private static void TestOpenEBTSFile_CreateNew(string ebstFilePath, string verFilePath)
		{
			try
			{
				using (OpenEBTSFile ebtsFile = new OpenEBTSFile())
				{
					using (OpenEBTSVerification verFile = new OpenEBTSVerification())
					{
						ebtsFile.New("CAR");

						verFile.LoadFromFile(verFilePath);
						ebtsFile.AssociateVerification(verFile);

						// Set Type-1 fields. We don't need to add a Type-1 record, all EBTS files have one and exactly one of these
						ebtsFile.SetDataViaMnemonic("T1_DAT", 1, 1, "20080709");
						ebtsFile.SetDataViaMnemonic("T1_PRY", 1, 1, "4");
						ebtsFile.SetDataViaMnemonic("T1_DAI", 1, 1, "TEST00001");
						ebtsFile.SetDataViaMnemonic("T1_ORI", 1, 1, "TEST00001");
						ebtsFile.SetDataViaMnemonic("T1_TCN", 1, 1, "TEST-20080528173758-SOFT-0001-1C629");
						ebtsFile.SetDataViaMnemonic("T1_NSR", 1, 1, "19.69");
						ebtsFile.SetDataViaMnemonic("T1_NTR", 1, 1, "19.69");
						ebtsFile.SetDataViaMnemonic("T1_DOM_IMP", 1, 1, "EBTSTEST");
						ebtsFile.SetDataViaMnemonic("T1_DOM_VER", 1, 1, "NORAM");

						ebtsFile.AddRecord(2);

						ebtsFile.SetDataViaMnemonic("T2_RET", 1, 1, "Y");
						ebtsFile.SetDataViaMnemonic("T2_NAM", 1, 1, "LISA, MONA J");
						ebtsFile.SetDataViaMnemonic("T2_POB", 1, 1, "IT");
						ebtsFile.SetDataViaMnemonic("T2_DOB", 1, 1, "19750620");
						ebtsFile.SetDataViaMnemonic("T2_SEX", 1, 1, "F");
						ebtsFile.SetDataViaMnemonic("T2_RAC", 1, 1, "W");
						ebtsFile.SetDataViaMnemonic("T2_HGT", 1, 1, "510");
						ebtsFile.SetDataViaMnemonic("T2_WGT", 1, 1, "100");
						ebtsFile.SetDataViaMnemonic("T2_EYE", 1, 1, "BRO");
						ebtsFile.SetDataViaMnemonic("T2_HAI", 1, 1, "BLK");
						ebtsFile.SetDataViaMnemonic("T2_CRI", 1, 1, "DDNMI0039");
						ebtsFile.SetDataViaMnemonic("T2_DOA", 1, 1, "20080709");
						ebtsFile.SetDataViaMnemonic("T2_ASL_DOO", 1, 1, "20080709");
						ebtsFile.SetDataViaMnemonic("T2_ASL_AOL", 1, 1, "Armed Robbery");

						int newIndex;

						// Add an image record directly from a file
						newIndex = ebtsFile.AddRecord(4);
						ebtsFile.SetImageFromFile(4, newIndex, GetFullPath("finger8.jpg"), OpenEBTSImageFormat.imageFormatJPG,
							OpenEBTSImageFormat.imageFormatWSQ, 15);
						ebtsFile.SetDataViaMnemonic("T4_IMP", newIndex, 1, "0"); //0 = livescan
						ebtsFile.SetDataViaMnemonic("T4_FGP", newIndex, 1, "2"); //2 = right index

						// Add an image record from a byte array
						newIndex = ebtsFile.AddRecord(4);
						byte[] imageData = File.ReadAllBytes(GetFullPath("finger8.jpg"));
						ebtsFile.SetImageFromMem(4, newIndex, imageData, OpenEBTSImageFormat.imageFormatJPG,
							OpenEBTSImageFormat.imageFormatJP2, 15);
						ebtsFile.SetDataViaMnemonic("T4_IMP", newIndex, 1, "0"); //0 = livescan
						ebtsFile.SetDataViaMnemonic("T4_FGP", newIndex, 1, "2"); //2 = right index

						// Add an image record from a .NET Image
						newIndex = ebtsFile.AddRecord(4);
						Bitmap image = new Bitmap(GetFullPath("finger8.jpg"));
						ebtsFile.SetImage(4, newIndex, image, OpenEBTSImageFormat.imageFormatJPG, 15);
						ebtsFile.SetDataViaMnemonic("T4_IMP", newIndex, 1, "0"); //0 = livescan
						ebtsFile.SetDataViaMnemonic("T4_FGP", newIndex, 1, "2"); //2 = right index

						if (DoVerifyHelper(ebtsFile))
						{
							ebtsFile.WriteToFile(ebstFilePath);
						}
					}

				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		private static void TestOpenEBTSFile_ObjectReferences(string ebtsFilePath)
		{
			OutputRecords(ebtsFilePath);
			// The following line of code will force the OpenEBTSFile object created in GetRecords to be disposed
			// which in turn will call IWClose in OpenEBTS.dll which will release all allocated memory associated with
			// this EBTS file. This can only be properly verified by placing a breakpoint in directly in OpenEBTS.dll
			// at IWClose or placing a breakpoint right after this line and checking the verbose OpenEBTS logfile
			// for the "IWClose" log entry.
			GarbageCollect();
		}

		private static void OutputRecords(string ebtsFilePath)
		{
			try
			{
				// Get OpenEBTSRecord instances that each contain an OpenEBTSFile reference
				List<OpenEBTSRecord> records = GetRecords(ebtsFilePath);

				GarbageCollect();

				// Verify that OpenEBTSRecord instances still have valid OpenEBTSFile reference
				foreach (OpenEBTSRecord record in records)
				{
					Console.WriteLine(string.Format("Type-{0} record with index {1} has {2} fields", record.RecordType, record.RecordIndex, record.FieldCount));
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		private static List<OpenEBTSRecord> GetRecords(string ebtsFilePath)
		{
			List<OpenEBTSRecord> records = new List<OpenEBTSRecord>();

			try
			{
				OpenEBTSFile ebtsFile = new OpenEBTSFile();
				ebtsFile.LoadFromFile(ebtsFilePath);
				ConsoleWriteLoadSuccess("LoadFromFile", ebtsFilePath, ebtsFile);

				foreach (OpenEBTSRecord record in ebtsFile)
				{
					records.Add(record);
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			return records;
		}


		private static void TestOpenEBTSVerification(string verFilePath)
		{
			try
			{
				using (OpenEBTSVerification verFile = new OpenEBTSVerification())
				{
					verFile.LoadFromFile(verFilePath);
					OpenEBTSTransaction trans = verFile.TransactionCategories.GetTransactionCategory(0).Transactions.GetTransaction(12);
					OpenEBTSFieldDefinitions defs = trans.FieldDefinitions;
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		private static void TestOpenEBTSVerification_ObjectReferences(string verFilePath)
		{
			OutputTransactions(verFilePath);
			// The following line of code will force the OpenEBTSVerification object created in GetTransactions to be disposed
			// which in turn will call IWCloseVerification in OpenEBTS.dll which will release all allocated memory associated with
			// this verification file. This can only be properly verified by placing a breakpoint in directly in OpenEBTS.dll
			// at IWCloseVerification or placing a breakpoint right after this line and checking the verbose OpenEBTS logfile
			// for the "IWCloseVerification" log entry.
			GarbageCollect();
		}

		private static void OutputTransactions(string verFilePath)
		{
			try
			{
				// Get OpenEBTSTransaction instances that each contain an OpenEBTSVerification reference
				List<OpenEBTSTransaction> transactions = GetTransactions(verFilePath);

				GarbageCollect();

				// Verify that OpenEBTSTransaction instances still have valid OpenEBTSVerification reference
				foreach (OpenEBTSTransaction transaction in transactions)
				{
					Console.WriteLine(string.Format("Transaction: {0}", transaction.Name));
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		private static List<OpenEBTSTransaction> GetTransactions(string verFilePath)
		{
			List<OpenEBTSTransaction> transactions = new List<OpenEBTSTransaction>();

			try
			{
				OpenEBTSVerification verFile = new OpenEBTSVerification();
				verFile.LoadFromFile(verFilePath);

				// OpenEBTS has no way of iterating through all records, we need to ask it how many records of type X it has
				foreach (OpenEBTSTransactionCategory category in verFile.TransactionCategories)
				{
					foreach (OpenEBTSTransaction transaction in category.Transactions)
					{
						transactions.Add(transaction);
					}
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}

			return transactions;
		}

		#region Helper functions

		private static bool DoVerifyHelper(OpenEBTSFile ebtsFile)
		{
			if (ebtsFile.Verify())
			{
				Console.WriteLine("Verify of newly created EBTS file succeeded");

				return true;
			}
			else
			{
				Console.WriteLine("Verify of newly created EBTS file failed");
				foreach (string error in ebtsFile.VerifyErrors)
				{
					Console.WriteLine("\tVerify error: " + error);
				}

				return false;
			}
		}

		private static void ConsoleWriteLoadSuccess(string context, string ebtsFilePath, OpenEBTSFile ebtsFile)
		{
			Console.WriteLine(string.Format("{0} '{1}' succeeded: TOT {2} has {3} records",
				context, Path.GetFileName(ebtsFilePath), ebtsFile.TransactionType, ebtsFile.RecordCount.ToString()));
		}

		private static string OutputImage(byte[] imageData, int recordType, int recordIndex, OpenEBTSImageFormat format, int width, int height, int bitDepth)
		{
			string extension = OpenEBTSFile.ImageFormatToFormatString(format);
			string filename = string.Format("Image_{0}_{1}_{2}x{3}x{4}_out.{5}", recordType, recordIndex, width, height, bitDepth, extension);
			string filePath = GetFullPath(filename);

			File.WriteAllBytes(filePath, imageData);

			return filePath;
		}

		private static void GarbageCollect()
		{
			GC.Collect();
			GC.WaitForPendingFinalizers();
		}

		#endregion Helper functions
	}
}
