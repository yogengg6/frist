using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// Describes one Type of Transaction of a Verification File
	/// </summary>
	public class OpenEBTSTransaction
	{
		/// <summary>
		/// Description of Transaction Type, e.g., &quot;Criminal Tenprint Submission&quot;
		/// </summary>
		public string Description { get; internal set; }

		private OpenEBTSFieldDefinitions fieldDefinitions = null;
		private OpenEBTSVerification EBTSVerification { get; set; }
		
		public OpenEBTSTransaction(OpenEBTSVerification ebtsVerification)
		{
			EBTSVerification = ebtsVerification;
		}

		/// <summary>
		/// List of all optional and mandatory Field Definitions for this Type of Transaction
		/// </summary>
		public OpenEBTSFieldDefinitions FieldDefinitions 
		{ 
			get 
			{
				if (fieldDefinitions == null)
				{
					fieldDefinitions = GetFieldDefinitions(Name);
				}
				return fieldDefinitions;
			} 
			internal set 
			{
				fieldDefinitions = value;
			}
		}

		/// <summary>
		/// Name of Transaction Type, e.g., &quot;CAR&quot;
		/// </summary>
		public string Name { get; internal set; }

		/// <summary>
		/// Descriptor of record occurrence limitations for this Transaction Type
		/// </summary>
		public OpenEBTSRecordOccurrences RecordOccurrences { get; internal set; }

		private OpenEBTSFieldDefinitions GetFieldDefinitions(string transactionName)
		{
			// Get Transaction field definition
			OpenEBTSFieldDefinitions retVal = new OpenEBTSFieldDefinitions();
			Dictionary<string, OpenEBTSFieldDefinition> parentFields = new Dictionary<string, OpenEBTSFieldDefinition>();
			Tuple<string, string>[] fieldDefs = OpenEBTSInteropWrap.GetMnemonics(EBTSVerification.Handle, transactionName);

			// Process each field definitions
			foreach (Tuple<string, string> field in fieldDefs)
			{
				string locationIndex = OpenEBTSInteropWrap.GetLocactionIndex(EBTSVerification.Handle, transactionName, field.Item1);
				OpenEBTSFieldDefinition currentFieldDefinition = GetFieldDefinition(transactionName, field.Item1, field.Item2);

				// Process value list for this field deginition
				Tuple<bool, Tuple<string, string>[]> valueList = OpenEBTSInteropWrap.GetValueList(EBTSVerification.Handle, transactionName, currentFieldDefinition.Mnemonic);

				// Not forgetting the possible filename of where the values comes from
				currentFieldDefinition.ValuesFile = OpenEBTSInteropWrap.GetValueListFilename(EBTSVerification.Handle, transactionName, currentFieldDefinition.Mnemonic);
				currentFieldDefinition.Values = new OpenEBTSFieldValues();

				if (valueList.Item2.Length > 0)
				{
					currentFieldDefinition.Values.IsMandatory = valueList.Item1;
					foreach (Tuple<string, string> fieldValueTuple in valueList.Item2)
					{
						currentFieldDefinition.Values.Add(new OpenEBTSFieldValue(fieldValueTuple.Item1, fieldValueTuple.Item2));
					}
				}

				// Process possible child fields
				currentFieldDefinition.ItemList = null;
				if (IsParentField(currentFieldDefinition))
				{
					if (!parentFields.ContainsKey(TransformKey(currentFieldDefinition.LocationIndex)))
					{
						parentFields.Add(TransformKey(currentFieldDefinition.LocationIndex), currentFieldDefinition);
					}
				}

				if (IsSubField(currentFieldDefinition))
				{
					if (parentFields.ContainsKey(TransformKey(currentFieldDefinition.LocationIndex)))
					{
						OpenEBTSFieldDefinition parentField = parentFields[TransformKey(currentFieldDefinition.LocationIndex)];
						if (object.ReferenceEquals(parentField.ItemList, null))
						{
							parentField.ItemList = new OpenEBTSFieldDefinitions();
						}
						parentField.ItemList.Add(currentFieldDefinition);
					}
				}
				else
				{
					retVal.Add(currentFieldDefinition);
				}
			}
			return retVal;
		}

		private OpenEBTSFieldDefinition GetFieldDefinition(string transactionName, string fieldDefinitionMnemonic, string fieldDefinitionDescription)
		{
			OpenEBTSFieldRules fieldRules = OpenEBTSInteropWrap.GetRuleRestrictions(EBTSVerification.Handle, transactionName, fieldDefinitionMnemonic);
			OpenEBTSFieldDefinition fieldDefinition = new OpenEBTSFieldDefinition();

			fieldDefinition.AdvancedRule = fieldRules.advancedRule;
			fieldDefinition.DataTypeA = fieldRules.charType.Contains("A");
			fieldDefinition.DataTypeB = fieldRules.charType.Contains("B");
			fieldDefinition.DataTypeC = fieldRules.charType.Contains("C");
			fieldDefinition.DataTypeN = fieldRules.charType.Contains("N");
			fieldDefinition.DataTypeP = fieldRules.charType.Contains("P");
			fieldDefinition.DataTypeSET = fieldRules.charType.Equals("SET", StringComparison.InvariantCultureIgnoreCase);
			fieldDefinition.DateFormat = fieldRules.dateFormat;
			fieldDefinition.Description = fieldRules.longDesc;
			fieldDefinition.FieldIndex = fieldRules.field;
			fieldDefinition.FieldSizeMax = fieldRules.sizeMax;
			fieldDefinition.FieldSizeMin = fieldRules.sizeMin;
			fieldDefinition.Identifier = fieldDefinitionMnemonic.Substring(fieldDefinitionMnemonic.IndexOf('_') + 1);
			fieldDefinition.IsAutomaticallySet = fieldRules.automaticallySet == 0 ? false : true;
			fieldDefinition.IsDate = !string.IsNullOrEmpty(fieldDefinition.DateFormat);
			fieldDefinition.IsMandatory = fieldRules.mandatory == 0 ? false : true;
			fieldDefinition.ItemIndex = fieldRules.item;
			fieldDefinition.ItemList = null;
			fieldDefinition.LocationIndex = OpenEBTSInteropWrap.GetLocactionIndex(EBTSVerification.Handle, transactionName, fieldDefinitionMnemonic);
			fieldDefinition.Mnemonic = fieldDefinitionMnemonic;
			fieldDefinition.Name = fieldRules.desc;
			fieldDefinition.OccurrencesMax = fieldRules.occMax;
			fieldDefinition.OccurrencesMin = fieldRules.occMin;
			fieldDefinition.RecordType = fieldRules.recordType;
			fieldDefinition.SpecialChars = fieldRules.specialChars;
			fieldDefinition.TagNumber = "not yet supported";

			return fieldDefinition;
		}

		private static bool IsSubField(OpenEBTSFieldDefinition fieldDefintion)
		{
			return IsSubField(fieldDefintion.LocationIndex);
		}

		private static bool IsSubField(string input)
		{
			return !input.EndsWith(":", StringComparison.InvariantCultureIgnoreCase) && input.Contains("..") && !input.Contains("...");
		}

		private static bool IsParentField(OpenEBTSFieldDefinition fieldDefintion)
		{
			return fieldDefintion.LocationIndex.EndsWith(":", StringComparison.InvariantCultureIgnoreCase);
		}

		private static string TransformKey(string input)
		{
			if (input.EndsWith(":", StringComparison.InvariantCultureIgnoreCase))
			{
				return input.Substring(0, input.Length - 1);
			}
			if (IsSubField(input))
			{
				return input.Substring(0, input.IndexOf(".."));
			}
			return input;
		}
	}
}
